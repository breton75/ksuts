#ifndef SKM_DEFS_H
#define SKM_DEFS_H

#include <QMap>


/** ******  ТИП ДАННЫХ 0x01  ********* **/
#define FACTOR_1   0x01U
#define FACTOR_2   0x02U
#define FACTOR_3   0x03U
#define FACTOR_4   0x04U
#define FACTOR_5   0x05U
#define FACTOR_6   0x06U

#define VIN_TK1    2U
#define VIN_TK2    3U
#define VIN_TK3    5U
#define VIN_TK4    6U
#define VIN_TK5    7U
#define VIN_TK6    8U
#define VIN_TK7    31U
#define VIN_TK8    32U
#define VIN_TK9    9U
#define VIN_TK10   10U
#define VIN_TK11   11U
#define VIN_TK12   12U
#define VIN_TK13   14U
#define VIN_TK14   15U
#define VIN_TK15   13U
#define VIN_TK16   19U
#define VIN_TK17   17U
#define VIN_TK18   18U
#define VIN_TK19   1U
#define VIN_TK20   22U
#define VIN_TK21   23U
#define VIN_TK22   30U
#define VIN_TK23   29U
#define VIN_TK24   21U
#define VIN_TK25   25U
#define VIN_TK26   26U
#define VIN_TK27   27U
#define VIN_TK28   28U
#define VIN_TK29   24U
#define VIN_TK30   35U
#define VIN_TK31   33U
#define VIN_TK32   34U
#define VIN_TK33   36U

typedef QMap<quint8, QString> signal_by_factor;

signal_by_factor TK1  = {{FACTOR_1, "BI39_SS2_TK1_1"},  {FACTOR_2, "BI39_SS2_TK1_2"},  {FACTOR_3, "BI39_SS2_TK1_3"},  {FACTOR_4, "BI39_SS2_TK1_4"},  {FACTOR_5, "BI39_SS2_TK1_5"},  {FACTOR_6, "BI39_SS2_TK1_6"}};
signal_by_factor TK2  = {{FACTOR_1, "BI39_SS2_TK2_1"},  {FACTOR_2, "BI39_SS2_TK2_2"},  {FACTOR_3, "BI39_SS2_TK2_3"},  {FACTOR_4, "BI39_SS2_TK2_4"},  {FACTOR_5, "BI39_SS2_TK2_5"},  {FACTOR_6, "BI39_SS2_TK2_6"}};
signal_by_factor TK3  = {{FACTOR_1, "BI39_SS2_TK3_1"},  {FACTOR_2, "BI39_SS2_TK3_2"},  {FACTOR_3, "BI39_SS2_TK3_3"},  {FACTOR_4, "BI39_SS2_TK3_4"},  {FACTOR_5, "BI39_SS2_TK3_5"},  {FACTOR_6, "BI39_SS2_TK3_6"}};
signal_by_factor TK4  = {{FACTOR_1, "BI39_SS2_TK4_1"},  {FACTOR_2, "BI39_SS2_TK4_2"},  {FACTOR_3, "BI39_SS2_TK4_3"},  {FACTOR_4, "BI39_SS2_TK4_4"},  {FACTOR_5, "BI39_SS2_TK4_5"},  {FACTOR_6, "BI39_SS2_TK4_6"}};
signal_by_factor TK5  = {{FACTOR_1, "BI39_SS2_TK5_1"},  {FACTOR_2, "BI39_SS2_TK5_2"},  {FACTOR_3, "BI39_SS2_TK5_3"},  {FACTOR_4, "BI39_SS2_TK5_4"},  {FACTOR_5, "BI39_SS2_TK5_5"},  {FACTOR_6, "BI39_SS2_TK5_6"}};
signal_by_factor TK6  = {{FACTOR_1, "BI39_SS2_TK6_1"},  {FACTOR_2, "BI39_SS2_TK6_2"},  {FACTOR_3, "BI39_SS2_TK6_3"},  {FACTOR_4, "BI39_SS2_TK6_4"},  {FACTOR_5, "BI39_SS2_TK6_5"},  {FACTOR_6, "BI39_SS2_TK6_6"}};
signal_by_factor TK7  = {{FACTOR_1, "BI39_SS2_TK7_1"},  {FACTOR_2, "BI39_SS2_TK7_2"},  {FACTOR_3, "BI39_SS2_TK7_3"},  {FACTOR_4, "BI39_SS2_TK7_4"},  {FACTOR_5, "BI39_SS2_TK7_5"},  {FACTOR_6, "BI39_SS2_TK7_6"}};
signal_by_factor TK8  = {{FACTOR_1, "BI39_SS2_TK8_1"},  {FACTOR_2, "BI39_SS2_TK8_2"},  {FACTOR_3, "BI39_SS2_TK8_3"},  {FACTOR_4, "BI39_SS2_TK8_4"},  {FACTOR_5, "BI39_SS2_TK8_5"},  {FACTOR_6, "BI39_SS2_TK8_6"}};
signal_by_factor TK9  = {{FACTOR_1, "BI39_SS2_TK9_1"},  {FACTOR_2, "BI39_SS2_TK9_2"},  {FACTOR_3, "BI39_SS2_TK9_3"},  {FACTOR_4, "BI39_SS2_TK9_4"},  {FACTOR_5, "BI39_SS2_TK9_5"},  {FACTOR_6, "BI39_SS2_TK9_6"}};
signal_by_factor TK10 = {{FACTOR_1, "BI39_SS2_TK10_1"}, {FACTOR_2, "BI39_SS2_TK10_2"}, {FACTOR_3, "BI39_SS2_TK10_3"}, {FACTOR_4, "BI39_SS2_TK10_4"}, {FACTOR_5, "BI39_SS2_TK10_5"}, {FACTOR_6, "BI39_SS2_TK10_6"}};
signal_by_factor TK11 = {{FACTOR_1, "BI39_SS2_TK11_1"}, {FACTOR_2, "BI39_SS2_TK11_2"}, {FACTOR_3, "BI39_SS2_TK11_3"}, {FACTOR_4, "BI39_SS2_TK11_4"}, {FACTOR_5, "BI39_SS2_TK11_5"}, {FACTOR_6, "BI39_SS2_TK11_6"}};
signal_by_factor TK12 = {{FACTOR_1, "BI39_SS2_TK12_1"}, {FACTOR_2, "BI39_SS2_TK12_2"}, {FACTOR_3, "BI39_SS2_TK12_3"}, {FACTOR_4, "BI39_SS2_TK12_4"}, {FACTOR_5, "BI39_SS2_TK12_5"}, {FACTOR_6, "BI39_SS2_TK12_6"}};
signal_by_factor TK13 = {{FACTOR_1, "BI39_SS2_TK13_1"}, {FACTOR_2, "BI39_SS2_TK13_2"}, {FACTOR_3, "BI39_SS2_TK13_3"}, {FACTOR_4, "BI39_SS2_TK13_4"}, {FACTOR_5, "BI39_SS2_TK13_5"}, {FACTOR_6, "BI39_SS2_TK13_6"}};
signal_by_factor TK14 = {{FACTOR_1, "BI39_SS2_TK14_1"}, {FACTOR_2, "BI39_SS2_TK14_2"}, {FACTOR_3, "BI39_SS2_TK14_3"}, {FACTOR_4, "BI39_SS2_TK14_4"}, {FACTOR_5, "BI39_SS2_TK14_5"}, {FACTOR_6, "BI39_SS2_TK14_6"}};
signal_by_factor TK15 = {{FACTOR_1, "BI39_SS2_TK15_1"}, {FACTOR_2, "BI39_SS2_TK15_2"}, {FACTOR_3, "BI39_SS2_TK15_3"}, {FACTOR_4, "BI39_SS2_TK15_4"}, {FACTOR_5, "BI39_SS2_TK15_5"}, {FACTOR_6, "BI39_SS2_TK15_6"}};
signal_by_factor TK16 = {{FACTOR_1, "BI39_SS2_TK16_1"}, {FACTOR_2, "BI39_SS2_TK16_2"}, {FACTOR_3, "BI39_SS2_TK16_3"}, {FACTOR_4, "BI39_SS2_TK16_4"}, {FACTOR_5, "BI39_SS2_TK16_5"}, {FACTOR_6, "BI39_SS2_TK16_6"}};
signal_by_factor TK17 = {{FACTOR_1, "BI39_SS2_TK17_1"}, {FACTOR_2, "BI39_SS2_TK17_2"}, {FACTOR_3, "BI39_SS2_TK17_3"}, {FACTOR_4, "BI39_SS2_TK17_4"}, {FACTOR_5, "BI39_SS2_TK17_5"}, {FACTOR_6, "BI39_SS2_TK17_6"}};
signal_by_factor TK18 = {{FACTOR_1, "BI39_SS2_TK18_1"}, {FACTOR_2, "BI39_SS2_TK18_2"}, {FACTOR_3, "BI39_SS2_TK18_3"}, {FACTOR_4, "BI39_SS2_TK18_4"}, {FACTOR_5, "BI39_SS2_TK18_5"}, {FACTOR_6, "BI39_SS2_TK18_6"}};
signal_by_factor TK19 = {{FACTOR_1, "BI39_SS2_TK19_1"}, {FACTOR_2, "BI39_SS2_TK19_2"}, {FACTOR_3, "BI39_SS2_TK19_3"}, {FACTOR_4, "BI39_SS2_TK19_4"}, {FACTOR_5, "BI39_SS2_TK19_5"}, {FACTOR_6, "BI39_SS2_TK19_6"}};
signal_by_factor TK20 = {{FACTOR_1, "BI39_SS2_TK20_1"}, {FACTOR_2, "BI39_SS2_TK20_2"}, {FACTOR_3, "BI39_SS2_TK20_3"}, {FACTOR_4, "BI39_SS2_TK20_4"}, {FACTOR_5, "BI39_SS2_TK20_5"}, {FACTOR_6, "BI39_SS2_TK20_6"}};
signal_by_factor TK21 = {{FACTOR_1, "BI39_SS2_TK21_1"}, {FACTOR_2, "BI39_SS2_TK21_2"}, {FACTOR_3, "BI39_SS2_TK21_3"}, {FACTOR_4, "BI39_SS2_TK21_4"}, {FACTOR_5, "BI39_SS2_TK21_5"}, {FACTOR_6, "BI39_SS2_TK21_6"}};
signal_by_factor TK22 = {{FACTOR_1, "BI39_SS2_TK22_1"}, {FACTOR_2, "BI39_SS2_TK22_2"}, {FACTOR_3, "BI39_SS2_TK22_3"}, {FACTOR_4, "BI39_SS2_TK22_4"}, {FACTOR_5, "BI39_SS2_TK22_5"}, {FACTOR_6, "BI39_SS2_TK22_6"}};
signal_by_factor TK23 = {{FACTOR_1, "BI39_SS2_TK23_1"}, {FACTOR_2, "BI39_SS2_TK23_2"}, {FACTOR_3, "BI39_SS2_TK23_3"}, {FACTOR_4, "BI39_SS2_TK23_4"}, {FACTOR_5, "BI39_SS2_TK23_5"}, {FACTOR_6, "BI39_SS2_TK23_6"}};
signal_by_factor TK24 = {{FACTOR_1, "BI39_SS2_TK24_1"}, {FACTOR_2, "BI39_SS2_TK24_2"}, {FACTOR_3, "BI39_SS2_TK24_3"}, {FACTOR_4, "BI39_SS2_TK24_4"}, {FACTOR_5, "BI39_SS2_TK24_5"}, {FACTOR_6, "BI39_SS2_TK24_6"}};
signal_by_factor TK25 = {{FACTOR_1, "BI39_SS2_TK25_1"}, {FACTOR_2, "BI39_SS2_TK25_2"}, {FACTOR_3, "BI39_SS2_TK25_3"}, {FACTOR_4, "BI39_SS2_TK25_4"}, {FACTOR_5, "BI39_SS2_TK25_5"}, {FACTOR_6, "BI39_SS2_TK25_6"}};
signal_by_factor TK26 = {{FACTOR_1, "BI39_SS2_TK26_1"}, {FACTOR_2, "BI39_SS2_TK26_2"}, {FACTOR_3, "BI39_SS2_TK26_3"}, {FACTOR_4, "BI39_SS2_TK26_4"}, {FACTOR_5, "BI39_SS2_TK26_5"}, {FACTOR_6, "BI39_SS2_TK26_6"}};
signal_by_factor TK27 = {{FACTOR_1, "BI39_SS2_TK27_1"}, {FACTOR_2, "BI39_SS2_TK27_2"}, {FACTOR_3, "BI39_SS2_TK27_3"}, {FACTOR_4, "BI39_SS2_TK27_4"}, {FACTOR_5, "BI39_SS2_TK27_5"}, {FACTOR_6, "BI39_SS2_TK27_6"}};
signal_by_factor TK28 = {{FACTOR_1, "BI39_SS2_TK28_1"}, {FACTOR_2, "BI39_SS2_TK28_2"}, {FACTOR_3, "BI39_SS2_TK28_3"}, {FACTOR_4, "BI39_SS2_TK28_4"}, {FACTOR_5, "BI39_SS2_TK28_5"}, {FACTOR_6, "BI39_SS2_TK28_6"}};
signal_by_factor TK29 = {{FACTOR_1, "BI39_SS2_TK29_1"}, {FACTOR_2, "BI39_SS2_TK29_2"}, {FACTOR_3, "BI39_SS2_TK29_3"}, {FACTOR_4, "BI39_SS2_TK29_4"}, {FACTOR_5, "BI39_SS2_TK29_5"}, {FACTOR_6, "BI39_SS2_TK29_6"}};
signal_by_factor TK30 = {{FACTOR_1, "BI39_SS2_TK30_1"}, {FACTOR_2, "BI39_SS2_TK30_2"}, {FACTOR_3, "BI39_SS2_TK30_3"}, {FACTOR_4, "BI39_SS2_TK30_4"}, {FACTOR_5, "BI39_SS2_TK30_5"}, {FACTOR_6, "BI39_SS2_TK30_6"}};
signal_by_factor TK31 = {{FACTOR_1, "BI39_SS2_TK31_1"}, {FACTOR_2, "BI39_SS2_TK31_2"}, {FACTOR_3, "BI39_SS2_TK31_3"}, {FACTOR_4, "BI39_SS2_TK31_4"}, {FACTOR_5, "BI39_SS2_TK31_5"}, {FACTOR_6, "BI39_SS2_TK31_6"}};
signal_by_factor TK32 = {{FACTOR_1, "BI39_SS2_TK32_1"}, {FACTOR_2, "BI39_SS2_TK32_2"}, {FACTOR_3, "BI39_SS2_TK32_3"}, {FACTOR_4, "BI39_SS2_TK32_4"}, {FACTOR_5, "BI39_SS2_TK32_5"}, {FACTOR_6, "BI39_SS2_TK32_6"}};
signal_by_factor TK33 = {{FACTOR_1, "BI39_SS2_TK33_1"}, {FACTOR_2, "BI39_SS2_TK33_2"}, {FACTOR_3, "BI39_SS2_TK33_3"}, {FACTOR_4, "BI39_SS2_TK33_4"}, {FACTOR_5, "BI39_SS2_TK33_5"}, {FACTOR_6, "BI39_SS2_TK33_6"}};



QMap<quint8, signal_by_factor*> signal_by_vin = {{VIN_TK1 , &TK1 }, {VIN_TK2 , &TK2 }, {VIN_TK3 , &TK3 }, {VIN_TK4 , &TK4 },
                                                 {VIN_TK5 , &TK5 }, {VIN_TK6 , &TK6 }, {VIN_TK7 , &TK7 }, {VIN_TK8 , &TK8 },
                                                 {VIN_TK9 , &TK9 }, {VIN_TK10, &TK10}, {VIN_TK11, &TK11}, {VIN_TK12, &TK12},
                                                 {VIN_TK13, &TK13}, {VIN_TK14, &TK14}, {VIN_TK15, &TK15}, {VIN_TK16, &TK16},
                                                 {VIN_TK17, &TK17}, {VIN_TK18, &TK18}, {VIN_TK19, &TK19}, {VIN_TK20, &TK20},
                                                 {VIN_TK21, &TK21}, {VIN_TK22, &TK22}, {VIN_TK23, &TK23}, {VIN_TK24, &TK24},
                                                 {VIN_TK25, &TK25}, {VIN_TK26, &TK26}, {VIN_TK27, &TK27}, {VIN_TK28, &TK28},
                                                 {VIN_TK29, &TK29}, {VIN_TK30, &TK30}, {VIN_TK31, &TK31}, {VIN_TK32, &TK32},
                                                 {VIN_TK33, &TK33}};







/** ******  ТИП ДАННЫХ 0x02  ********* **/
#define BI83_SS4_VD1   "BI83_SS4_VD1"
#define BI83_SS4_VD2   "BI83_SS4_VD2"
#define BI83_SS4_VD3   "BI83_SS4_VD3"
#define BI83_SS4_VD4   "BI83_SS4_VD4"
#define BI83_SS4_VD5   "BI83_SS4_VD5"
#define BI83_SS4_VD6   "BI83_SS4_VD6"
#define BI83_SS4_VD7   "BI83_SS4_VD7"
#define BI83_SS4_VD8   "BI83_SS4_VD8"
#define BI83_SS4_VD9   "BI83_SS4_VD9"
#define BI83_SS4_VD10  "BI83_SS4_VD10"
#define BI83_SS4_VD11  "BI83_SS4_VD11"
#define BI83_SS4_VD12  "BI83_SS4_VD12"
#define BI83_SS4_VD13  "BI83_SS4_VD13"
#define BI83_SS4_VD14  "BI83_SS4_VD14"
#define BI83_SS4_VD15  "BI83_SS4_VD15"
#define BI83_SS4_VDV1  "BI83_SS4_VDV1"
#define BI83_SS4_VDV2  "BI83_SS4_VDV2"
#define BI83_SS4_VDV3  "BI83_SS4_VDV3"
#define BI83_SS4_VD16  "BI83_SS4_VD16"

#define CALC_VD1( VAL) ((VAL >> 0) & 1)
#define CALC_VD2( VAL) ((VAL >> 1) & 1)
#define CALC_VD3( VAL) ((VAL >> 2) & 1)
#define CALC_VD4( VAL) ((VAL >> 3) & 1)
#define CALC_VD5( VAL) ((VAL >> 4) & 1)
#define CALC_VD6( VAL) ((VAL >> 5) & 1)
#define CALC_VD7( VAL) ((VAL >> 6) & 1)
#define CALC_VD8( VAL) ((VAL >> 7) & 1)
#define CALC_VD9( VAL) ((VAL >> 0) & 1)
#define CALC_VD10(VAL) ((VAL >> 1) & 1)
#define CALC_VD11(VAL) ((VAL >> 2) & 1)
#define CALC_VD12(VAL) ((VAL >> 3) & 1)
#define CALC_VD13(VAL) ((VAL >> 4) & 1)
#define CALC_VD14(VAL) ((VAL >> 5) & 1)
#define CALC_VD15(VAL) ((VAL >> 6) & 1)
#define CALC_VDV1(VAL) ((VAL >> 7) & 1)
#define CALC_VDV2(VAL) ((VAL >> 0) & 1)
#define CALC_VDV3(VAL) ((VAL >> 1) & 1)
#define CALC_VD16(VAL) ((VAL >> 2) & 1)


#endif // SKM_DEFS_H







