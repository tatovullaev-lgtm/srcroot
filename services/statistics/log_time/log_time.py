import argparse
import datetime
import matplotlib.pyplot as plt
import numpy as np
import os
import psycopg2
import re

import log_time.config as config

from exceptions import NoDataException


class TimeStatComputer:
    def __init__(self, config_dict):

        self.EXTENSIONS = config_dict.get('extensions')
        self.SUCCESS = int(config_dict.get('success', "1"))
        self.FAILURE = config_dict.get('failure', "")
        self.DB_USER = config_dict.get('db_user')
        self.DB_PASS = config_dict.get('db_pass')
        self.DB_NAME = config_dict.get('db_name')
        self.DB_HOST = config_dict.get('db_host')

        self.DPI = config_dict.get('dpi', config.DPI)
        self.FOLDERS_IGNORE = config.FOLDERS_IGNORE
        self.FOLDERS_IGNORE.extend(config_dict.get('folders_ignore', config.FOLDERS_IGNORE))


    # ### Old implementation ###
    # def get_event_count(self, run_num):
    #     conn = psycopg2.connect(dbname=self.DB_NAME, user=self.DB_USER, 
    #                             password=self.DB_PASS, host=self.DB_HOST)
    #     cursor = conn.cursor()
    #     cursor.execute(f'SELECT event_count FROM run_ WHERE run_number = {run_num}')
    #     count = cursor.fetchone()
    #     if count is None:
    #         return None
    #     count = count[0]
    #     cursor.close()
    #     conn.close()
    #     return count

    def get_event_count(self, run_num):
        """ Returns events_count from DB (or None) and a warning flag (if there where multiple runs with same num) """
        conn = psycopg2.connect(dbname=self.DB_NAME, user=self.DB_USER, 
                                password=self.DB_PASS, host=self.DB_HOST)
        cursor = conn.cursor()
        cursor.execute(f"SELECT period_number, event_count FROM run_ WHERE run_number = {run_num}")
        resp = cursor.fetchall()
        cursor.close()
        conn.close()
        # print(resp)
        if resp is None or len(resp) == 0:
            return (None, False) 
        if len(resp) == 1:
            count = resp[0][1]
            return (count, False)
        else:
            # Look for maximum period number with given run_num
            count = max(resp, key = lambda tup: tup[0])[1]
            return (count, True)
        

    def is_file_to_parse(self, root, file):
        # filepath = os.path.join(root, file)
        correct_ext = any([file.endswith(ext) for ext in self.EXTENSIONS]) or ("*" in self.EXTENSIONS)
        correct_folder = all([elem not in os.path.join(root, file) for elem in self.FOLDERS_IGNORE])
        return correct_ext and correct_folder


    def parse_dir(self, _dir, recursive):
        time_arr = []
        time_per_events_arr = []
        unsuccessful_arr = []
        unsuccessful_arr_files = []
        successful_arr_files = []
        if recursive:
            files_to_walk = os.walk(_dir)
        else:
            files_to_walk = [next(os.walk(_dir))]
        
        files_parsed = 0
        for root, dirs, files in files_to_walk:
            for file in files:
                if self.is_file_to_parse(root, file):
                    try:
                        time, is_successful, failure_marker, run_number, log_event_number, file_processing = self.parse_log(os.path.join(root, file))
                        #print(''.join(filter(None, ('time = ',str(time),'. is_successful=',str(is_successful),'. run_number=',str(run_number),'. log_event_number=',str(log_event_number),'. file_processing=',str(file_processing)))))
                    except PermissionError:
                        print(f"Permission denied - skipping file {os.path.join(root, file)}")
                        continue
                    except Exception as ex:
                        print(f"Error while processing: {ex} - skipping file {os.path.join(root, file)}")
                        continue
                    files_parsed += 1
                    print("+", end="", flush=True)

                    if failure_marker:
                        print(f"\n Failure marker was found for file - skipping {os.path.join(root, file)}")
                        time = None
                        is_successful = -1
                    if time is not None:
                        if is_successful >= self.SUCCESS:
                            time_arr.append(time)
                            event_count = None
                            warn_flag = False
                            if run_number is not None:
                                event_count, warn_flag = self.get_event_count(run_number) 
                                if event_count is not None and event_count == 0:
                                    event_count = None
                            if event_count is None and (log_event_number is not None and log_event_number != 0):
                                event_count = log_event_number
                            if event_count is not None:
                                if warn_flag:
                                    print(f"Warning: more than one run records: the latest period number is selected for {os.path.join(root, file)}")
                                time_per_events_arr.append(time / event_count)
                            else:
                                print(f"\nCan't get event count from the database for file {os.path.join(root, file)}")
                        else:
                            print(f"\n The log shows that the task was not ended successfully - skipping {os.path.join(root, file)}")
                    elif is_successful >= self.SUCCESS:
                        print(f"\nCan not parse time in successfully ended log file {os.path.join(root, file)}")
                        
                    if is_successful < self.SUCCESS:
                        unsuccessful_arr.append(os.path.join(root, file))
                        if file_processing != None:
                            unsuccessful_arr_files.append(file_processing)
                        else:
                            print(f"\nUnsuccessfully processed file name was not defined in {os.path.join(root, file)}.")
                    else:
                        if file_processing != None:
                            successful_arr_files.append(file_processing)
                        else:
                            print(f"\nSuccessfully processed file name was not defined in {os.path.join(root, file)}.")
                        
        if time_arr == []:
            raise NoDataException

        print(f"\n\nTotal files parsed: {files_parsed}")
        return np.array(time_arr), np.array(time_per_events_arr), unsuccessful_arr, unsuccessful_arr_files, successful_arr_files, files_parsed


    def compute(self, _dir, recursive):
        arr, arr_per_event, unsuccessful_arr, unsuccessful_arr_files, successful_arr_files, files_parsed = self.parse_dir(_dir, recursive)
        if len(arr) == 0:
            unit = config.UNITS[0]
            title = f"Array of time values is empty."
        else:
            arr, unit = self.convert_units(arr)
            title = f"Time, {unit}. Mean = {np.mean(arr):.3f} {unit}, overall {len(arr)} files."

        if len(arr_per_event) == 0:
            unit_per_event = config.UNITS[0]
            title_per_event = f"Array of time values per event is empty."
        else:
            arr_per_event, unit_per_event = self.convert_units(arr_per_event)
            title_per_event = f"Time per event, {unit_per_event}. Mean = {np.mean(arr_per_event):.3f} {unit_per_event}, " \
                f"overall {len(arr_per_event)} files."

        print()
        if len(unsuccessful_arr) == 0:
            print("All runs ended successfully.\n")
        else:
            print("Unsuccessfully ended runs:")
            for elem in unsuccessful_arr:
                print(elem)
            if config.UNSUCCESSFUL_LOG_FILE is not None:
                print()
                with open(config.UNSUCCESSFUL_LOG_FILE, "wt") as f:
                    for elem in sorted(unsuccessful_arr_files):
                        f.write(elem + "\n")
                print(f"Unsuccessfully processed files list ({len(unsuccessful_arr_files)}/{files_parsed}, {(100*len(unsuccessful_arr_files)/files_parsed):.1f}%)"\
                    f" was saved to {config.UNSUCCESSFUL_LOG_FILE}")
        if config.SUCCESSFUL_LOG_FILE is not None:
            with open(config.SUCCESSFUL_LOG_FILE, "wt") as f:
                for elem in sorted(successful_arr_files):
                    f.write(elem + "\n")
            print(f"Successfully processed files list ({len(successful_arr_files)}/{files_parsed}, {(100*len(successful_arr_files)/files_parsed):.1f}%)"\
                    f" was saved to {config.SUCCESSFUL_LOG_FILE}")

        print()
        print("Obtained characteristics:")
        if arr.size == 0:
            print("  Array of time values is empty.")
        else:
            print(f"  Mean time = {np.mean(arr):.3f} {unit}.")
            print(f"  Summary time = {np.sum(arr):.3f} {unit}.")
        if arr_per_event.size == 0:
            print("  Array of time values per event is empty.")
        else:
            print(f"  Mean time per event = {np.mean(arr_per_event):.3f} {unit_per_event}.")

        return (arr, unit, title, arr_per_event, unit_per_event, title_per_event)


    """
        Defines units for the given array closest to the mean value
        and converts the array to the units.

        Parameters
        ----------
        arr : numpy.ndarray
            Array to be converted to the mean units.

        Returns
        -------
        arr : numpy.ndarray
            array converted to the mean units.
        unit : str
            Unit of the converted array.
    """
    def convert_units(self, arr):
        if arr.size == 0:
            return arr, config.UNITS[0]
        mean = np.mean(arr)
        u = len(config.UNITS) - 1
        for i, unit in enumerate(config.UNITS):
            mean = mean / config.SIZE[i]
            if mean < config.SIZE[i+1]:
                u = i
                break
        for i in range (u + 1):
            arr = arr / config.SIZE[i]
        return arr, config.UNITS[u]


    def convert_month(self, month):
        res = config.MONTH_ARR[month]
        return res


    """
        Parse date from regex match result and return datetime object.

        Parameters
        ----------
        result : regex match object
            Contains year, month, day, time and time12 groups.

        Returns
        -------
        datetime object
    """
    def get_date(self, result):
        start = f'{result.group("year")}-{self.convert_month(result.group("month"))}-{result.group("day")}'
        time = result.group('time')
        if result.group('time12'):
            start = f'{start} {time} {result.group("time12")}'
            print(start)
            return datetime.datetime.strptime(start, "%Y-%m-%d %I:%M:%S %p")
        else:
            start = f'{start} {time}'
            return datetime.datetime.strptime(start, "%Y-%m-%d %H:%M:%S")


    def parse_log(self, log_file):
        is_successful = 0
        failure_marker = False
        start = None
        end = None
        run_number = None
        event_number = None
        file_processing = None
        with open(log_file, 'r') as f:
            for line in f:
                if config.START in line:
                    result = re.search(config.START_REGEX, line)
                    if result != None:
                        start = self.get_date(result)
                    else:
                        print(f"Error while parsing start datetime in log file: {log_file}")
                if config.END in line:
                    result = re.search(config.END_REGEX, line)
                    if result != None:
                        end = self.get_date(result)
                    else:
                        print(f"Error while parsing end datetime in log file: {log_file}")
                if config.SUCCESS in line:
                    is_successful += 1
                if self.FAILURE != "" and self.FAILURE in line:
                    failure_marker = True
                if config.RUN_EXTENSION in line and run_number == None:
                    result = re.search(config.RUN_REGEX, line)
                    if result != None:
                        run_number = int(result.group(1))
                if event_number == None:
                    result = re.search(config.EVENT_REGEX, line)
                    if result != None:
                        event_number = int(result.group(1))
                if file_processing == None:
                    if line.startswith(config.PROCESSING_LINE_START):
                        result = re.search(config.PROCESSING_REGEX, line)
                        if result != None:
                            if len(result.groups()) > 0:
                                file_processing = result.groups()[0]
                            # print("file_processing = ", file_processing)
        if start == None or end == None:
            delta = None
        else:
            delta = (end - start).total_seconds()
        # TODO add verbose output option
        #print(f"Log file {log_file}, delta {delta}, is_successful {is_successful}, run_num {run_num}")

        return delta, is_successful, failure_marker, run_number, event_number, file_processing
