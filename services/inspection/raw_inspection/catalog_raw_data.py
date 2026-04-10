# This python script is developed for matching raw file metadata with the File Catalogue database for storages: DDC/DAQ, NCX or CICC/MICC
import os
import glob
import json
import logging
import argparse
import psycopg2
import function_set

logging.getLogger().setLevel(logging.INFO)

def argument_parser():
    parser = argparse.ArgumentParser(
        description='Script for matching raw file metadata with the File Catalogue\
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
        '--storage', '-s',
        nargs=1,
        type=str,
        choices=['ddc','ncx','cicc'],
        help='Name of the storage to write raw file metadata to the File Catalogue',
        required=True
    )

    parser.add_argument(
        '--delete', '-del',
        action='store_true',
        help='Remove file metadata in the File Catalogue if there is no corresponding file on the storage'
    )

    args = parser.parse_args()

    run_number, storage_name, force_delete = args.run, args.storage, args.delete

    return run_number[0],storage_name[0],force_delete


run_number, storage_name, force_delete = argument_parser()
logging.info("Matching raw file metadata with the File Catalogue started for '%s' storage" % (storage_name))

# load database configuration from JSON
config = json.load(open("file_catalogue.json"))

# connect to the file database
try:
    conn = psycopg2.connect(("dbname=%s user=%s host=%s password=%s") % (config["db_name"], config["db_user"], config["db_host"], config["db_pass"]))
except Exception as e:
    logging.error("ERROR: Database connection failed: %s" % (e))
    exit(1)
cursor = conn.cursor()

# get storage information
cursor.execute("select rs.run_storage_id, rs.storage_path, s.xrootd_url "
               "from run_storage rs join storage_ s on rs.storage_name = s.storage_name "
               "where rs.run_number=%d and rs.storage_name='%s'" % (run_number, storage_name))
conn.commit()
run_storage_id, storage_path, xrootd_url = cursor.fetchone()
if storage_path is None:
    logging.error("ERROR: No storage information (%s) found in the database for Run%s" % (storage_name, run_number))
    exit(2)

# get a list of the raw files in the File Catalogue
cursor.execute("select fc.file_guid, fc.file_path, fc.file_size, fc.file_checksum, 0 "
               "from file_catalogue fc join run_storage rs on fc.run_storage_id = rs.run_storage_id "
               "where rs.run_storage_id=%d" % (run_storage_id))
conn.commit()
catalogue_files = cursor.fetchall()
catalogue_matches = []
catalogue_matches = [0 for i in range(len(catalogue_files))]

# get full file list in the storage directory
#result = [os.path.join(dp, f) for dp, dn, filenames in os.walk(watch_dir) for f in filenames if os.path.splitext(f)[1] == '.data']
all_files = glob.glob(storage_path + '/**/*', recursive=True)
#print(list(all_files))

#cursor.execute("select file_path from operation_info where file_path = ANY(%s);", [[10, 20]])
#cursor.execute(cursor.mogrify("select file_path from operation_info where file_path in %s", [tuple([10, 20])]))
# cycle for all files in the input directory
for cur_file in all_files:
    logging.debug("Current file '%s'" % (cur_file))

    # searching for a file in the File Catalogue with the same path
    i = 0
    for file_record in catalogue_files:
        catalogue_file_guid, catalogue_file_path, catalogue_file_size, catalogue_file_checksum, catalogue_file_matched = file_record
        if catalogue_file_path == cur_file:
            catalogue_matches[i] = 1
            break
        i+=1
    # if no corresponding file in the File Catalogue
    else:
        file_record = None

    if file_record is not None:
        logging.debug('The current file exists')
        catalogue_file_guid, catalogue_file_path, catalogue_file_size, catalogue_file_checksum, catalogue_file_matched = file_record

        if catalogue_file_size is None:
            try:
                file_size = os.path.getsize(cur_file)
                cursor.execute("update file_catalogue set file_size=%d where file_guid=%d" % (file_size, catalogue_file_guid))
                conn.commit()
                logging.debug('The size of the file has been updated in the File Catalogue')
            except OSError as error:
                logging.error("ERROR: File is not accessible to get its size: %s" % (cur_file))

        if catalogue_file_checksum is None:
            file_checksum = ''
            if xrootd_url is None:
                file_checksum = function_set.get_file_adler32c(cur_file)
            else:
                file_checksum = function_set.get_file_xrdsum(cur_file, xrootd_url)
            if (file_checksum != ''):
                cursor.execute("update file_catalogue set file_checksum=%s where file_guid=%d" % (file_checksum, catalogue_file_guid))
                conn.commit()
                logging.debug('The checksum of the file has been updated in the File Catalogue')
    else:
        logging.info("Adding file '%s' to the File Catalogue" % (cur_file))
        try:
            file_size = os.path.getsize(cur_file)
        except OSError as error:
            logging.error("ERROR: File is not accessible to get its size: %s" % (cur_file))
            continue;
        file_checksum = ''
        if xrootd_url is None:
            file_checksum = function_set.get_file_adler32c(cur_file)
        else:
            file_checksum = function_set.get_file_xrdsum(cur_file, xrootd_url)
        if (file_checksum == ''):
            continue;

        logging.debug('Inserting a new file record with file path = "%s", file size = %d, file_checksum = %s' % (cur_file, file_size, file_checksum))
        cursor.execute("insert into file_catalogue(run_storage_id, file_path, file_size, file_checksum) "
                       "values (%d, '%s', %d, '%s')" % (run_storage_id, cur_file, file_size, file_checksum))
        conn.commit()
        logging.info("Information on the current file has been successfullly added\n")

# searching for records in the File Catalogue that have no pair on the file storage
i = 0
for file_record in catalogue_files:
    catalogue_file_guid, catalogue_file_path, catalogue_file_size, catalogue_file_checksum, catalogue_file_matched = file_record
    if catalogue_matches[i] == 0:
        logging.warning("WARNING: The record in the File Catalogue ('%s') has no corresponding file" % (catalogue_file_path))
        if force_delete:
            cursor.execute("delete from file_catalogue where run_storage_id = %d and file_path = '%s'" % (run_storage_id, catalogue_file_path))
            conn.commit()
            logging.info("The record on the absent file in the File Catalogue has been deleted")
    i+=1

cursor.close()
conn.close()
logging.info("Matching raw file metadata with the File Catalogue finished for '%s' storage" % (storage_name))
