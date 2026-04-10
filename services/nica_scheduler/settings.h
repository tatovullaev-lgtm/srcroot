/* IMPORTANT:
 * Recompile the software after the changing any variable below
 */

#ifndef SETTINGS_H
#define SETTINGS_H

/* Experiment Database settings */
const char* const UNI_DB_HOST = "nc13.jinr.ru";
const char* const UNI_DB_NAME = "bmn_db";
const char* const UNI_DB_USERNAME = "db_reader";
const char* const UNI_DB_PASSWORD = "reader_pass";


/* Predefined Database Settings for experiments
format: { "name", {"db_host", "db_name", "db_username", "db_password"} } */
std::map<string, struct_database_settings> predefined_database_settings =
{
    {"bmn", {"nc13.jinr.ru", "bmn_db", "db_reader", "reader_pass"}}
};

#endif // SETTINGS_H
