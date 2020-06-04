﻿#ifndef GEN_DEFS_H
#define GEN_DEFS_H

#include "../ksuts_devices/oht/sv_oht.h"
#include "../ksuts_devices/opa/sv_opa.h"
#include "../ksuts_devices/skm/sv_skm.h"
#include "../ksuts_devices/sktv/sv_ktv.h"

#define ACTUAL_DB_VERSION "1.1.0"

#define OPTION_DB_HOST                   "db_host"
#define OPTION_DB_PORT                   "db_port"
#define OPTION_DB_NAME                   "db_name"
#define OPTION_DB_USER                   "db_user"
#define OPTION_DB_PASS                   "db_pass"
#define OPTION_ECHO                      "echo"
#define OPTION_DETILED                   "detiled"
#define OPTION_SOEG_PORT                 "soeg_port"
#define OPTION_SINGLE_DEVICE_MODE        "single_device_mode"
#define OPTION_SINGLE_DEVICE_INDEX       "single_device_index"
#define OPTION_LOGGING                   "logging"
#define OPTION_LOG_LEVEL                 "log_level"
#define OPTION_LOG_DEVICE                "log_device"
#define OPTION_LOG_DIRECTORY             "log_directory"
#define OPTION_LOG_FILENAME              "log_filename"
#define OPTION_LOG_TRUNCATE_ON_ROTATION  "log_truncate_on_rotation"
#define OPTION_LOG_ROTATION_AGE          "log_rotation_age"
#define OPTION_LOG_ROTATION_SIZE         "log_rotation_size"
#define OPTION_AUTORUN_CFG_FILE          "autorun_cfg_file"
#define OPTION_TEMPLATES_DIRECTORY       "templates_directory"
#define OPTION_PATH_TO_POSTGRES_BIN      "postgres_bin_path"

struct AppConfig {
    QString db_name;
    QString db_host;
    quint16 db_port;
    QString db_user;
    QString db_pass;
    quint16 soeg_port;
    bool single_device_mode;
    int single_device_index;
    sv::log::Options log_options;
    QString autorun_cfg_file;
    QString templates_dir;
    QString postgres_bin_path;
};

#endif // GEN_DEFS_H