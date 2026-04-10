# This python script is developed to search for abnormaly small (and optionally large) summary sizes of raw files in runs
import datetime
import time
import os
import sys
import glob
import string
import json
import logging
import subprocess
import hashlib
import re
import argparse
import zlib # for adler32
import psycopg2
import operator

MIN_RUN_SIZE=500*1024*1024 # 1000 events * 500KB = app. 500MB
MAX_FILE_SIZE=50*1024*1024*1024 # 100000 events * 500KB = app. 50GB

filter_only_small = True
log_level = logging.INFO
#logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%Y-%m-%d %H:%M:%S', filename='sim_db.log', filemode='w', level=log_level)
logging.basicConfig(
    format='%(asctime)s %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    handlers=[
        #logging.FileHandler("sync_raw_data.log"),
        logging.StreamHandler(sys.stdout)
    ],
    level=log_level
)

def argument_parser():
    parser = argparse.ArgumentParser(
        description='Script for writing information on abnormaly small (optionally large) raw files to a text file\
        For more information run with --help option'
    )
    parser.add_argument(
        '--directory', '-dir',
        nargs=1,
        type=str,
        help='Output file path to write information on abnormaly small (optionally large) raw files',
        required=True
    )

    args = parser.parse_args()

    input_file_dir = args.directory

    return input_file_dir[0]

def size_human(num, suffix="B"):
    for unit in ["", "K", "M", "G", "T", "P", "E", "Z"]:
        if abs(num) < 1024.0:
            return f"{num:3.1f}{unit}{suffix}"
        num /= 1024.0
    return f"{num:.1f}Yi{suffix}"

input_file_dir = argument_parser()
logging.info("Searching for abnormaly small (optionally large) raw files in '%s' started" % (input_file_dir))

# get full file list in the input directory
#result = [os.path.join(dp, f) for dp, dn, filenames in os.walk(watch_dir) for f in filenames if os.path.splitext(f)[1] == '.data']
all_files = glob.glob(input_file_dir + '/**/*', recursive=True)
#print(list(all_files))
if not all_files:
    logging.error("ERROR: There are no files in the directory: %s" % (input_file_dir))
    exit(0)

# group by run numbers
run_numbers = set(map(lambda x:"0" if not re.findall(r'\d+', os.path.basename(x)) else re.findall(r'\d+', os.path.basename(x))[0], all_files))
run_numbers = sorted(run_numbers, key=int)
#print(run_numbers)
correct_files = [[y for y in all_files if x in os.path.basename(y)] for x in run_numbers if x != "0"]
wrong_files = [x for x in all_files if not re.findall(r'\d+', os.path.basename(x))]
#print(correct_files)
#print(wrong_files)

result_group_file = open("filter_raw_data.grp", 'w')
result_single_file = open("filter_raw_data.lst", 'w')
# cycle for all files in the input directory
for cur_file_group in correct_files:
    logging.debug("Current file group '%s'" % (cur_file_group))

    small_file = False
    # calculating sizes by groups
    sum_group_size = 0
    for cur_file in cur_file_group:
        logging.debug("Current file '%s'" % (cur_file))
        try:
            file_size = os.path.getsize(cur_file)
        except OSError as error:
            logging.error("ERROR: File is not accessible to get its size: %s" % (cur_file))
            continue;
        sum_group_size += file_size
        # if file is less than a given limit than set the flag
        if file_size < MIN_RUN_SIZE:
            small_file = True
        # if file is greater than a given limit than set the flag
        if not filter_only_small and file_size > MAX_FILE_SIZE:
            print("Too big file '%s' has %s size %s" % (os.path.basename(cur_file), size_human(file_size), os.linesep))
            print("{0} {1}".format(cur_file, size_human(file_size)), file=result_single_file)
            print("", file=result_single_file)

    logging.debug("Current file group size '%s'" % (size_human(sum_group_size)))
    # if group size is less than the limit
    if sum_group_size < MIN_RUN_SIZE:
        for cur_file in cur_file_group:
            try:
                file_size = os.path.getsize(cur_file)
            except OSError as error:
                continue;
            print("Too small file '%s' has %s size" % (os.path.basename(cur_file), size_human(file_size)))
            print("{0} {1}".format(cur_file, size_human(file_size)), file=result_group_file)
        print("Total run size is equal " + size_human(sum_group_size) + os.linesep)
        print("", file=result_group_file)
    # if group size is greater than the limit, but there are small files
    elif small_file:
        # check that the small files is not the last ones in the run (it is appropriate if it is the last one)
        part_numbers = set(map(lambda x:int(re.findall(r'\d+', os.path.basename(x))[-1]), cur_file_group))
        max_idx, max_part_number = max(enumerate(part_numbers), key=operator.itemgetter(1))
        is_not_last = False
        for cur_file in cur_file_group:
            try:
                file_size = os.path.getsize(cur_file)
            except OSError as error:
                continue;
            #print("For the current file ('%s'): current_part_num = %s, where max_part_num = %s" % (os.path.basename(cur_file), re.findall(r'\d+', os.path.basename(cur_file))[-1], format(max_val)))
            if file_size < MIN_RUN_SIZE:
                cur_part_number = re.findall(r'\d+', os.path.basename(cur_file))[-1]
                if  cur_part_number != str(max_part_number):
                    is_not_last = True
                    break
                # check big difference between files of the same part number (the last number in the file name)
                sub_cur_file_group = [x for x in cur_file_group if (re.findall(r'\d+', os.path.basename(x))[-1] == cur_part_number) and (x != cur_file)]
                for sub_cur_file in sub_cur_file_group:
                    try:
                        sub_file_size = os.path.getsize(sub_cur_file)
                    except OSError as error:
                        continue;
                    if abs(sub_file_size - file_size) > sub_file_size*0.15:
                        is_not_last = True
                    break

        # if only for the last part then all is right and continue
        if not is_not_last:
            continue

        # if there is small files, which are not corresponding an only last part of cutting
        for cur_file in cur_file_group:
            try:
                file_size = os.path.getsize(cur_file)
            except OSError as error:
                continue;
            if file_size < MIN_RUN_SIZE:
                print("Too small file '%s' has %s size" % (os.path.basename(cur_file), size_human(file_size)))
            else:
                print("Not small file '%s' (but in the group) has %s size" % (os.path.basename(cur_file), size_human(file_size)))
            print("{0} {1}".format(cur_file, size_human(file_size)), file=result_single_file)
        print("The run with the small file sizes has been finished" + os.linesep)
        print("", file=result_single_file)

# show file list with incorrect run numbers in their names
if wrong_files:
    print("There are files with incorrect run numbers in their names")
for cur_file in wrong_files:
    try:
        file_size = os.path.getsize(cur_file)
    except OSError as error:
        logging.error("ERROR: File is not accessible to get its size: %s" % (cur_file))
        continue;
    print("Incorrect file name of the following file '%s' with %s size" % (cur_file, size_human(file_size)))

result_group_file.close()
result_single_file.close()
logging.info("Searching for abnormaly small (optionally large) raw files in '%s' finished" % (input_file_dir))
