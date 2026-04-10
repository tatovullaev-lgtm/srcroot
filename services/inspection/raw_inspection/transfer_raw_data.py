# This python script is developed for matching raw data files between storages: DDC/DAQ -> NCX -> CICC/MICC
import sys
import json
import logging
import argparse
import psycopg2
#from concurrent.futures import ThreadPoolExecutor
import function_set

logging.getLogger().setLevel(logging.INFO)

# input argument parser
def argument_parser():
    parser = argparse.ArgumentParser(
        description='Script for transferring missing raw files to a destination storage\
        For more information run with --help option'
    )
    parser.add_argument(
        '--run', '-r',
        nargs=1,
        type=int,
        help='Run number',
        default=8,
        choices=[8],
        required=True
    )
    parser.add_argument(
        '--source', '-s',
        nargs=1,
        type=str,
        choices=function_set.storage_dictionary,
        help='Source storage to match and transfer missing raw files',
        required=True
    )
    parser.add_argument(
        '--destination', '-d',
        nargs=1,
        type=str,
        choices=function_set.storage_dictionary,
        help='Destination storage to obtain missing raw files',
        required=True
    )
    parser.add_argument(
        '--force', '-f',
        action='store_true',
        help='Rewrite existing destination file if exists but has another size or checksum'
    )
    parser.add_argument(
        '--delete', '-del',
        action='store_true',
        help='Remove destination file if there is no corresponding source file'
    )

    args = parser.parse_args()

    run_number, source_storage, destination_storage, file_force, file_delete = args.run, args.source, args.destination, args.force, args.delete

    if (source_storage[0] == destination_storage[0]):
        logging.error("ERROR: source (%s) and destination (%s) storages must be different" % (source_storage[0], destination_storage[0]))
        sys.exit(7)

    return run_number[0],source_storage[0],destination_storage[0],file_force,file_delete

# copy source file to the destination if it is absent on the destination storage
def transfer_file(source_file_path, source_storage_url,
                  destination_file_path, destination_storage_url, destination_run_storage_id,
                  source_file_size, source_file_checksum):
    logging.info('Copying source file "%s" to the destination path "%s"' % (source_file_path, destination_file_path))
    if function_set.cp_file_xrd(source_file_path, source_storage_url, destination_file_path, destination_storage_url) == False:
        return False
    logging.info('The source file has been copied')

    #os.makedirs(flag_dir, 0o755, True)
    #Path(flag_dir+file_name).import psycopg2touch()
    # connect to the database
    try:
        conn = psycopg2.connect(("dbname=%s user=%s host=%s password=%s") % (config["db_name"], config["db_user"], config["db_host"], config["db_pass"]))
    except Exception as e:
        logging.error("Database Connection Error: %s" % (e))
        return False
    cursor = conn.cursor()

    # if no XRootD access then do not check final size and checksum
    if (destination_storage_url is None):
        logging.debug('Inserting a destination file record with file path = "%s", no file size, no file_checksum' % (destination_file_path))
        cursor.execute("insert into file_catalogue(run_storage_id, file_path) "
                       "values (%d, '%s', %d, '%s')" % (destination_run_storage_id, destination_file_path))
        conn.commit()
        cursor.close()
        conn.close()
        logging.info("Adding information on the destination file has been successfull, but without the size and checksum")
        return True

    # check the size and checksum of the destination file if possible via XRootD
    destination_file_size = function_set.get_file_xrdsize(destination_file_path, destination_storage_url)
    if (destination_file_size != str(source_file_size)):
        logging.error("ERROR: Transferring file has been failed: destination (%s) and source (%d) file sizes are different" % (destination_file_size, source_file_size))
        cursor.close()
        conn.close()
        return False
    destination_file_checksum = function_set.get_file_xrdsum(destination_file_path, destination_storage_url)
    if (destination_file_checksum != source_file_checksum):
        logging.error("ERROR: Transferring file has been failed: destination (%s) and source (%s) file checksums are different" % (destination_file_checksum, source_file_checksum))
        cursor.close()
        conn.close()
        return False

    logging.debug('Inserting a destination file record with file path = "%s", file size = %d, file_checksum = %s' % (destination_file_path, source_file_size, destination_file_checksum))
    cursor.execute("insert into file_catalogue(run_storage_id, file_path, file_size, file_checksum) "
                   "values (%d, '%s', %d, '%s')" % (destination_run_storage_id, destination_file_path, source_file_size, destination_file_checksum))
    conn.commit()
    logging.info("Adding information on the destination file has been successfull")

    cursor.close()
    conn.close()
    return True


run_number,source_storage,destination_storage,file_force,file_delete = argument_parser()
logging.info("Transferring of missing raw files from '%s' to '%s' started" % (source_storage, destination_storage))

# load File Catalogue database configuration from JSON
config = json.load(open("file_catalogue.json"))

# connect to the File Catalogue database
try:
    conn = psycopg2.connect(("dbname=%s user=%s host=%s password=%s") % (config["db_name"], config["db_user"], config["db_host"], config["db_pass"]))
except Exception as e:
    logging.error("ERROR: Database connection failed: %s" % (e))
    exit(1)
cursor = conn.cursor()

# get source storage information from the File Catalogue
cursor.execute("select rs.run_storage_id, rs.storage_path, s.xrootd_url "
               "from run_storage rs join storage_ s on rs.storage_name = s.storage_name "
               "where rs.run_number=%d and rs.storage_name='%s'" % (run_number, source_storage))
conn.commit()
src_run_storage_id, source_storage_path, source_storage_url = cursor.fetchone()
if source_storage_path is None:
    logging.error("ERROR: No storage information (%s) found in the database for Run%s" % (source_storage, run_number))
    exit(2)
# get destination storage information from the File Catalogue
cursor.execute("select rs.run_storage_id, rs.storage_path, s.xrootd_url "
               "from run_storage rs join storage_ s on rs.storage_name = s.storage_name "
               "where rs.run_number=%d and rs.storage_name='%s'" % (run_number, destination_storage))
conn.commit()
dest_run_storage_id, destination_storage_path, destination_storage_url = cursor.fetchone()
if destination_storage_path is None:
    logging.error("ERROR: No storage information (%s) found in the database for Run%s" % (destination_storage_path, run_number))
    exit(3)

# get a list of the raw files on the source storage from the File Catalogue
cursor.execute("select file_path, file_size, file_checksum "
               "from file_catalogue fc join run_storage rs on fc.run_storage_id = rs.run_storage_id "
               "where run_number=%d and storage_name='%s'" % (run_number, source_storage))
conn.commit()
source_files = cursor.fetchall()

# get a list of the raw files on the destination storage from the File Catalogue
cursor.execute("select file_path, file_size, file_checksum "
               "from file_catalogue fc join run_storage rs on fc.run_storage_id = rs.run_storage_id "
               "where run_number=%d and storage_name='%s'" % (run_number, destination_storage))
conn.commit()
destination_files = cursor.fetchall()
destination_matches = []
destination_matches = [-1 for i in range(len(destination_files))]
cursor.close()
conn.close()

# create text file to write missing files as a list
file_list = open("transfer_raw_data.txt", "w")

# main loop to match source and destination raw files
iter_src = -1
for cur_src_file in source_files:
    iter_src+=1
    src_file_path, src_file_size, src_file_checksum = cur_src_file
    src_file_end = src_file_path.replace(source_storage_path,'')
    logging.debug('Current check for source file: "%s"' % (src_file_path))

    # searching for a File Catalogue record at the destination storage with the same end of the file path
    iter_dest = 0
    for cur_dest_file in destination_files:
        dest_file_path, dest_file_size, dest_file_checksum = cur_dest_file
        if dest_file_path.endswith(src_file_end):
            destination_matches[iter_dest] = iter_src
            break
        iter_dest+=1
    # if no corresponding file on the destination storage then transfer
    else:
        dest_file_path = destination_storage_path + src_file_end
        logging.info("The destination file is missing at '%s'" % (dest_file_path))
        print(dest_file_path, file=file_list)
        transfer_file(src_file_path, source_storage_url,
                      dest_file_path, destination_storage_url, dest_run_storage_id,
                      src_file_size, src_file_checksum)
        continue

    # if more than one raw file with the same name (critical error)
    #if len(destination_file) > 1:
    #    logging.error("CRITICAL ERROR: Several raw files with the same name: %s" % (src_file_end))
    #    continue

    dest_file_path, dest_file_size, dest_file_checksum = cur_dest_file
    # if there is a corresponding file found then check size and checksum
    if dest_file_size != src_file_size:
        if (dest_file_size is None):
            logging.error("ERROR: the destination file (%s) have no information on the size.\nPlease, run 'match_raw_data.py' script at the destination storage" % (dest_file_path))
            continue
        logging.error("ERROR: the source (%s) and destination (%s) files have different sizes: %d <> %d" % (src_file_path, dest_file_path, src_file_size, dest_file_size))
        if file_force:
            logging.info("The destination file at '%s' has an incorrect size and must be replaced (%d <> %d)" % (dest_file_path, dest_file_size, src_file_size))
            print(dest_file_path, file=file_list)
            #transfer_file(src_file_path, source_storage_url,
            #              dest_file_path, destination_storage_url, dest_run_storage_id,
            #              src_file_size, src_file_checksum)
        continue
    if dest_file_checksum != src_file_checksum:
        if (dest_file_checksum is None):
            logging.error("ERROR: the destination file (%s) have no information on the checksum.\nPlease, run 'match_raw_data.py' script at the destination storage" % (dest_file_path))
            continue
        logging.error("ERROR: the source (%s) and destination (%s) files have different checksum: %s <> %s" % (src_file_path, dest_file_path, src_file_checksum, dest_file_checksum))
        if file_force:
            logging.info("The destination file at '%s' has an incorrect checksum and must be replaced (%s <> %s)" % (dest_file_path, dest_file_checksum, src_file_checksum))
            print(dest_file_path, file=file_list)
            #transfer_file(src_file_path, source_storage_url,
            #              dest_file_path, destination_storage_url, dest_run_storage_id,
            #              src_file_size, src_file_checksum)
        continue

    logging.debug("The source and destination file are fully matched")

logging.info("Transferring of missing raw files from '%s' to '%s' finished" % (source_storage, destination_storage))

# searching for destination files that have no source pair
iter_dest = 0
print("", file=file_list)
for cur_dest_file in destination_files:
    dest_file_path, dest_file_size, dest_file_checksum = cur_dest_file
    if destination_matches[iter_dest] == -1:
        logging.warning("WARNING: The destination file at '%s' has no corresponding source file" % (dest_file_path))
        print(dest_file_path, file=file_list)
        if file_delete:
            #cursor.execute("delete from file_catalogue where run_storage_id = %d and file_path = '%s'" % (run_storage_id, catalogue_file_path))
            #conn.commit()
            logging.info("NOT IMPLEMENTED YET: The destination file with no corresponding source file has been deleted")
    iter_dest+=1

file_list.close()
