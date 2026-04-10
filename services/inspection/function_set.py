# common functions to use in BM@N python scipts
import os
import sys
import string
import logging
import subprocess
import hashlib
import re
import zlib # for adler32

CHECKSUM_BLOCKSIZE=1024*1024*1024
storage_dictionary = ['ddc','ncx', 'cicc']

# tuning log of output messages
logging.basicConfig(
    format='%(asctime)s %(levelname)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    handlers=[
        logging.FileHandler("file_inspector.log", mode='w'),
        logging.StreamHandler(sys.stdout)
    ]
)

class CustomFormatter(logging.Formatter):
    """ Class for colouring output messages"""
    grey = "\x1b[38;20m"
    yellow = "\x1b[33;93m"
    red = "\x1b[33;91m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    format='%(asctime)s %(levelname)s: %(message)s'
    datefmt='%Y-%m-%d %H:%M:%S',
    FORMATS = {
        logging.DEBUG: grey + format + reset,
        logging.INFO: format,
        logging.WARNING: yellow + format + reset,
        logging.ERROR: red + format + reset,
        logging.CRITICAL: bold_red + format + reset
    }
    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)
# colouring messages only for output stream, not file
logging.getLogger().handlers[1].setFormatter(CustomFormatter())


class FileAccessException(Exception):
    """ Common exception while trying to get file attributes or content"""
    def __init__(self, message, error_code=None):
        self.message = message
        self.error_code = error_code
    def __str__(self):
        return str(self.message)


### SYSTEM PROCESS FUNCTIONS ###
# get process output
def get_process_output(process_string):
    output_lines = subprocess.run([process_string], stdout=subprocess.PIPE).stdout.splitlines()
    return output_lines

# get process return code
def get_process_retcode(process_string):
    child = subprocess.Popen(process_string, stdout=subprocess.PIPE)
    streamdata = child.communicate()[0]
    return child.returncode

# get process output, error and return code
def get_process_full(process_string):
    popen = subprocess.Popen(process_string, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    #process_output, process_error = popen.communicate()
    popen.wait()
    process_output = popen.stdout.read().decode("utf-8")
    process_error = popen.stderr.read().decode("utf-8")
    return process_output, process_error, popen.returncode


### GET FILE INFORMATION ###
# check whether file exists using SSH protocol
def check_file_exists_ssh(file_path, ssh_data):
    process_output, process_error, returncode = get_process_full(f"ssh {ssh_data} 'test -f {file_path} | echo $?'")
    process_output = process_output.strip()
    is_digits = bool(re.match('^[0-9]*$', process_output))
    if (not process_output) or (not is_digits) or process_error:
        logging.error(f'{file_path}: verifying the existence of the file using SSH failed: {returncode} - {process_error}')
        return None
    if int(process_output) == 0:
        return True
    else:
        return False

# check whether file exists using XRootD protocol, where absence is defined by output with "[ERROR]"
def check_file_exists_xrd(file_path, xrootd_url):
    process_output, process_error, returncode = get_process_full(f'xrdfs {xrootd_url} ls {file_path} | grep "\[ERROR]"')
    process_output = process_output.strip()
    if process_output or process_error:
        return False
    return True

# check whether file exists using EOS protocol
def check_file_exists_eos(file_path, xrootd_url):
    if get_process_retcode(f"eos {xrootd_url} stat -f {file_path}") == 0:
        return True
    else:
        return False

def get_file_sshsize(file_path, ssh_data):
    """ Get file size in bytes via SSH (in case of ssh-key access)

    :param file_path: file path
    :param ssh_data:  ssh login format: "user@host"
    :returns: file size in bytes
    :raises FileAccessException: if there is any error while getting file size (message + integer code)
    """
    process_output, process_error, returncode = get_process_full(f"ssh {ssh_data} 'du -b {file_path} | cut -f1'")
    process_output = process_output.strip()
    is_digits = bool(re.match('^[0-9]*$', process_output))
    if (not process_output) or (not is_digits) or process_error:
        logging.error(f'{file_path}: obtaining the file size using SSH failed: {returncode} - {process_error}')
        raise FileAccessException(process_error, returncode)
    return int(process_output)

def get_file_xrdsize(file_path, xrootd_url):
    """ Get file size in bytes via XRootD

    :param file_path: file path
    :param xrootd_url:  XRootD URL, e.g. 'root://eos.jinr.ru//eos/nica/bmn...'
    :returns: file size in bytes
    :raises FileAccessException: if there is any error while getting file size (message + integer code)
    """
    process_output, process_error, returncode = get_process_full(f"xrdfs {xrootd_url} stat {file_path} | grep Size | awk -F ' ' '{{print $2}}'")
    process_output = process_output.strip()
    is_digits = bool(re.match('^[0-9]*$', process_output))
    if (not process_output) or (not is_digits) or process_error:
        logging.error(f"{file_path}: obtaining the file size using 'xrdfs' failed: {returncode} - {process_error}")
        raise FileAccessException(process_error, returncode)
    return int(process_output)

def get_file_eossize(file_path, xrootd_url):
    """ Get file size in bytes via EOS

    :param file_path: file path
    :param xrootd_url:  XRootD/EOS URL, e.g. 'root://eos.jinr.ru//eos/nica/bmn...'
    :returns: file size in bytes
    :raises FileAccessException: if there is any error while getting file size (message + integer code)
    """
    process_output, process_error, returncode = get_process_full(f"eos {xrootd_url} fileinfo {file_path} --size | awk '{{print $2}}'")
    process_output = process_output.strip()
    is_digits = bool(re.match('^[0-9]*$', process_output))
    if (not process_output) or (not is_digits) or process_error:
        logging.error(f"{file_path}: obtaining the file size using 'eos' failed: {returncode} - {process_error}")
        raise FileAccessException(process_error, returncode)
    return int(process_output)

def get_file_MD5c(file_path):
    """ Get file MD5 checksum (read splitted by chunks)

    :param file_path: file path
    :returns: MD5 file checksum
    :raises FileAccessException: if there is any error while getting file checksum (message + integer code)
    """
    file_md5 = hashlib.md5()
    try:
        f = open(file_path, "rb")
        for chunk in iter(lambda: f.read(CHECKSUM_BLOCKSIZE), b""):
            file_md5.update(chunk)
        return file_md5.hexdigest()
    except FileNotFoundError as e:
        logging.error(f"{file_path}: the file is not accessible to calculate MD5 checksum (exception: {e})")
        raise FileAccessException(str(e), 1)
    except Exception as e:
        logging.error(f"{file_path}: the file is not accessible to calculate MD5 checksum (exception: {e})")
        raise FileAccessException(str(e), 2)

def get_file_MD5a(file_path):
    """ Get file MD5 checksum (read all at once)

    :param file_path: file path
    :returns: MD5 file checksum
    :raises FileAccessException: if there is any error while getting file checksum (message + integer code)
    """
    try:
        a_file = open(file_path, "rb")
        file_content = a_file.read()
        file_md5 = hashlib.md5(file_content).hexdigest()
        return file_md5
    except FileNotFoundError as e:
        logging.error(f"{file_path}: the file is not accessible to calculate MD5 checksum (exception: {e})")
        raise FileAccessException(str(e), 1)
    except Exception as e:
        logging.error(f"{file_path}: the file is not accessible to calculate MD5 checksum (exception: {e})")
        raise FileAccessException(str(e), 2)

def get_file_adler32c(file_path):
    """ Get file Adler32 checksum (read splitted by chunks)

    :param file_path: file path
    :returns: Adler32 file checksum
    :raises FileAccessException: if there is any error while getting file checksum (message + integer code)
    """
    adler_sum = 1
    try:
        with open(file_path,"rb") as f:
            while True:
                data = f.read(CHECKSUM_BLOCKSIZE)
                if not data:
                    break
                adler_sum = zlib.adler32(data, adler_sum)
                if adler_sum < 0:
                    adler_sum += 2**32
        return hex(adler_sum)[2:10].zfill(8).lower()
    except FileNotFoundError as e:
        logging.error(f"{file_path}: the file is not accessible to calculate Adler32 checksum (exception: {e})")
        raise FileAccessException(str(e), 1)
    except Exception as e:
        logging.error(f"{file_path}: the file is not accessible to calculate Adler32 checksum (exception: {e})")
        raise FileAccessException(str(e), 2)

def get_file_adler32a(file_path):
    """ Get file Adler32 checksum (read whole file at once)

    :param file_path: file path
    :returns: Adler32 file checksum
    :raises FileAccessException: if there is any error while getting file checksum (message + integer code)
    """
    BLOCKSIZE=256*1024*1024
    file_md5 = 1
    try:
        with open(file_path,"rb") as f:
            while True:
                data = f.read(BLOCKSIZE)
                if not data:
                    break
                file_md5 = zlib.adler32(data, file_md5)
                if file_md5 < 0:
                    file_md5 += 2**32
        return hex(file_md5)[2:10].zfill(8).lower()
    except FileNotFoundError as e:
        logging.error(f"{file_path}: the file is not found to calculate Adler32 checksum (exception: {e})")
        raise FileAccessException(str(e), 1)
    except Exception as e:
        logging.error(f"{file_path}: the file is not accessible to calculate Adler32 checksum (exception: {e})")
        raise FileAccessException(str(e), 2)

def get_file_xrdsum(file_path, xrootd_url):
    """ Get file checksum via XRootD

    :param file_path: file path
    :param xrootd_url:  XRootD/EOS URL, e.g. 'root://eos.jinr.ru//eos/nica/bmn...'
    :returns: file checksum (Adler32 at the NICA storages)
    :raises FileAccessException: if there is any error while getting file checksum (message + integer code)
    :raises ValueError: if returned checksum has incorrect format (non-HEX value)
    """
    process_output, process_error, returncode = get_process_full(f"xrdfs {xrootd_url} query checksum {file_path} | awk -F ' ' '{{print $2}}'")
    process_output = process_output.strip()
    is_hex = all(c in string.hexdigits for c in process_output)
    if (not process_output) or process_error:
        logging.error(f"{file_path}: query of the file checksum using 'xrdfs' failed: {returncode} - {process_error}")
        raise FileAccessException(process_error, returncode)
    if not is_hex:
        logging.error(f"{file_path}: query of the file checksum using 'xrdfs' returned non-HEX value: {process_output}")
        raise ValueError(f"{file_path}: query of the file checksum using 'xrdfs' returned non-HEX value: {process_output}")
    return process_output


#       
def get_file_info(file_path, storage_url, protocol_type, ssh_con, start_tmp_dir, match_checksum = 0):
    """ Get file information (size and checksum) using different protocols (in case of errors: file_size and/or file_checksum is None)

    :param file_path: input file path to get its information
    :param storage_url: storage URL, where the input file is located
    :param protocol type: 0 - local file; 1 - SSH protocol; 2 - XRootD protocol; 3 - EOS protocol
    :param ssh_con: SSH prefix as 'user@server'
    :param start_tmp_dir: temporary directory to copy file for checking
    :param match_checksum: 0 - do not match checksums, 1 - fast comparing checksum if it exists in the File System, 2- always manual recalculating checksum before comparing
    :returns: 
       - 1 - file size (integer, in bytes);
       - 2 - file checksum (string);
       - 3 - error code (0 - no errors, 1 - file not found, 2 - file read error, 3 - error while getting checksum, <0 - internal errors)
       - 4 - error message (in case of any errors)
    """
    match_sum = {
        0: 'no', 
        1: 'fast',
        2: 'full'}
    logging.debug(f"Get file information locating at \"{file_path}\" (url: {storage_url}, match checksum: {match_sum.get(match_checksum, 'no')})")
    
    # check destination file size
    logging.debug(f'Checking the file size (protocol = {protocol_type})...')
    file_size = None
    if protocol_type == 0 or (protocol_type == 1 and ssh_con == ''):
        try:
            file_size = os.path.getsize(file_path)
        except OSError as error:
            logging.error(f"{file_path}: the file is not accessible to get its size")
            return None, None, 2, f"{file_path}: the file is not accessible to get its size"
    elif protocol_type == 1:
        try:
            file_size = get_file_sshsize(file_path, ssh_con)
        except Exception as e:
            return None, None, 2, str(e)
    elif protocol_type == 2:
        try:
            file_size = get_file_xrdsize(file_path, storage_url)
        except Exception as e:
            return None, None, 2, str(e)
    elif protocol_type == 3:
        try:
            file_size = get_file_eossize(file_path, storage_url)
        except Exception as e:
            return None, None, 2, str(e)
    else:
        logging.error(f"{file_path}: transfer protocol (#{protocol_type}) is not supported")
        return None, None, 2, f"{file_path}: transfer protocol (#{protocol_type}) is not supported"
    
    if file_size == None:
        logging.error(f"{file_path}: the file size is not defined")
        return None, None, 2, f"{file_path}: the file size is not defined"

    if match_checksum == 0:
        return file_size, '', 0, ''
    # check destination file checksum
    logging.debug('Checking the file checksum')
    file_checksum = ''
    if match_checksum == 1 and storage_url is not None:
        if check_file_exists_xrd(file_path, storage_url):
            try:
                file_checksum = get_file_xrdsum(file_path, storage_url)
            except Exception as e:
                return file_size, None, 2, str(e)
        else:
            return file_size, None, 1, f"File not found using XRootD: {file_path}"
    else:
        if protocol_type == 0 or (protocol_type == 1 and ssh_con == ''):
            try:
                file_checksum = get_file_adler32c(file_path)
            except Exception as e:
                return file_size, None, 2, str(e)
        elif protocol_type == 1 or protocol_type == 2 or protocol_type == 3:
            local_file_copy = start_tmp_dir + '/' + os.path.basename(file_path)
            if protocol_type == 1:
                cp_file_ssh(file_path, local_file_copy, ssh_con)
            elif protocol_type == 2:
                #file_checksum = get_file_xrdsum(destination_file_path, destination_storage_url)
                cp_file_xrd(file_path, storage_url, local_file_copy, None)
            else:
                #file_checksum = get_file_xrdsum(destination_file_path, destination_storage_url)
                cp_file_eos(file_path, storage_url, local_file_copy, None)
            try:
                file_checksum = get_file_adler32c(local_file_copy)
            except Exception as e:
                return file_size, None, 2, str(e)
            os.remove(local_file_copy)
        else:
            logging.error(f"{file_path}: the transfer protocol (#{protocol_type}) is not supported")
            return file_size, None, 2, f"{file_path}: the transfer protocol (#{protocol_type}) is not supported"

    if file_checksum == '':
        logging.error(f"{file_path}: the checksum of the file is not defined")
        #print(f'no checksum: match_checksum = {match_checksum}, storage_url = {storage_url}, protocol_type = {protocol_type}')
        return file_size, None, 3, f"{file_path}: the checksum of the file is not defined"

    return file_size, file_checksum, 0, ''

### GET FILE LIST ###
# list XRootD directory
def ls_dir_xrd(file_path, xrootd_url):
    xrd_ls_lines = subprocess.run([f'xrdfs', '{xrootd_url} ls {file_path}'], stdout=subprocess.PIPE).stdout.splitlines()
    return xrd_ls_lines

# list EOS directory with appending indicator '/' to directories at the end
def ls_dir_eos(file_path, xrootd_url):
    eos_ls_lines = subprocess.run(['eos', f'{xrootd_url} ls -F {file_path}'], stdout=subprocess.PIPE).stdout.splitlines()
    return eos_ls_lines

# get file list in XRootD directory recursively
def get_files_xrd(dir, xrootd_url):
    files = list()
    for item in ls_dir_xrd(dir, xrootd_url):
        try:
            if get_process_retcode(f"xrdfs {xrootd_url} stat -q IsDir {item} &> /dev/null") == 0:
                files = files + get_files_xrd(item)
            else:
                files.append(item)
        except FileNotFoundError as err:
            logging.error(f'invalid directory ({item}) encountered while listing "{dir}" directory')
    return files

# get file list in EOS directory recursively
def get_files_eos(dir, xrootd_url):
    if not dir.endswith('/'): dir += '/'

    files = list()
    for item in ls_dir_eos(dir, xrootd_url):
        abspath = dir + item
        try:
            if abspath.endswith('/'):
                files = files + get_files_eos(abspath)
            else:
                files.append(item)
        except FileNotFoundError as err:
            logging.error(f'invalid directory ({item}) encountered while listing "{dir}" directory')
    return files

### COPY FILE ###
# copy file via SCP (in case of ssh-key access). ssh_prefix format: "user@host"
def cp_file_ssh(source_file_path, destination_file_path, source_ssh_prefix, destination_ssh_prefix):
    full_source_file = source_file_path
    if source_ssh_prefix != '': full_source_file = source_ssh_prefix + ":" + source_file_path
    full_destinaton_file = destination_file_path
    if destination_ssh_prefix != '': full_destinaton_file = destination_ssh_prefix + ":" + destination_file_path

    # create destination directory (if not exist)
    destination_dir = os.path.dirname(destination_file_path)
    if destination_ssh_prefix != '':
        process_output, process_error, returncode = get_process_full(f"ssh {destination_ssh_prefix} 'mkdir -p {destination_dir}'")
        if process_output or process_error:
            logging.error(f'{destination_dir}: creating the destination folder failed: {returncode} - {process_error} (output: {process_output})')
            return False
    else:
        if not os.path.exists(destination_dir):
            os.makedirs(destination_dir) # ,0o755,True

    process_output, process_error, returncode = get_process_full(f"scp -B -p {full_source_file} {full_destinaton_file}") # -B batch mode (not asking for passwords), -p preserves times from origin
    if process_output or process_error:
        logging.error(f'scp command failed: {returncode} - {process_error} (output: {process_output})')
        return False
    return True

# copy file using specified protocol via SSH connection (in case of ssh-key access). ssh_prefix format: "user@host"
# protocol type: 0 - local copy after SSH connection, 1 - scp copy -//-, 2 - xrd copy -//-, 3 - eos copy -//-
# copy_prefix: prefix for a specified transfer protocol type
# is_source_copy_prefix: whether use copy_prefix for source or destination file path (true = prefix:source, false = prefix:destination)
def cp_file_ssh_protocol(source_file_path, destination_file_path, connection_ssh_prefix,  transfer_protocol,
                         copy_prefix, is_source_copy_prefix = False):
    if connection_ssh_prefix == '':
        logging.error('ssh prefix for the first connection is not defined')
        return False
    
    copy_file_line = f"{copy_prefix}:{source_file_path} {destination_file_path}" if is_source_copy_prefix \
                else f"{source_file_path} {copy_prefix}:{destination_file_path}"
    
    command_line = f'ssh {connection_ssh_prefix} "'
    if transfer_protocol == 0:
        destination_dir = os.path.dirname(destination_file_path)
        command_line += f'mkdir -p {destination_dir};\cp -f {source_file_path} {destination_file_path}"'
    elif transfer_protocol == 1:
        destination_dir = os.path.dirname(destination_file_path)
        command_line += f'ssh {connection_ssh_prefix} mkdir -p {destination_dir}; scp -B -p {copy_file_line}"'
    elif transfer_protocol == 2:
        command_line += f'xrdcp --nopbar {copy_file_line}"'
    elif transfer_protocol == 3:
        command_line += f'eos cp -p -n -P -S {copy_file_line}"'

    process_output, process_error, returncode = get_process_full(command_line)
    if process_output or process_error:
        logging.error(f'copy command via SSH failed: {returncode} - {process_error} (output: {process_output})')
        return False
    return True

# copy file via XRootD
def cp_file_xrd(source_file_path, destination_file_path, source_storage_url, destination_storage_url):
    full_source_file = source_file_path
    if source_storage_url is not None: full_source_file = source_storage_url + "/" + source_file_path
    full_destinaton_file = destination_file_path
    if destination_storage_url is not None: full_destinaton_file = destination_storage_url + "/" + destination_file_path

    process_output, process_error, returncode = get_process_full(f"xrdcp --nopbar {full_source_file} {full_destinaton_file}")
    if process_output or process_error:
        logging.error(f'xrdcp command failed: {returncode} - {process_error} (output: {process_output})')
        return False
    return True

# copy file via EOS
def cp_file_eos(source_file_path, destination_file_path, source_storage_url, destination_storage_url):
    full_source_file = source_file_path
    if source_storage_url is not None: full_source_file = source_storage_url + "/" + source_file_path
    full_destinaton_file = destination_file_path
    if destination_storage_url is not None: full_destinaton_file = destination_storage_url + "/" + destination_file_path

    process_output, process_error, returncode = get_process_full(f"eos cp -p -n -P -S {full_source_file} {full_destinaton_file}")
    if process_output or process_error:
        logging.error(f'eos cp command failed: {returncode} - {process_error} (output: {process_output})')
        return False
    return True

### REMOVE FILE ###
# remove remote file via XRootD
def rm_file_xrd(file_path, xrootd_url):
    process_output, process_error, returncode = get_process_full(f"xrdfs {xrootd_url} rm {file_path} && xrdfs {xrootd_url} ls {file_path} | grep [ERROR]") # "xrdfs {xrootd_url} rm {file_path}"
    process_output = process_output.strip()
    if (not process_error) and (not process_output):
        logging.error(f"{file_path}: removing the file using 'xrdfs rm' failed': {returncode} - {process_error} (output: {process_output})")
        return False
    return True
