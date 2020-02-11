#ifndef SKTV_DEFS_H
#define SKTV_DEFS_H

#include <QMap>


/** ******  ТИП ДАННЫХ 0x33  ********* **/

// датчики температуры
#define NUM_FI150_DV1T  1
#define NUM_FI150_DV2T  2
#define NUM_FI150_DV3T  3
#define NUM_FI150_DV4T  4
#define NUM_FI150_DV5T  5
#define NUM_FI150_DV6T  6
#define NUM_FI150_DV7T  7
#define NUM_FI150_DV8T  8
#define NUM_FI150_DV9T  9
#define NUM_FI150_DV10T 10
#define NUM_FI150_DV11T 11
#define NUM_FI150_DV12T 12

// датчики влажности
#define NUM_FI150_DV1F  1
#define NUM_FI150_DV2F  2
#define NUM_FI150_DV3F  3
#define NUM_FI150_DV4F  4
#define NUM_FI150_DV5F  5
#define NUM_FI150_DV6F  6
#define NUM_FI150_DV7F  7
#define NUM_FI150_DV8F  8
#define NUM_FI150_DV9F  9
#define NUM_FI150_DV10F 10
#define NUM_FI150_DV11F 11
#define NUM_FI150_DV12F 12

typedef QMap<quint8, QString> T_sensors_by_number;
typedef QMap<quint8, QString> H_sensors_by_number;

T_sensors_by_number TS = {{NUM_FI150_DV1T,  "FI150_DV1T" }, {NUM_FI150_DV2T,  "FI150_DV2T" }, {NUM_FI150_DV3T,  "FI150_DV3T" },
                          {NUM_FI150_DV4T,  "FI150_DV4T" }, {NUM_FI150_DV5T,  "FI150_DV5T" }, {NUM_FI150_DV6T,  "FI150_DV6T" },
                          {NUM_FI150_DV7T,  "FI150_DV7T" }, {NUM_FI150_DV8T,  "FI150_DV8T" }, {NUM_FI150_DV9T,  "FI150_DV9T" },
                          {NUM_FI150_DV10T, "FI150_DV10T"}, {NUM_FI150_DV11T, "FI150_DV11T"}, {NUM_FI150_DV12T, "FI150_DV12T"}};


H_sensors_by_number HS = {{NUM_FI150_DV1F,  "FI150_DV1F" }, {NUM_FI150_DV2F,  "FI150_DV2F" }, {NUM_FI150_DV3F,  "FI150_DV3F" },
                          {NUM_FI150_DV4F,  "FI150_DV4F" }, {NUM_FI150_DV5F,  "FI150_DV5F" }, {NUM_FI150_DV6F,  "FI150_DV6F" },
                          {NUM_FI150_DV7F,  "FI150_DV7F" }, {NUM_FI150_DV8F,  "FI150_DV8F" }, {NUM_FI150_DV9F,  "FI150_DV9F" },
                          {NUM_FI150_DV10F, "FI150_DV10F"}, {NUM_FI150_DV11F, "FI150_DV11F"}, {NUM_FI150_DV12F, "FI150_DV12F"}};


#endif // SKTV_DEFS_H







