#!/usr/bin/python3

import datetime
import psycopg2
import hashlib
import json
import re
import os


with open("inspector.json","r") as fp: #full PATH  to file
    set = json.load(fp)
    storage = set["storage"]
    if(set["check_type"] == 1):
        check_type=1
        table = "simulation_file"
    else:
        check_type=2
        table = "run_"

found_errors=0
mail_adress="*"

def connection_bmn_database():
    conn_bmn = psycopg2.connect(dbname="bmn_db", user="*", password="*", host="*", port="5432")
    cursor = conn_bmn.cursor()
    cursor.execute("SELECT file_path, file_md5 FROM "+ table.format())
    records = cursor.fetchall()
    cursor.close()
    conn_bmn.close()
    return records, cursor.rowcount

def integrity_check(conn_integrity, check_date):
    file_count = connection_bmn_database()[1]
    cursor = conn_integrity.cursor()
    cursor.execute("INSERT INTO integrity_check(type_id, storage_id, check_date, file_count) VALUES (%s, %s, %s, %s) RETURNING check_id", [check_type, storage, check_date, file_count])
    check_id = cursor.fetchone()[0]
    conn_integrity.commit()
    cursor.close()
    return check_id


def failed_record(conn_integrity, check_id, error_id, file_path, error_details):
    cursor = conn_integrity.cursor()
    cursor.execute("INSERT INTO failed_record(check_id, error_id, file_path, error_details) VALUES (%s, %s, %s, %s)", [check_id, error_id, file_path, error_details])
    conn_integrity.commit()
    cursor.close()

def inspection(count, conn_integrity, check_id, records):
    global found_errors
    for i in range(count):
        path_to_file=str(records[i][0]).strip()
        if (path_to_file.__contains__('/') != True):
            print(path_to_file)
            continue
        if (path_to_file.strip()[-1] == '/'):
            print(path_to_file)
            continue
        if(str(records[i][1]).strip() == "None"):
            found_errors=found_errors+1
            failed_record(conn_integrity, check_id, 3, path_to_file,'')
        else:
            hash_md5 = hashlib.md5()
            try:
                with open(path_to_file, 'rb') as fp:
                    for chunk in iter(lambda: fp.read(8192), b""):
                        hash_md5.update(chunk)
            except FileNotFoundError as e:
                found_errors=found_errors+1
                failed_record(conn_integrity, check_id,1,path_to_file,'')
            except BaseException as e:
                found_errors=found_errors+1
                e = re.sub(": '"+path_to_file+"'", "", str(e))
                failed_record(conn_integrity, check_id,2,path_to_file,str(e))
            else:
                if (str(records[i][1]).strip() != hash_md5.hexdigest()):
                    found_errors=found_errors+1
                    failed_record(conn_integrity, check_id,4,path_to_file,'')
# --- START ---
def connection_integrity_database():
    conn_integrity = psycopg2.connect(dbname="*", user="*", password="*", host="*", port="5432")
    check_date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    check_id = integrity_check(conn_integrity, check_date)
    records, count = connection_bmn_database()
    inspection(count, conn_integrity, check_id, records)
    finish_date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    cursor = conn_integrity.cursor()
    cursor.execute("UPDATE integrity_check SET complete_date=%s WHERE check_id=%s", [finish_date, check_id])
    conn_integrity.commit()
    cursor.close()
    conn_integrity.close()

    sendmail_location = "/usr/sbin/sendmail"
    p = os.popen("%s -t" % sendmail_location, "w")
    p.write("From: %s\n" % "NCX Inspector < * >")
    p.write("To: %s\n" % mail_adress)
    p.write("Subject: NCX Inspector Robot\n")
    p.write("\n")
    p.write("Inspection of the * files has been completed.")
    p.write("\n")
    p.write("Operating time: " + check_date + " - " + finish_date)
    p.write("\n")
    p.write("Number of errors: " + str(found_errors))
    p.close()

connection_integrity_database()
