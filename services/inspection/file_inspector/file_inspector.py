# This python script is developed for checking integrity of files with experimental and simulated data
# on all storages: DDC@DAQ - NCX@LHEP - CICC@LIT
import datetime
import os
import sys
import glob
import time
import json
import logging
import argparse
import psycopg2
import socket
import subprocess
from itertools import chain
from shutil import copyfile
try:
    import telnetlib
except ImportError as e:
    if "telnet" not in e.msg:
        raise

mail_address = "gertsen@jinr.ru"
logging.getLogger().setLevel(logging.INFO)


# load script with function set
#function_file_path = os.environ['VMCWORKDIR'] + "/services/inspection/function_set.py"
function_file_path = os.path.dirname(os.path.realpath(__file__)) + "/../function_set.py"
with open(function_file_path, "rb") as source_file:
    code = compile(source_file.read(), function_file_path, "exec")
exec(code) #exec(code, globals, locals)

# connect to the File Inspector database
config = json.load(open("file_inspector.json")) # load File Inspector database configuration from JSON
try:
    conn = psycopg2.connect(("dbname=%s user=%s host=%s password=%s") % (config["db_name"], config["db_user"], config["db_host"], config["db_pass"]))
except Exception as e:
    logging.error(f"Inspector Database connection failed: {e}")
    exit(1)
cursor = conn.cursor()
# get cluster and storage list for the parameters
cursor.execute("select cluster_name "
               "from cluster_")
conn.commit()
cursor_result = cursor.fetchall()
cluster_dictionary = list(chain(*cursor_result))
storage_dictionary_expanded = ["*"]
for row in cluster_dictionary:
    storage_dictionary_expanded.append(row+":"+"*")
cursor.execute("select cluster_name, storage_name "
               "from storage_")
conn.commit()
cursor_result = cursor.fetchall()
storage_dictionary = []
for row in cursor_result:
   storage_dictionary.append(row[0]+":"+row[1])
   storage_dictionary_expanded.append(row[0]+":"+row[1])
# define counters for final statistics
cnt_all_files = cnt_found_catalogue = cnt_fileinfo_error = cnt_mismatched_files = cnt_update_info = cnt_add_catalogue = cnt_missed_files = cnt_deleted_files = 0


# input argument parser
def argument_parser():
    parser = argparse.ArgumentParser(
        description='Script for checking integrity of experiment files on specified storages\
        For more information execute it with --help option'
    )
    parser.add_argument(
        '--start', '-s',
        nargs=1,
        type=str,
        help='cluster name, where the script is started',
        choices=cluster_dictionary,
        required=False
    )
    parser.add_argument(
        '--inspect', '-i',
        nargs=1,
        type=str,
        help='storage to inspect as "cluster_name:storage_name"',
        choices=storage_dictionary,
        required=True
    )
    """
    parser.add_argument(
        '--force', '-f',
        action='store_true',
        help='Rewrite existing destination file if exists but has another size or file checksum'
    )
    parser.add_argument(
        '--delete', '-del',
        action='store_true',
        help='Remove destination file if there is no corresponding source file'
    )"""

    parser.add_argument(
        '--add_cron', '-a',
        nargs=1,
        type=str,
        help='storage name, whose check is added to CRON',
        choices=storage_dictionary_expanded,
        required=False
    )

    args = parser.parse_args()
    start_cluster, inspect_storage, storage_2cron_address = args.start, args.inspect, args.add_cron
    if start_cluster is not None:
        start_cluster = start_cluster[0]
    if inspect_storage is not None:
        inspect_storage = inspect_storage[0]
    if storage_2cron_address is not None:
        storage_2cron_address = storage_2cron_address[0]

    return start_cluster,inspect_storage,storage_2cron_address

def add_failed_check(inspection_check_id, file_guid, error_type, error_details, conn):
    cursor = conn.cursor()
    logging.info(f'Inserting information on failed check #{inspection_check_id} for FGUID {file_guid}: error type = {error_type} (details: {error_details})\n')
    error_details = error_details.replace('\'', '"') # replace inner ' (single quotes) sybmol with " (double quotes)
    cursor.execute("insert into failed_check(check_id, file_guid, error_type_id, error_details) "
                   f"values ({inspection_check_id}, %s, {error_type}, '{error_details}')", (file_guid,))
    conn.commit()

# copy source file to a destination storage element
def transfer_file(source_cluster, source_storage, source_file_path, source_xrootd_url, source_file_size, source_file_checksum,
                  destination_cluster, destination_storage, destination_file_path, destination_xrootd_url,
                  transfer_protocol_code, source_ssh_prefix, destination_ssh_prefix, start_tmp_dir, conn):
    logging.info(f'Copying source file "{source_file_path}" at "{source_cluster}:{source_storage}" to the destination path "{destination_file_path}" at "{destination_cluster}:{destination_storage}"')
    # transfer_protocol_code:
    # 0 - local copy,
    # 1 - ssh copy to local,   2 - xrd copy to local,   3 - eos copy to local
    # 4 - ssh copy from local, 5 - xrd copy from local, 6 - eos copy from local
    # 7 - full ssh copy (scp), 8 - full xrootd copy,    9 - full eos copy
    # 10 - local to local copy through SSH connection, 11 - ssh copy to local through SSH conection
    # 12 - xrd copy to local through SSH connection, 13 - eos copy to local through SSH connection
    # 20 - local from local copy through SSH connection, 21 - ssh copy from local through SSH connection
    # 22 - xrd copy from local through SSH connection, 23 - eos copy from local through SSH connection
    if transfer_protocol_code == 0:
        destination_dir = os.path.dirname(destination_file_path)
        if not os.path.exists(destination_dir):
            os.makedirs(destination_dir)
        try:
            copyfile(source_file_path, destination_file_path)
        except Exception as e:
            logging.error(f"{source_file_path}: copying the local file to {destination_file_path} failed: {e}")
            return False
    elif transfer_protocol_code == 1:
        if cp_file_ssh(source_file_path, destination_file_path, source_ssh_prefix, '') == False:
            return False
    elif transfer_protocol_code == 2:
        if cp_file_xrd(source_file_path, destination_file_path, source_xrootd_url, '') == False:
            return False
    elif transfer_protocol_code == 3:
        if cp_file_eos(source_file_path, destination_file_path, source_xrootd_url, '') == False:
            return False
    elif transfer_protocol_code == 4:
        if cp_file_ssh(source_file_path, destination_file_path, '', destination_ssh_prefix) == False:
            return False
    elif transfer_protocol_code == 5:
        if cp_file_xrd(source_file_path, destination_file_path, '', destination_xrootd_url) == False:
            return False
    elif transfer_protocol_code == 6:
        if cp_file_eos(source_file_path, destination_file_path, '', destination_xrootd_url) == False:
            return False
    elif transfer_protocol_code == 7:
        if cp_file_ssh(source_file_path, destination_file_path, source_ssh_prefix, destination_ssh_prefix) == False:
            return False
    elif transfer_protocol_code == 8:
        if cp_file_xrd(source_file_path, destination_file_path, source_xrootd_url, destination_xrootd_url) == False:
            return False
    elif transfer_protocol_code == 9:
        if cp_file_eos(source_file_path, destination_file_path, source_xrootd_url, destination_xrootd_url) == False:
            return False
    elif transfer_protocol_code == 10:
        connection_ssh_prefix = destination_ssh_prefix if destination_ssh_prefix != "" else source_ssh_prefix
        cp_file_ssh_protocol(source_file_path, destination_file_path, connection_ssh_prefix, 0, '')
    elif transfer_protocol_code == 11:
        if source_ssh_prefix == '' or destination_ssh_prefix == '':
            logging.error("ssh copy to local through SSH has no SSH information")
            return False
        cp_file_ssh_protocol(source_file_path, destination_file_path, destination_ssh_prefix, 1, source_ssh_prefix, True)
    elif transfer_protocol_code == 12:
        if destination_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, destination_ssh_prefix, 2, source_xrootd_url, True)
        elif source_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, source_ssh_prefix, 2, destination_xrootd_url, False)
        else:
            logging.error("xrd copy to local through SSH has no SSH information")
            return False
    elif transfer_protocol_code == 13:
        if destination_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, destination_ssh_prefix, 3, source_xrootd_url, True)
        elif source_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, source_ssh_prefix, 3, destination_xrootd_url, False)
        else:
            logging.error("eos copy to local through SSH has no SSH information")
            return False
    elif transfer_protocol_code == 20:
        connection_ssh_prefix = source_ssh_prefix if source_ssh_prefix != "" else destination_ssh_prefix
        cp_file_ssh_protocol(source_file_path, destination_file_path, connection_ssh_prefix, 0, '')
    elif transfer_protocol_code == 21:
        if source_ssh_prefix == '' or destination_ssh_prefix == '':
            logging.error("ssh copy to local through SSH has no SSH information")
            return False
        cp_file_ssh_protocol(source_file_path, destination_file_path, source_ssh_prefix, 1, destination_ssh_prefix, False)
    elif transfer_protocol_code == 22:
        if source_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, source_ssh_prefix, 2, destination_xrootd_url, False)
        elif destination_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, destination_ssh_prefix, 2, source_xrootd_url, True)
        else:
            logging.error("xrd copy from local through SSH has no SSH information")
            return False
    elif transfer_protocol_code == 23:
        if source_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, source_ssh_prefix, 3, destination_xrootd_url, False)
        elif destination_ssh_prefix != '':
            cp_file_ssh_protocol(source_file_path, destination_file_path, destination_ssh_prefix, 3, source_xrootd_url, True)
        else:
            logging.error("eos copy from local through SSH has no SSH information")
            return False   
    else:
        logging.error(f"the transfer protocol (#{transfer_protocol_code}) is not supported")
        return False

    # transfer_protocol_code:
    # 20 - local from local copy through SSH connection, 21 - ssh copy from local through SSH connection
    # 22 - xrd copy from local through SSH connection, 23 - eos copy from local through SSH connection
    # protocol type: 0 - local file; 1 - SSH protocol; 2 - XRootD protocol; 3 - EOS protocol
    check_protocol_type = 0
    if transfer_protocol_code == 4 or transfer_protocol_code == 7 or transfer_protocol_code == 10 or transfer_protocol_code == 11:
        check_protocol_type = 1
    if transfer_protocol_code == 5 or transfer_protocol_code == 8: check_protocol_type = 2
    if transfer_protocol_code == 6 or transfer_protocol_code == 9: check_protocol_type = 3

    destination_file_size, destination_file_checksum, _, _ = get_file_info(destination_file_path, destination_xrootd_url, check_protocol_type, destination_ssh_prefix, start_tmp_dir, 1) # fast checksum after transfer
    if destination_file_size is None or destination_file_checksum == '':
        return False

    if destination_file_size != source_file_size:
        logging.error(f"{source_file_path}: transferring the file failed: destination and source file sizes are different: {destination_file_size} <> {source_file_size}")
        return False
    if (destination_file_checksum != source_file_checksum):
        logging.error(f"{source_file_path}: transferring the file failed: destination and source file checksums are different: {destination_file_checksum} <> {source_file_checksum}")
        return False

    logging.info('The source file has been successfully copied')
    
    logging.debug(f'Inserting a record for the transferred file with path = "{destination_file_path}", file size = {destination_file_checksum}, file_checksum = {destination_file_checksum}')
    cursor = conn.cursor()
    cursor.execute("insert into file_catalogue(cluster_name, storage_name, file_path, file_size, file_checksum) "
                   f"values ('{destination_cluster}', '{destination_storage}', '{destination_file_path}', {destination_file_size}, '{destination_file_checksum}')")
    conn.commit()
    logging.info("Information on the transferred file has been successfullly added to the database\n")
    return True

# checking integrity of files in the file list (comparing with the file catalogue in the database)
def check_files(file_list, catalogue_files, cluster_name, storage_name, xrootd_url, access_protocol, ssh_prefix, full_check, catalogue_matches, start_tmp_dir, conn, inspection_check_id):
    for cur_file in file_list:
        logging.debug(f"Current check: '{cur_file}' file")
        cnt_all_files += 1

        # searching for a file in the file catalogue with the same path
        for i, file_record in enumerate(catalogue_files):
            catalogue_file_guid, catalogue_file_path, _, _, _, _ = file_record
            if catalogue_file_path == cur_file:
                catalogue_matches[i] = 1
                cnt_found_catalogue += 1
                break
        # if no corresponding file in the File Catalogue
        else:
            file_record = None

        # current file at the storage found in the database (File Catalogue)
        if file_record is not None:
            logging.debug('The current file is present within the File Catalogue')
            catalogue_file_guid, catalogue_file_path, catalogue_file_size, catalogue_file_checksum, catalogue_replica, catalogue_file_matched = file_record

            match_checksum = 0
            if full_check or (catalogue_file_checksum is None):
                match_checksum = 2
            elif xrootd_url is not None:
                match_checksum = 1
            file_size, file_checksum, retcode, error_message = get_file_info(cur_file, xrootd_url, access_protocol, ssh_prefix, start_tmp_dir, match_checksum)
            # error while getting file information, file size or checksum
            if retcode != 0 or file_size is None or file_checksum is None:
                cnt_fileinfo_error += 1
                if retcode != 0: add_failed_check(inspection_check_id, catalogue_file_guid, retcode, error_message, conn)
                else:
                    logging.error(f"{cur_file}: critical error occured while accessing the file (retcode = {retcode}, file_size = {file_size}, file_checksum = {file_checksum}\n")
                continue

            cursor = conn.cursor()
            # compare file size with the catalogue value
            if catalogue_file_size is None:
                cursor.execute(f"update file_catalogue set file_size={file_size} where file_guid={catalogue_file_guid}")
                conn.commit()
                logging.debug(f'The size of the file has been updated in the catalogue: {cur_file}')
                cnt_update_info += 1
            else:
                if file_size != catalogue_file_size:
                    cnt_mismatched_files += 1
                    logging.error(f"{cur_file}: the file size is different from the catalogue value: {file_size} <> {catalogue_file_size}")
                    continue
                else:
                    logging.debug(f"The file size is equal to the catalogue value: {file_size}")
            # compare file checksum with the catalogue value if 'match_checksum' enabled
            if match_checksum != 0:
                if catalogue_file_checksum is None:
                    add_failed_check(inspection_check_id, catalogue_file_guid, 3, "", conn)
                    cursor.execute(f"update file_catalogue set file_checksum='{file_checksum}' where file_guid={catalogue_file_guid}")
                    conn.commit()
                    logging.debug('The checksum of the file has been updated in the catalogue\n')
                    cnt_update_info += 1
                else:
                    if file_checksum != catalogue_file_checksum:
                        cnt_mismatched_files += 1
                        logging.error(f"{cur_file}: the checksum of the file is different from the catalogue value ({file_checksum} <> {catalogue_file_checksum})")
                        add_failed_check(inspection_check_id, catalogue_file_guid, 4, "", conn)
                        continue
                    else:
                        logging.debug(f"The file checksum is equal to the catalogue value: {file_checksum}\n")
        # current file at the storage not found in the database (File Catalogue)
        else:
            logging.info(f"Adding file '{cur_file}' to the database")

            file_size, file_checksum, retcode, error_message = get_file_info(cur_file, xrootd_url, access_protocol, ssh_prefix, start_tmp_dir, 2) # manual checksum calculation when adding to the database
            if retcode != 0 or file_size is None or file_checksum is None:
                cnt_fileinfo_error += 1
                if retcode != 0: add_failed_check(inspection_check_id, None, retcode, error_message, conn)
                else:
                    logging.error(f"{cur_file}: critical error occured while accessing the file (retcode = {retcode}, file_size = {file_size}, file_checksum = {file_checksum}\n")
                continue

            # compare the obtained checksum with XRootD checksum for the file (if exists)
            xrd_file_checksum = ''
            if xrootd_url is not None:
                try:
                    xrd_file_checksum = get_file_xrdsum(cur_file, xrootd_url)
                except Exception as e:
                    cnt_fileinfo_error += 1
                    add_failed_check(inspection_check_id, None, 2, str(e), conn)
                    continue
            if file_checksum != xrd_file_checksum:
                cnt_mismatched_files += 1
                logging.error(f"{cur_file}: the сhecksum of the file is different from the XRootD storage value ({file_checksum} <> {xrd_file_checksum})")
                add_failed_check(inspection_check_id, None, 4, f"{cur_file}: the checksum of the file is different ({file_checksum} <> {xrd_file_checksum})", conn)
                continue

            logging.debug(f'Inserting a new file record with file path = "{cur_file}", file size = {file_size}, file_checksum = {file_checksum}')
            cursor = conn.cursor()
            cursor.execute("insert into file_catalogue(cluster_name, storage_name, file_path, file_size, file_checksum) "
                           f"values ('{cluster_name}', '{storage_name}', '{cur_file}', {file_size}, '{file_checksum}')")
            conn.commit()
            cnt_add_catalogue += 1
            logging.info("Information on the current file has been successfullly added to the database\n")

# define transfer protocol to update files between source storage and replica one
# return transfer_protocol_code:
# 0 - local copy,
# 1 - ssh copy to local,   2 - xrd copy to local,   3 - eos copy to local
# 4 - ssh copy from local, 5 - xrd copy from local, 6 - eos copy from local
# 7 - full ssh copy (scp), 8 - full xrootd copy,    9 - full eos copy
# 10 - local copy to local through SSH, 11 - ssh copy to local through SSH,
# 12 - xrd copy to local through SSH, 13 - eos copy to local through SSH
# 20 - local copy from local through SSH, 21 - ssh copy from local through SSH,
# 22 - xrd copy from local through SSH, 23 - eos copy from local through SSH
def define_transfer_protocol(so_transfer_protocol, si_transfer_protocol, oi_transfer_protocol, io_transfer_protocol):
    transfer_protocol_code = -1
    
    if si_transfer_protocol == 0:              # local copy
        if io_transfer_protocol is not None:
           return io_transfer_protocol
    elif so_transfer_protocol == 0:
        if oi_transfer_protocol is not None:
            if oi_transfer_protocol == 0:      # local copy
                return 0
            else:
                return 4 + oi_transfer_protocol

    if si_transfer_protocol > 1 and so_transfer_protocol > 1:
        if si_transfer_protocol == 3 and so_transfer_protocol == 3:    # both eos copy
            return 9
        else:
            return 8
    
    if si_transfer_protocol == 1:
        if so_transfer_protocol == 1:
            return 7
        elif io_transfer_protocol is not None:
            return 10 + io_transfer_protocol
    
    if so_transfer_protocol == 1:
        if oi_transfer_protocol is not None:
            return 20 + oi_transfer_protocol

    return transfer_protocol_code

# synchronize storages marked as replicas
def compare_storages(inspect_cluster, inspect_storage, inspect_storage_path, inspect_xrootd_url,
                     origin_cluster, origin_storage, origin_storage_path, origin_xrootd_url,  replica_flags,
                     start_cluster, start_tmp_dir, si_check_protocol, si_transfer_protocol, si_ssh_prefix, conn):
    logging.debug(f'Compare replica inspected storage "{inspect_cluster}:{inspect_storage}" and origin (source) storage "{replica_cluster}:{replica_storage}" for inspected path "{inspect_storage_path}" vs source path "{replica_storage_path}"')
    cursor = conn.cursor()

    add_file = ('a' in replica_flags)
    delete_file = ('d' in replica_flags)
    update_file = ('u' in replica_flags)

    transfer_protocol_code = -1
    if add_file or update_file:
        # get check and transfer protocol between inspected cluster and origin (source of the replica) storage
        cursor.execute("select check_protocol, transfer_protocol, ssh_prefix "
                       "from storage_communication "
                       f"where source_cluster_name='{inspect_cluster}' and destination_cluster_name='{origin_cluster}' and destination_storage_name='{origin_storage}'")
        conn.commit()
        io_check_protocol, io_transfer_protocol, io_ssh_prefix = cursor.fetchone()
        # get check and transfer protocol between origin cluster and inspected storage
        cursor.execute("select check_protocol, transfer_protocol, ssh_prefix "
                       "from storage_communication "
                       f"where source_cluster_name='{origin_cluster}' and destination_cluster_name='{inspect_cluster}' and destination_storage_name='{inspect_storage}'")
        conn.commit()
        oi_check_protocol, oi_transfer_protocol, oi_ssh_prefix = cursor.fetchone()
        # get check and transfer protocol between start cluster and origin storage
        cursor.execute("select check_protocol, transfer_protocol, ssh_prefix "
                       "from storage_communication "
                       f"where source_cluster_name='{start_cluster}' and destination_cluster_name='{origin_cluster}' and destination_storage_name='{origin_storage}'")
        conn.commit()
        so_check_protocol, so_transfer_protocol, so_ssh_prefix = cursor.fetchone()
        transfer_protocol_code = define_transfer_protocol(so_transfer_protocol, si_transfer_protocol, oi_transfer_protocol, io_transfer_protocol)
        if transfer_protocol_code == -1:
            logging.error(f"no communication protocol is defined to transfer files between '{origin_cluster}:{origin_storage}' and '{inspect_cluster}:{inspect_storage}' from '{start_cluster}'. "
                           "Please, specify it in the database")
            return

    # get a list of the files on the source of the replica storage from the File Catalogue
    cursor.execute("select file_guid, file_path, file_size, file_checksum, replica_validity, 0 "
                   "from file_catalogue "
                   f"where cluster_name='{origin_cluster}' and storage_name='{origin_storage}'")
    conn.commit()
    origin_files = cursor.fetchall()
    
    # get a list of the files in the File Fatalogue for the inspected storage
    cursor.execute("select file_guid, file_path, file_size, file_checksum, replica_validity, 0 "
                   "from file_catalogue "
                   f"where cluster_name='{inspect_cluster}' and storage_name='{inspect_storage}'")
    conn.commit()
    inspect_files = cursor.fetchall()
    
    file_matches = []
    file_matches = [-1 for i in range(len(inspect_files))]
    # cycle matching files between the storage elements using database records
    for iter_origin, cur_origin_file in enumerate(origin_files):
        origin_file_guid, origin_file_path, origin_file_size, origin_file_checksum, origin_replica, origin_file_matched = cur_origin_file

        logging.debug(f'Current check for origin file: "{origin_file_path}"')
        origin_file_end = origin_file_path.replace(origin_storage_path, '')

        # searching for a database record at the inspected storage with the same end of the file path
        for iter_inspect, cur_inspect_file in enumerate(inspect_files):
            inspect_file_guid, inspect_file_path, inspect_file_size, inspect_file_checksum, inspect_replica, inspect_file_matched = cur_inspect_file
            if inspect_file_path.endswith(origin_file_end):
                file_matches[iter_inspect] = iter_origin
                break
        # if no corresponding file on the inspected storage found, then transfer the missing file from the source of the replica
        else:
            if add_file:
                new_inspect_file_path = inspect_storage_path + origin_file_end
                logging.info(f"'{new_inspect_file_path}' file is missing at the inspected storage")
                #print(new_inspect_file_path, file=file_list)
                transfer_file(origin_cluster, origin_storage, origin_file_path, origin_xrootd_url, origin_file_size, origin_file_checksum,
                              inspect_cluster, inspect_storage, new_inspect_file_path, inspect_xrootd_url,
                              transfer_protocol_code, so_ssh_prefix, si_ssh_prefix, start_tmp_dir, conn)
            continue

        inspect_file_guid, inspect_file_path, inspect_file_size, inspect_file_checksum, inspect_replica, inspect_file_matched = cur_inspect_file
        # if there is a corresponding file found then check size
        if inspect_file_size != origin_file_size:
            if inspect_file_size is None:
                logging.error(f"file '{inspect_file_size}' on '{inspect_storage}' storage has no information on the size")
                continue
            logging.error(f"the source ({origin_file_path}) and inspected ({inspect_file_size}) files have different sizes: {origin_file_size} <> {inspect_file_size}")
            # if inspected file is different from the origin, then copy/replace the first inspected file if 'update' flag is active
            if update_file:
                logging.info(f"The inspected file at '{inspect_storage}' storage has an incorrect size and will be replaced")
                #print(inspect_file_path, file=file_list)
                transfer_file(origin_cluster, origin_storage, origin_file_path, origin_xrootd_url, origin_file_size, origin_file_checksum,
                              inspect_cluster, inspect_storage, new_inspect_file_path, inspect_xrootd_url,
                              transfer_protocol_code, so_ssh_prefix, si_ssh_prefix, start_tmp_dir, conn)
            continue
        # if there is a corresponding file found then check checksum
        if inspect_file_checksum != origin_file_checksum:
            if inspect_file_checksum is None:
                logging.error(f"file '{inspect_file_path}' on '{inspect_storage}' storage has no information on the checksum")
                continue
            logging.error(f"the source ({origin_file_path}) and inspected ({inspect_file_path}) files have different checksum: {origin_file_checksum} <> {inspect_file_checksum}")
            # if inspected file is different from the origin, then copy/replace the first inspected file if 'update' flag is active
            if update_file:
                logging.info(f"The inspected file at '{inspect_storage}' storage has an incorrect checksum and will be replaced")
                #print(inspect_file_path, file=file_list)
                transfer_file(origin_cluster, origin_storage, origin_file_path, origin_xrootd_url, origin_file_size, origin_file_checksum,
                              inspect_cluster, inspect_storage, new_inspect_file_path, inspect_xrootd_url,
                              transfer_protocol_code, so_ssh_prefix, si_ssh_prefix, start_tmp_dir, conn)
            continue

        logging.debug("The source and destination file are fully matched")

    logging.info(f"Comparison of inspected files on the '{inspect_storage}' storage with the origin '{origin_storage}' storage finished")

    # searching for inspected files that have no pair with the source replica
    #print("", file=file_list)
    for iter_inspect, cur_inspect_file in enumerate(inspect_files):
        inspect_file_guid, inspect_file_path, inspect_file_size, inspect_file_checksum, inspect_replica, inspect_file_matched = cur_inspect_file
        if file_matches[iter_inspect] == -1:
            logging.warning(f"{inspect_file_path}: the inspected file has no corresponding source file")
            #print(dest_file_path, file=file_list)
            if delete_file:
                cursor.execute(f"delete from file_catalogue where file_guid = {inspect_file_guid}")
                conn.commit()
                logging.info("The destination file with no corresponding source file has been deleted")

# send email with results
def send_email(from_address, mail_address, subject, body):
    # try send mail via 'mail' command
    try:
        command = f"echo '{body}' | mail -s '{subject}' -r '{from_address}' '{mail_address}'"
        result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("Email with summary results sent successfully")
    except subprocess.CalledProcessError as e:
        print(f"Failed to send email: {e.stderr.decode()}")
        # try send mail via 'Telnet' command
        print("Trying to send via Telnet...")
        with telnetlib.Telnet('mail.jinr.ru', 25) as tn:
            time.sleep(0.3)
            tn.write(b"HELO idev.jinr.ru\n")
            time.sleep(0.3)
            tn.write(f"MAIL FROM: {from_address}\n".encode())
            time.sleep(0.3)
            tn.write(f"RCPT TO: <{mail_address}>\n".encode())
            time.sleep(0.3)
            tn.write(b"DATA\n")
            time.sleep(0.3)
            tn.write(body.encode())
            time.sleep(0.3)
            tn.write(b"\r\n.\r\n")
            time.sleep(0.3)
            tn.write(b"quit\n")
            telnet_answer = tn.read_all()
            if "Error" in telnet_answer:
                print("Sending email with summary results failed")
                return False
            print("Email with summary results sent via Telnet")
    return True


# MAIN PYTHON FUNCTION
start_cluster, inspect_storage_address, storage_2cron_address = argument_parser()

# if 'start cluster' optional argument is absent then automatically define the current cluster by node prefix (the database column)
if start_cluster is None:
    cluter_host = socket.gethostname()
    cursor.execute("select cluster_name "
                   "from cluster_ "
                   f"where starts_with('{cluter_host}', node_prefix)")
    conn.commit()
    if cursor.rowcount != 1:
        logging.error(f"the start hostname ({cluter_host}) does not match the prefix of any of the clusters")
        exit(3)
    (start_cluster,) = cursor.fetchone()

# get information on the cluster, where the script has run
cursor.execute("select temp_dir, ssh_prefix "
               "from cluster_ "
               f"where cluster_name='{start_cluster}'")
conn.commit()
start_tmp_dir, start_ssh_prefix = cursor.fetchone()
if start_tmp_dir is None:
    logging.error(f"no start cluster found in the database for name: {start_cluster}")
    exit(2)

# if just create CRON task for the given storage
if storage_2cron_address is not None:
    current_script_path = os.path.realpath(__file__)
    
    if storage_2cron_address == "*":
        storage_2cron_cluster = "*"
        storage_2cron_storage = "*"
    else:
        storage_2cron_array = storage_2cron_address.split(":")
        storage_2cron_cluster = storage_2cron_array[0]
        storage_2cron_storage = storage_2cron_array[1]

    # get information on the storage (adding to CRON) from the File Inspector database
    storage_request = "select s.check_interval,s.cluster_name,s.storage_name from storage_ s "
    if storage_2cron_cluster != "*":
        storage_request += f"where s.cluster_name='{storage_2cron_cluster}'"
    if storage_2cron_storage != "*":
        storage_request += f" and s.storage_name='{storage_2cron_storage}'"

    cursor.execute(storage_request)
    conn.commit()
    storage_2cron_list = cursor.fetchall()
    if not storage_2cron_list:
        logging.error(f"{inspect_storage_address}: no storage found in the database to add to the CRON")
        exit(3)

    for storage_2cron_element in storage_2cron_list:
        process_output, process_error, process_returncode = get_process_full(f'crontab -l | {{ cat; echo "* * */{storage_2cron_element[0]} * * python3 {current_script_path} '
                                                                             f'-s {start_cluster} -i {storage_2cron_element[1]}:{storage_2cron_element[2]}"; }} | crontab -' )
        if process_returncode != 0:
            print(f'Error occured while adding job to UNIX CRON...\nOUTPUT: \n{process_output}\nERROR: \n{process_error}')
    exit(process_returncode)

# check inspect storage (required argument)
inspect_storage_array = inspect_storage_address.split(":")
inspect_cluster = inspect_storage_array[0]
inspect_storage = inspect_storage_array[1]

inspection_starttime = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
logging.info(f"Script for checking integrity of experiment files on the '{inspect_storage_address}' storage started")

# get information on the inspected storage from the File Inspector database
cursor.execute("select s.storage_path, s.storage_type, s.xrootd_url, s.check_interval, s.full_check, s.check_flags, s.replica_cluster, s.replica_storage, s.replica_flags, c.ssh_prefix "
               "from storage_ s join cluster_ c on s.cluster_name = c.cluster_name "
               f"where s.cluster_name='{inspect_cluster}' and s.storage_name='{inspect_storage}'")
conn.commit()
inspect_storage_path, inspect_storage_type, inspect_xrootd_url, inspect_storage_interval, inspect_storage_full, check_flags, replica_cluster, replica_storage, replica_flags, inspect_ssh_prefix = cursor.fetchone()
if inspect_storage_path is None:
    logging.error(f"{inspect_storage_address}: no storage found in the database")
    exit(3)

# get access protocol between the start cluster and inspection storage from the File Inspector database
# check_protocol: 0 = local, 1 = ssh, 2 = xrootd, 3 = eos   &&   transfer_protocol: 0 = cp, 1 = scp, 2 = xrootd, 3 = eos
cursor.execute("select check_protocol, transfer_protocol "
               "from storage_communication "
               f"where source_cluster='{start_cluster}' and destination_cluster='{inspect_cluster}' and destination_storage='{inspect_storage}'")
conn.commit()
if cursor.rowcount < 1:
    logging.error(f"no communication protocol is defined between '{start_cluster}' and '{inspect_storage_address}'. Please, specify it in the database")
    exit(4)
si_check_protocol, si_transfer_protocol = cursor.fetchone()

# define if the current inspection is fast (get checksum only if it provides by the storage) or full
full_check = False
cursor.execute("select count(*) as check_count "
               "from storage_check "
               f"where cluster_name='{inspect_cluster}' and storage_name='{inspect_storage}' and start_datetime > (select max(start_datetime) "
               "from storage_check "
               f"where cluster_name='{inspect_cluster}' and storage_name='{inspect_storage}' and full_check)")
conn.commit()
(fast_check_count,) = cursor.fetchone()
if fast_check_count >= (inspect_storage_full-1):
    full_check = True

# get a list of the files in the file catalogue for the storage element
cursor.execute("select file_guid, file_path, file_size, file_checksum, replica_validity, 0 "
               "from file_catalogue "
               f"where cluster_name='{inspect_cluster}' and storage_name='{inspect_storage}'")
conn.commit()
catalogue_files = cursor.fetchall()
catalogue_matches = []
catalogue_matches = [0 for i in range(len(catalogue_files))]

# insert information on the integrity check
catalogue_file_count = len(catalogue_files)
cursor.execute("insert into storage_check(cluster_name, storage_name, start_datetime, file_count, full_check) "
               f"values ('{inspect_cluster}', '{inspect_storage}', '{inspection_starttime}', {catalogue_file_count}, '{full_check}') RETURNING check_id")
inspection_check_id = cursor.fetchone()[0]
conn.commit()

# get and check full file list in the local directory
if si_check_protocol == 0:
    #result = [os.path.join(dp, f) for dp, dn, filenames in os.walk(watch_dir) for f in filenames if os.path.splitext(f)[1] == '.data']
    all_files = glob.glob(inspect_storage_path + '/**/*', recursive=True)
    all_files = [f for f in all_files if os.path.isfile(f)]   # remove directories from file list
    #print(list(all_files))
    if not all_files:
        logging.error(f"{inspect_storage_address}: there are no files on the storage")
        exit(5)
    check_files(all_files, catalogue_files,  inspect_cluster, inspect_storage, inspect_xrootd_url, 0, inspect_ssh_prefix, full_check, catalogue_matches, start_tmp_dir, conn, inspection_check_id)

# get and check full file list using SSH connection to the inspected storage machine
elif si_check_protocol == 1:
    ssh_failed=True
    while ssh_failed:
        process_output, process_error, returncode = get_process_full(f"tar cf - . | ssh {inspect_ssh_prefix} 'D=`mktemp -d`; tar xf - -C $D; $D/ssh_run.sh \"-s {inspect_cluster}\" \"-i {inspect_cluster}:{inspect_storage}\"'")
        process_output = process_output.strip()
        if process_error:
            logging.error(f'{inspect_storage_address}: execution via SSH failed for the destination storage: {returncode} - {process_error}')
            #continue
            exit(6)
        ssh_failed=False

# get and check full file list using XRootD commands to the inspected storage machine
elif si_check_protocol == 2:
    xrd_file_list = get_files_xrd(inspect_storage_path, inspect_xrootd_url)
    if not xrd_file_list:
        logging.error(f"{inspect_storage_address}: there are no files on the storage")
        exit(7)
    check_files(xrd_file_list, catalogue_files, inspect_cluster, inspect_storage, inspect_xrootd_url, 2, inspect_ssh_prefix, full_check, catalogue_matches, start_tmp_dir, conn, inspection_check_id)

# get and check full file list using EOS commands to the inspected storage machine
elif si_check_protocol == 3:
    eos_file_list = get_files_eos(inspect_storage_path, inspect_xrootd_url)
    if not eos_file_list:
        logging.error(f"{inspect_storage_address}: there are no files on the storage")
        exit(8)
    check_files(eos_file_list, catalogue_files, inspect_cluster, inspect_storage, inspect_xrootd_url, 3, inspect_ssh_prefix, full_check, catalogue_matches, start_tmp_dir, conn, inspection_check_id)

# searching for files in the File Catalogue that have no pair on the inspected storage element
delete_absent_files = ("d" in check_flags) # check_flags - 'd' means "delete absent files"
for i,file_record in enumerate(catalogue_files):
    if catalogue_matches[i] == 0:
        catalogue_file_guid, catalogue_file_path, _, _, _, _ = file_record
        logging.warning(f"{catalogue_file_path}: the record in the File Catalogue has no corresponding file on the storage element")
        cnt_missed_files += 1
        add_failed_check(inspection_check_id, catalogue_file_guid, 1, '', conn)
        # if File Catalogue record has no pair on the inspected storage and 'd' flag is set then delete the record
        if delete_absent_files:
            cursor.execute(f"delete from file_catalogue where file_guid = {catalogue_file_guid}")
            conn.commit()
            cnt_deleted_files += 1
            logging.info("The record of the absent file in the File Catalogue has been deleted")

# if the inspected storage element is replica, then compare it with the source (transfer_protocol: 0 = cp, 1 = scp, 2 = xrootd, 3 = eos)
if replica_cluster is not None:
    if "e" in replica_flags:
        # get information on a parent storage, for which the current one is replica
        cursor.execute("select s.storage_path, s.xrootd_url "
                       "from storage_ s "
                       f"where s.cluster_name = '{replica_cluster}' and s.storage_name = '{replica_storage}'")
        conn.commit()
        replica_storage_path, replica_xrootd_url = cursor.fetchone()
        compare_storages(inspect_cluster, inspect_storage, inspect_storage_path, inspect_xrootd_url,
                         replica_cluster, replica_storage, replica_storage_path, replica_xrootd_url,  replica_flags,
                         start_cluster, start_tmp_dir, si_check_protocol, si_transfer_protocol, inspect_ssh_prefix, conn)

inspection_endtime = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
cursor.execute(f"update storage_check set end_datetime='{inspection_endtime}' where check_id={inspection_check_id}")
conn.commit()

cursor.execute("select count(*) "
               "from failed_check "
               f"where check_id={inspection_check_id}")
found_errors = cursor.fetchone()[0]
conn.commit()
cursor.close()
conn.close()

# send email with resuls of the integrity check
from_address = "<inspector@jinr.ru>"
subject = "File Inspector - Integrity Results"
body = (f"File inspection on '{inspect_storage_address}' storage completed\n"
        f"Operation time: {inspection_starttime} - {inspection_endtime}\n"
        f"Number of errors: {found_errors}\n")
send_email(from_address, mail_address, subject, body)

logging.info(f"Script for checking integrity of experiment files on the '{inspect_storage_address}' storage finished")
logging.info("Statistics:")
logging.info(f" All checked files - {cnt_all_files}, Found files in the Catalogue - {cnt_found_catalogue}")
logging.info(f" Missed files - {cnt_missed_files}, Deleted files from the Catalogue - {cnt_deleted_files}")
logging.info(f" Get file info error - {cnt_fileinfo_error}\n Updated records (file info) - {cnt_update_info}")
logging.info(f" Mismatched records - {cnt_mismatched_files}\n Added files to the Catalogue - {cnt_add_catalogue}")
