#ifndef OPA_DEFS_H
#define OPA_DEFS_H

#include <QMap>

/** ******  ТИП ДАННЫХ 0x02  ********* **/
#define FACTOR_RESET            0x00

#define FACTOR_f_TRESHOLD1      0x01
#define FACTOR_T_TRESHOLD1      0x02
#define FACTOR_dT_TRESHOLD1     0x03
#define FACTOR_DP_TRESHOLD1     0x04
#define FACTOR_CO_TRESHOLD1     0x05
#define FACTOR_VLAG_TRESHOLD1   0x0C
#define FACTOR_GOTV_TRESHOLD1   0x0D
#define FACTOR_HAND_TRESHOLD1   0xFF

#define FACTOR_f_TRESHOLD2      0x81
#define FACTOR_T_TRESHOLD2      0x82
#define FACTOR_VLAG_TRESHOLD2   0x8C
#define FACTOR_GOTV_TRESHOLD2   0x8D


typedef QMap<quint16, QString> signals_by_detector;
typedef QMap<quint8, signals_by_detector*> signals_table;


signals_by_detector signals_f_Treshold1 = {{1,  "BI21_1SS1_DK3_f"},			{2,  "BI21_1SS1_DK2_f"},		{3,  "BI21_1SS1_DK1_f"},
                                           {6,  "BI21_1SS1_DFV1_f"},		{11,  "BI22_2SS1_DF3_f"},		{12,  "BI22_2SS1_DF4_f"},
                                           {16,  "BI22_2SS1_DF6_f"},		{18,  "BI22_2SS1_DF7_f"},		{20,  "BI22_2SS1_DF8_f"},
                                           {22,  "BI22_2SS1_DF5_f"},		{25,  "BI22_2SS1_DK2_f"},		{26,  "BI22_2SS1_DF2_f"},
                                           {28,  "BI22_2SS1_DF1_f"},		{33,  "BI22_2SS1_DK1_f"},		{37,  "BI23_3SS1_DF13_f"},
                                           {38,  "BI23_3SS1_DF14_f"},		{40,  "BI23_3SS1_DF8_f"},		{43,  "BI23_3SS1_DF7_f"},
                                           {48,  "BI23_3SS1_DK10_f"},		{49,  "BI23_3SS1_DK9_f"},		{52,  "BI23_3SS1_DF20_f"},
                                           {54,  "BI23_3SS1_DF9_f"},		{57,  "BI23_3SS1_DF10_f"},	{61,  "BI23_3SS1_DF17_f"},
                                           {63,  "BI23_3SS1_DK8_f"},		{67,  "BI23_3SS1_DF19_f"},	{68,  "BI23_3SS1_DF18_f"},
                                           {70,  "BI23_3SS1_DK11_f"},		{71,  "BI23_3SS1_DK12_f"},	{72,  "BI23_3SS1_DF6_f"},
                                           {74,  "BI23_3SS1_DK7_f"},		{76,  "BI23_3SS1_DK5_f"},		{81,  "BI23_3SS1_DK6_f"},
                                           {82,  "BI23_3SS1_DK15_f"},		{86,  "BI23_3SS1_DK2_f"},		{89,  "BI23_3SS1_DK3_f"},
                                           {93,  "BI23_3SS1_DK4_f"},		{94,  "BI23_3SS1_DK14_f"},	{96,  "BI23_3SS1_DF5_f"},
                                           {97,  "BI23_3SS1_DK13_f"},		{98,  "BI23_3SS1_DF3_f"},		{101,  "BI23_3SS1_DF4_f"},
                                           {106,  "BI23_3SS1_DK1_f"},		{111, "BI24_4SS1_DF2_f"},   {112,  "BI24_4SS1_DF3_f"},
                                           {114,  "BI24_4SS1_DF1_f"},		{117, "BI24_4SS1_DK10_f"},	{119,  "BI24_4SS1_DK11_f"},
                                           {129,  "BI24_4SS1_DK12_f"},	{130, "BI24_4SS1_DK13_f"},  {131,  "BI24_4SS1_DF4_f"},
                                           {147,  "BI24_4SS1_DK2_f"},   {148, "BI24_4SS1_DK3_f"},   {149,  "BI24_4SS1_DK4_f"},
                                           {150,  "BI24_4SS1_DK5_f"},   {151, "BI24_4SS1_DK6_f"},   {152,  "BI24_4SS1_DK7_f"},
                                           {154,  "BI24_4SS1_DK9_f"},   {158, "BI26_6SS1_DF4_f"},   {159,  "BI26_6SS1_DF5_f"},
                                           {161,  "BI26_6SS1_DF10_f"},  {163, "BI26_6SS1_DK15_f"},  {164,  "BI26_6SS1_DK16_f"},
                                           {169,  "BI26_6SS1_DK14_f"},  {166, "BI26_6SS1_DK13_f"},	{170,  "BI26_6SS1_DF8_f"},
                                           {172,  "BI26_6SS1_DF9_f"},		{174, "BI26_6SS1_DF7_f"},	  {176,  "BI26_6SS1_DF3_f"},
                                           {176,  "BI26_6SS1_DF6_f"},		{178, "BI26_6SS1_DF2_f"},	  {179,  "BI26_6SS1_DK12_f"},
                                           {185,  "BI26_6SS1_DF1_f"},		{186, "BI26_6SS1_DK9_f"},	  {189,  "BI26_6SS1_DK10_f"},
                                           {211,  "BI26_6SS1_DK3_f"},		{212, "BI26_6SS1_DK4_f"},	  {213,  "BI26_6SS1_DK5_f"},
                                           {214,  "BI26_6SS1_DK6_f"},		{215, "BI26_6SS1_DK7_f"},	  {216,  "BI26_6SS1_DK8_f"},
                                           {220,  "BI25_5SS1_DF1_f"},		{221, "BI25_5SS1_DF2_f"},	  {224,  "BI25_5SS1_DK3_f"},
                                           {225,  "BI25_5SS1_DF4_f"},		{228, "BI25_5SS1_DF3_f"},	  {230,  "BI25_5SS1_DF5_f"},
                                           {235,  "BI25_5SS1_DK2_f"},		{244, "BI27_7SS1_DK6_f"},   {210,  "BI26_6SS1_DK2_f"},
                                           {246,  "BI27_7SS1_DK7_f"},		{248, "BI27_7SS1_DK8_f"},   {250,  "BI27_7SS1_DK9_f"},
                                           {252,  "BI27_7SS1_DK10_f"},  {254, "BI27_7SS1_DF2_f"},   {255,  "BI27_7SS1_DK11_f"},
                                           {255,  "BI28_8SS1_DF5_f"},		{257, "BI27_7SS1_DK4_f"},   {258,  "BI27_7SS1_DK5_f"},
                                           {259,  "BI27_7SS1_DF1_f"},		{260, "BI27_7SS1_DK1_f"},   {261,  "BI27_7SS1_DK2_f"},
                                           {262,  "BI27_7SS1_DK3_f"},		{264, "BI28_8SS1_DF4_f"},   {271,  "BI28_8SS1_DF3_f"},
                                           {275,  "BI28_8SS1_DF2_f"},		{277, "BI28_8SS1_DF1_f"},   {287,  "BI28_8SS1_DK2_f"},
                                           {288,  "BI28_8SS1_DK1_f"},		{289, "BI29_9SS1_DK1_f"},   {291,  "BI29_9SS1_DK2_f"},
                                           {293,  "BI29_9SS1_DK3_f"},		{295, "BI29_9SS1_DK4_f"},   {296,  "BI29_9SS1_DK5_f"},
                                           {298,  "BI29_9SS1_DK6_f"},		{299, "BI29_9SS1_DK7_f"},   {302,  "BI29_9SS1_DFV1_f"},
                                           {305,  "BI30_10SS1_DF1_f"},  {306, "BI30_10SS1_DF2_f"},	{309,  "BI30_10SS1_DK2_f"},
                                           {310,  "BI30_10SS1_DF7_f"},  {312, "BI30_10SS1_DF4_f"},	{313,  "BI30_10SS1_DF6_f"},
                                           {315,  "BI30_10SS1_DF5_f"},  {316, "BI30_10SS1_DK4_f"},	{319,  "BI30_10SS1_DF3_f"},
                                           {321,  "BI30_10SS1_DK3_f"},  {324, "BI30_10SS1_DK1_f"},	{326,  "BI30_10SS1_DK5_f"},
                                           {328,  "BI30_10SS1_DF8_f"},  {329, "BI30_10SS1_DF9_f"}};

signals_by_detector signals_HLADON_Treshold1 = {{145,  "BI24_4SS1_DK1_f1"},	{156,  "BI24_4SS1_DK8_f1"},     {191,  "BI26_6SS1_DK11_f1"},
                                               {209,  "BI26_6SS1_DK1_f1"},  {238,  "BI25_5SS1_DK1_f1"},     {126,  "BI24_4SS1_DK14_f1"}};


signals_by_detector signals_T_Treshold1 = {{1, "BI21_1SS1_DK3_T"},          {2, "BI21_1SS1_DK2_T"},         {3, "BI21_1SS1_DK1_T"},
                                           {4, "BI21_1SS1_DT1_T1"},         {5, "BI21_1SS1_DTV1_T"},        {8, "BI21_1SS1_DTV2_T"},
                                           {14, "BI22_2SS1_DT9_T1"},        {15, "BI22_2SS1_DT10_T1"},      {17, "BI22_2SS1_DT13_T1"},
                                           {21, "BI22_2SS1_DT7_T1"},        {23, "BI22_2SS1_DT12_T1"},      {24, "BI22_2SS1_DT11_T1"},
                                           {25, "BI22_2SS1_DK2_T"},         {29, "BI22_2SS1_DT8_T1"},       {30, "BI22_2SS1_DT4_T1"},
                                           {31, "BI22_2SS1_DT5_T1"},        {32, "BI22_2SS1_DT6_T1"},       {33, "BI22_2SS1_DK1_T"},
                                           {34, "BI22_2SS1_DT1_T1"},        {35, "BI22_2SS1_DT2_T1"},       {36, "BI22_2SS1_DT3_T1"},
                                           {42, "BI23_3SS1_DT26_T1"},       {44, "BI23_3SS1_DT37_T1"},      {45, "BI23_3SS1_DT38_T1"},
                                           {46, "BI23_3SS1_DT28_T1"},       {47, "BI23_3SS1_DT29_T1"},      {48, "BI23_3SS1_DK10_T"},
                                           {49, "BI23_3SS1_DK9_T"},         {50, "BI23_3SS1_DT36_T1"},      {51, "BI23_3SS1_DT35_T1"},
                                           {56, "BI23_3SS1_DT27_T1"},       {58, "BI23_3SS1_DT39_T1"},      {59, "BI23_3SS1_DT40_T1"},
                                           {60, "BI23_3SS1_DT30_T1"},       {63, "BI23_3SS1_DK8_T"},        {65, "BI23_3SS1_DT34_T1"},
                                           {66, "BI23_3SS1_DT33_T1"},       {70, "BI23_3SS1_DK11_T"},       {71, "BI23_3SS1_DK12_T"},
                                           {74, "BI23_3SS1_DK7_T"},         {75, "BI23_3SS1_DT13_T1"},      {76, "BI23_3SS1_DK5_T"},
                                           {77, "BI23_3SS1_DT20_T1"},       {78, "BI23_3SS1_DT21_T1"},      {79, "BI23_3SS1_DT22_T1"},
                                           {80, "BI23_3SS1_DT23_T1"},       {81, "BI23_3SS1_DK6_T"},        {82, "BI23_3SS1_DK15_T"},
                                           {83, "BI23_3SS1_DT24_T1"},       {84, "BI23_3SS1_DT25_T1"},      {85, "BI23_3SS1_DT14_T1"},
                                           {86, "BI23_3SS1_DK2_T"},         {87, "BI23_3SS1_DT15_T1"},      {88, "BI23_3SS1_DT16_T1"},
                                           {89, "BI23_3SS1_DK3_T"},         {90, "BI23_3SS1_DT17_T1"},      {91, "BI23_3SS1_DT18_T1"},
                                           {92, "BI23_3SS1_DT19_T1"},       {93, "BI23_3SS1_DK4_T"},        {94, "BI23_3SS1_DK14_T"},
                                           {95, "BI23_3SS1_DT12_T1"},       {97, "BI23_3SS1_DK13_T"},       {99, "BI23_3SS1_DT6_T1"},
                                           {100, "BI23_3SS1_DT7_T1"},       {102, "BI23_3SS1_DT8_T1"},      {103, "BI23_3SS1_DT3_T1"},
                                           {104, "BI23_3SS1_DT4_T1"},       {105, "BI23_3SS1_DT1_T1"},      {106, "BI23_3SS1_DK1_T"},
                                           {107, "BI23_3SS1_DT10_T1"},      {108, "BI23_3SS1_DT9_T1"},      {110, "BI23_3SS1_DT11_T1"},
                                           {116, "BI24_4SS1_DT15_T1"},      {117, "BI24_4SS1_DK10_T"},      {118, "BI24_4SS1_DT10_T1"},
                                           {119, "BI24_4SS1_DK11_T"},       {120, "BI24_4SS1_DT12_T1"},     {122, "BI24_4SS1_VTE1_T"},
                                           {123, "BI24_4SS1_DT11_T1"},      {124, "BI24_4SS1_DT16_T1"},     {175, "BI26_6SS1_DT18_T1"},
                                           {127, "BI24_4SS1_DT17_T1"},      {129, "BI24_4SS1_DK12_T"},      {130, "BI24_4SS1_DK13_T"},
                                           {134, "BI24_4SS1_DT1_T1"},       {135, "BI24_4SS1_DT2_T1"},      {136, "BI24_4SS1_DT3_T1"},
                                           {137, "BI24_4SS1_DT4_T1"},       {138, "BI24_4SS1_DT5_T1"},      {139, "BI24_4SS1_DT6_T1"},
                                           {140, "BI24_4SS1_DT7_T1"},       {142, "BI24_4SS1_DT8_T1"},      {143, "BI24_4SS1_DT9_T1"},
                                           {147, "BI24_4SS1_DK2_T"},        {148, "BI24_4SS1_DK3_T"},       {149, "BI24_4SS1_DK4_T"},
                                           {150, "BI24_4SS1_DK5_T"},        {151, "BI24_4SS1_DK6_T"},       {152, "BI24_4SS1_DK7_T"},
                                           {154, "BI24_4SS1_DK9_T"},        {163, "BI26_6SS1_DK15_T"},      {164, "BI26_6SS1_DK16_T"},
                                           {166, "BI26_6SS1_DK13_T"},       {167, "BI26_6SS1_DT12_T1"},     {168, "BI26_6SS1_DT13_T1"},
                                           {169, "BI26_6SS1_DK14_T"},       {173, "BI26_6SS1_DT17_T1"},     {179, "BI26_6SS1_DK12_T"},
                                           {180, "BI26_6SS1_DT15_T1"},      {181, "BI26_6SS1_DT16_T1"},     {186, "BI26_6SS1_DK9_T"},
                                           {188, "BI26_6SS1_DT19_T1"},      {189, "BI26_6SS1_DK10_T"},      {190, "BI26_6SS1_DT20_T1"},
                                           {195, "BI26_6SS1_DT9_T1"},       {197, "BI26_6SS1_DT1_T1"},      {198, "BI26_6SS1_DT2_T1"},
                                           {200, "BI26_6SS1_DT3_T1"},       {202, "BI26_6SS1_DT4_T1"},      {204, "BI26_6SS1_DT5_T1"},
                                           {205, "BI26_6SS1_DT6_T1"},       {207, "BI26_6SS1_DT7_T1"},      {208, "BI26_6SS1_DT8_T1"},
                                           {210, "BI26_6SS1_DK2_T"},        {211, "BI26_6SS1_DK3_T"},       {212, "BI26_6SS1_DK4_T"},
                                           {213, "BI26_6SS1_DK5_T"},        {214, "BI26_6SS1_DK6_T"},       {215, "BI26_6SS1_DK7_T"},
                                           {216, "BI26_6SS1_DK8_T"},        {223, "BI25_5SS1_VTE1_T"},      {224, "BI25_5SS1_DK3_T"},
                                           {227, "BI25_5SS1_VTE2_T"},       {229, "BI25_5SS1_DT5_T1"},      {232, "BI25_5SS1_DT1_T1"},
                                           {233, "BI25_5SS1_DT2_T1"},       {235, "BI25_5SS1_DK2_T"},       {236, "BI25_5SS1_DT3_T1"},
                                           {242, "BI25_5SS1_DT4_T1"},       {243, "BI27_7SS1_DT1_T1"},      {244, "BI27_7SS1_DK6_T"},
                                           {245, "BI27_7SS1_DT2_T1"},       {246, "BI27_7SS1_DK7_T"},       {247, "BI27_7SS1_DT3_T1"},
                                           {248, "BI27_7SS1_DK8_T"},        {249, "BI27_7SS1_DT4_T1"},      {250, "BI27_7SS1_DK9_T"},
                                           {251, "BI27_7SS1_DT5_T1"},       {252, "BI27_7SS1_DK10_T"},      {253, "BI27_7SS1_DT6_T1"},
                                           {255, "BI27_7SS1_DK11_T"},       {257, "BI27_7SS1_DK4_T"},       {258, "BI27_7SS1_DK5_T"},
                                           {260, "BI27_7SS1_DK1_T"},        {261, "BI27_7SS1_DK2_T"},       {262, "BI27_7SS1_DK3_T"},
                                           {266, "BI28_8SS1_DT13_T1"},      {267, "BI28_8SS1_DT14_T1"},     {269, "BI28_8SS1_DT9_T1"},
                                           {270, "BI28_8SS1_DT10_T1"},      {273, "BI28_8SS1_DT5_T1"},      {274, "BI28_8SS1_DT4_T1"},
                                           {279, "BI28_8SS1_DT11_T1"},      {280, "BI28_8SS1_DT12_T1"},     {281, "BI28_8SS1_DT7_T1"},
                                           {282, "BI28_8SS1_DT8_T1"},       {283, "BI28_8SS1_DT6_T1"},      {284, "BI28_8SS1_DT1_T1"},
                                           {285, "BI28_8SS1_DT3_T1"},       {286, "BI28_8SS1_DT2_T1"},      {287, "BI28_8SS1_DK2_T"},
                                           {288, "BI28_8SS1_DK1_T"},        {289, "BI29_9SS1_DK1_T"},       {291, "BI29_9SS1_DK2_T"},
                                           {293, "BI29_9SS1_DK3_T"},        {295, "BI29_9SS1_DK4_T"},       {296, "BI29_9SS1_DK5_T"},
                                           {297, "BI29_9SS1_DT1_T1"},       {298, "BI29_9SS1_DK6_T"},       {299, "BI29_9SS1_DK7_T"},
                                           {300, "BI29_9SS1_DTV1_T1"},      {301, "BI29_9SS1_DTV2_T1"},     {307, "BI30_10SS1_DT3_T1"},
                                           {309, "BI30_10SS1_DK2_T"},       {314, "BI30_10SS1_DT5_T1"},     {316, "BI30_10SS1_DK4_T"},
                                           {317, "BI30_10SS1_DT6_T1"},      {321, "BI30_10SS1_DK3_T"},      {322, "BI30_10SS1_DT4_T1"},
                                           {323, "BI30_10SS1_DT2_T1"},      {324, "BI30_10SS1_DK1_T"},      {325, "BI30_10SS1_DT1_T1"},
                                           {326, "BI30_10SS1_DK5_T"},       {327, "BI30_10SS1_VTE1_T"},     {330, "BI30_10SS1_VTE2_T"},
                                           {331, "BI82_12SS1_DTV1_T1"},     {334, "BI82_12SS1_DTV2_T1"},    {336, "BI82_12SS1_DTV5_T1"},
                                           {339, "BI82_12SS1_DTV6_T1"},     {341, "BI82_12SS1_DTV4_T1"},    {344, "BI82_12SS1_DTV3_T1"},
                                           {346, "BI82_12SS1_DTV7_T1"},     {349, "BI82_12SS1_DTV8_T1"},    {351, "BI82_12SS1_DTV9_T1"},
                                           {354, "BI82_12SS1_DTV10_T1"},    {356, "BI82_12SS1_DTV11_T1"},   {359, "BI82_12SS1_DTV12_T1"}};

signals_by_detector signals_T_Treshold1_Z = {{331, "BI82_12SS1_DTV1_T1_Z"},     {334, "BI82_12SS1_DTV2_T1_Z"},	{336, "BI82_12SS1_DTV5_T1_Z"},
                                             {339, "BI82_12SS1_DTV6_T1_Z"},     {341, "BI82_12SS1_DTV4_T1_Z"},	{344, "BI82_12SS1_DTV3_T1_Z"},
                                             {346, "BI82_12SS1_DTV7_T1_Z"},     {349, "BI82_12SS1_DTV8_T1_Z"},	{351, "BI82_12SS1_DTV9_T1_Z"},
                                             {354, "BI82_12SS1_DTV10_T1_Z"},    {356, "BI82_12SS1_DTV11_T1_Z"}, {359, "BI82_12SS1_DTV12_T1_Z"}};


signals_by_detector signals_dT_Treshold1 = {{1, "BI21_1SS1_DK3_dT"},        {2, "BI21_1SS1_DK2_dT"},        {3, "BI21_1SS1_DK1_dT"},
                                            {4, "BI21_1SS1_DT1_dT"},        {5, "BI21_1SS1_DTV1_dT"},       {8, "BI21_1SS1_DTV2_dT"},
                                            {14, "BI22_2SS1_DT9_dT"},       {15, "BI22_2SS1_DT10_dT"},		{17, "BI22_2SS1_DT13_dT"},
                                            {23, "BI22_2SS1_DT12_dT"},		{24, "BI22_2SS1_DT11_dT"},		{25, "BI22_2SS1_DK2_dT"},
                                            {29, "BI22_2SS1_DT8_dT"},       {30, "BI22_2SS1_DT4_dT"},       {31, "BI22_2SS1_DT5_dT"},
                                            {32, "BI22_2SS1_DT6_dT"},       {33, "BI22_2SS1_DK1_dT"},       {34, "BI22_2SS1_DT1_dT"},
                                            {35, "BI22_2SS1_DT2_dT"},       {36, "BI22_2SS1_DT3_dT"},       {42, "BI23_3SS1_DT26_dT"},
                                            {44, "BI23_3SS1_DT37_dT"},		{45, "BI23_3SS1_DT38_dT"},		{46, "BI23_3SS1_DT28_dT"},
                                            {47, "BI23_3SS1_DT29_dT"},		{48, "BI23_3SS1_DK10_dT"},		{49, "BI23_3SS1_DK9_dT"},
                                            {51, "BI23_3SS1_DT35_dT"},		{56, "BI23_3SS1_DT27_dT"},		{58, "BI23_3SS1_DT39_dT"},
                                            {59, "BI23_3SS1_DT40_dT"},		{60, "BI23_3SS1_DT30_dT"},		{63, "BI23_3SS1_DK8_dT"},
                                            {65, "BI23_3SS1_DT34_dT"},		{66, "BI23_3SS1_DT33_dT"},		{70, "BI23_3SS1_DK11_dT"},
                                            {71, "BI23_3SS1_DK12_dT"},		{74, "BI23_3SS1_DK7_dT"},       {76, "BI23_3SS1_DK5_dT"},
                                            {77, "BI23_3SS1_DT20_dT"},		{78, "BI23_3SS1_DT21_dT"},		{79, "BI23_3SS1_DT22_dT"},
                                            {80, "BI23_3SS1_DT23_dT"},		{81, "BI23_3SS1_DK6_dT"},       {82, "BI23_3SS1_DK15_dT"},
                                            {83, "BI23_3SS1_DT24_dT"},		{84, "BI23_3SS1_DT25_dT"},		{85, "BI23_3SS1_DT14_dT"},
                                            {86, "BI23_3SS1_DK2_dT"},       {87, "BI23_3SS1_DT15_dT"},		{88, "BI23_3SS1_DT16_dT"},
                                            {89, "BI23_3SS1_DK3_dT"},       {90, "BI23_3SS1_DT17_dT"},		{91, "BI23_3SS1_DT18_dT"},
                                            {92, "BI23_3SS1_DT19_dT"},		{93, "BI23_3SS1_DK4_dT"},       {94, "BI23_3SS1_DK14_dT"},
                                            {95, "BI23_3SS1_DT12_dT"},		{97, "BI23_3SS1_DK13_dT"},		{99, "BI23_3SS1_DT6_dT"},
                                            {100, "BI23_3SS1_DT7_dT"},		{102, "BI23_3SS1_DT8_dT"},		{103, "BI23_3SS1_DT3_dT"},
                                            {104, "BI23_3SS1_DT4_dT"},		{105, "BI23_3SS1_DT1_dT"},		{106, "BI23_3SS1_DK1_dT"},
                                            {107, "BI23_3SS1_DT10_dT"},		{108, "BI23_3SS1_DT9_dT"},		{110, "BI23_3SS1_DT11_dT"},
                                            {117, "BI24_4SS1_DK10_dT"},		{118, "BI24_4SS1_DT10_dT"},		{119, "BI24_4SS1_DK11_dT"},
                                            {120, "BI24_4SS1_DT12_dT"},		{123, "BI24_4SS1_DT11_dT"},		{129, "BI24_4SS1_DK12_dT"},
                                            {130, "BI24_4SS1_DK13_dT"},		{134, "BI24_4SS1_DT1_dT"},		{135, "BI24_4SS1_DT2_dT"},
                                            {136, "BI24_4SS1_DT3_dT"},		{137, "BI24_4SS1_DT4_dT"},		{138, "BI24_4SS1_DT5_dT"},
                                            {139, "BI24_4SS1_DT6_dT"},		{140, "BI24_4SS1_DT7_dT"},		{142, "BI24_4SS1_DT8_dT"},
                                            {143, "BI24_4SS1_DT9_dT"},		{147, "BI24_4SS1_DK2_dT"},		{148, "BI24_4SS1_DK3_dT"},
                                            {149, "BI24_4SS1_DK4_dT"},		{150, "BI24_4SS1_DK5_dT"},		{151, "BI24_4SS1_DK6_dT"},
                                            {152, "BI24_4SS1_DK7_dT"},		{154, "BI24_4SS1_DK9_dT"},		{163, "BI26_6SS1_DK15_dT"},
                                            {164, "BI26_6SS1_DK16_dT"},		{166, "BI26_6SS1_DK13_dT"},		{167, "BI26_6SS1_DT12_dT"},
                                            {168, "BI26_6SS1_DT13_dT"},		{169, "BI26_6SS1_DK14_dT"},		{173, "BI26_6SS1_DT17_dT"},
                                            {179, "BI26_6SS1_DK12_dT"},		{180, "BI26_6SS1_DT15_dT"},		{181, "BI26_6SS1_DT16_dT"},
                                            {186, "BI26_6SS1_DK9_dT"},		{189, "BI26_6SS1_DK10_dT"},		{195, "BI26_6SS1_DT9_dT"},
                                            {197, "BI26_6SS1_DT1_dT"},		{198, "BI26_6SS1_DT2_dT"},		{200, "BI26_6SS1_DT3_dT"},
                                            {202, "BI26_6SS1_DT4_dT"},		{204, "BI26_6SS1_DT5_dT"},		{205, "BI26_6SS1_DT6_dT"},
                                            {207, "BI26_6SS1_DT7_dT"},		{208, "BI26_6SS1_DT8_dT"},		{210, "BI26_6SS1_DK2_dT"},
                                            {211, "BI26_6SS1_DK3_dT"},		{212, "BI26_6SS1_DK4_dT"},		{213, "BI26_6SS1_DK5_dT"},
                                            {214, "BI26_6SS1_DK6_dT"},		{215, "BI26_6SS1_DK7_dT"},		{216, "BI26_6SS1_DK8_dT"},
                                            {224, "BI25_5SS1_DK3_dT"},		{229, "BI25_5SS1_DT5_dT"},		{232, "BI25_5SS1_DT1_dT"},
                                            {233, "BI25_5SS1_DT2_dT"},		{235, "BI25_5SS1_DK2_dT"},		{236, "BI25_5SS1_DT3_dT"},
                                            {242, "BI25_5SS1_DT4_dT"},		{243, "BI27_7SS1_DT1_dT"},		{244, "BI27_7SS1_DK6_dT"},
                                            {245, "BI27_7SS1_DT2_dT"},		{246, "BI27_7SS1_DK7_dT"},		{247, "BI27_7SS1_DT3_dT"},
                                            {248, "BI27_7SS1_DK8_dT"},		{249, "BI27_7SS1_DT4_dT"},		{250, "BI27_7SS1_DK9_dT"},
                                            {251, "BI27_7SS1_DT5_dT"},		{252, "BI27_7SS1_DK10_dT"},		{253, "BI27_7SS1_DT6_dT"},
                                            {255, "BI27_7SS1_DK11_dT"},		{257, "BI27_7SS1_DK4_dT"},		{258, "BI27_7SS1_DK5_dT"},
                                            {260, "BI27_7SS1_DK1_dT"},		{261, "BI27_7SS1_DK2_dT"},		{262, "BI27_7SS1_DK3_dT"},
                                            {266, "BI28_8SS1_DT13_dT"},		{267, "BI28_8SS1_DT14_dT"},		{269, "BI28_8SS1_DT9_dT"},
                                            {270, "BI28_8SS1_DT10_dT"},		{273, "BI28_8SS1_DT5_dT"},		{274, "BI28_8SS1_DT4_dT"},
                                            {279, "BI28_8SS1_DT11_dT"},		{280, "BI28_8SS1_DT12_dT"},		{281, "BI28_8SS1_DT7_dT"},
                                            {282, "BI28_8SS1_DT8_dT"},		{283, "BI28_8SS1_DT6_dT"},		{285, "BI28_8SS1_DT3_dT"},
                                            {285, "BI28_8SS1_DT1_dT"},		{286, "BI28_8SS1_DT2_dT"},		{287, "BI28_8SS1_DK2_dT"},
                                            {288, "BI28_8SS1_DK1_dT"},		{289, "BI29_9SS1_DK1_dT"},		{291, "BI29_9SS1_DK2_dT"},
                                            {293, "BI29_9SS1_DK3_dT"},		{295, "BI29_9SS1_DK4_dT"},		{296, "BI29_9SS1_DK5_dT"},
                                            {298, "BI29_9SS1_DK6_dT"},		{299, "BI29_9SS1_DK7_dT"},		{307, "BI30_10SS1_DT3_dT"},
                                            {309, "BI30_10SS1_DK2_dT"},		{316, "BI30_10SS1_DK4_dT"},		{321, "BI30_10SS1_DK3_dT"},
                                            {322, "BI30_10SS1_DT4_dT"},		{323, "BI30_10SS1_DT2_dT"},		{324, "BI30_10SS1_DK1_dT"},
                                            {325, "BI30_10SS1_DT1_dT"},		{326, "BI30_10SS1_DK5_dT"},		{331, "BI82_12SS1_DTV1_dT"},
                                            {334, "BI82_12SS1_DTV2_dT"},	{336, "BI82_12SS1_DTV5_dT"},	{339, "BI82_12SS1_DTV6_dT"},
                                            {341, "BI82_12SS1_DTV4_dT"},	{344, "BI82_12SS1_DTV3_dT"},	{346, "BI82_12SS1_DTV7_dT"},
                                            {349, "BI82_12SS1_DTV8_dT"},	{351, "BI82_12SS1_DTV9_dT"},	{354, "BI82_12SS1_DTV10_dT"},
                                            {356, "BI82_12SS1_DTV11_dT"},	{359, "BI82_12SS1_DTV12_dT"}};

signals_by_detector signals_dT_Treshold1_Z = {{331, "BI82_12SS1_DTV1_dT_Z"},	{334, "BI82_12SS1_DTV2_dT_Z"},  {336, "BI82_12SS1_DTV5_dT_Z"},
                                              {339, "BI82_12SS1_DTV6_dT_Z"},	{341, "BI82_12SS1_DTV4_dT_Z"},  {344, "BI82_12SS1_DTV3_dT_Z"},
                                              {346, "BI82_12SS1_DTV7_dT_Z"},	{349, "BI82_12SS1_DTV8_dT_Z"},  {351, "BI82_12SS1_DTV9_dT_Z"},
                                              {354, "BI82_12SS1_DTV10_dT_Z"},	{356, "BI82_12SS1_DTV11_dT_Z"}, {359, "BI82_12SS1_DTV12_dT_Z"}};


signals_by_detector signals_DP_Treshold1 = {{7, "BI21_1SS1_DP1"},       {9, "BI21_1SS1_DP2"},       {125, "BI24_4SS1_DP5"},
                                            {128, "BI24_4SS1_DP6"},		{133, "BI24_4SS1_DP1"},		{144, "BI24_4SS1_DP2"},
                                            {146, "BI24_4SS1_DP3"},		{153, "BI24_4SS1_DP4"},		{187, "BI26_6SS1_DP6"},
                                            {193, "BI26_6SS1_DP7"},		{194, "BI26_6SS1_DP5"},		{199, "BI26_6SS1_DP1"},
                                            {201, "BI26_6SS1_DP2"},		{203, "BI26_6SS1_DP3"},		{206, "BI26_6SS1_DP4"},
                                            {234, "BI25_5SS1_DP2"},		{237, "BI25_5SS1_DP1"},		{256, "BI27_7SS1_DP1"},
                                            {257, "BI82_12SS1_DP11"},	{332, "BI82_12SS1_DP1"},    {335, "BI82_12SS1_DP2"},
                                            {337, "BI82_12SS1_DP5"},	{340, "BI82_12SS1_DP6"},    {342, "BI82_12SS1_DP4"},
                                            {345, "BI82_12SS1_DP3"},	{347, "BI82_12SS1_DP7"},    {350, "BI82_12SS1_DP8"},
                                            {352, "BI82_12SS1_DP9"},	{355, "BI82_12SS1_DP10"},   {360, "BI82_12SS1_DP12"}};

signals_by_detector signals_DP_Treshold1_Z = {{257, "BI82_12SS1_DP11_Z"},   {332, "BI82_12SS1_DP1_Z"},	{335, "BI82_12SS1_DP2_Z"},
                                              {337, "BI82_12SS1_DP5_Z"},    {340, "BI82_12SS1_DP6_Z"},	{342, "BI82_12SS1_DP4_Z"},
                                              {345, "BI82_12SS1_DP3_Z"},    {347, "BI82_12SS1_DP7_Z"},	{350, "BI82_12SS1_DP8_Z"},
                                              {352, "BI82_12SS1_DP9_Z"},	{355, "BI82_12SS1_DP10_Z"},	{360, "BI82_12SS1_DP12_Z"}};


signals_by_detector signals_CO_Treshold1 = {{70, "BI23_3SS1_DK11_V"},			{71, "BI23_3SS1_DK12_V"},			{97, "BI23_3SS1_DK13_V"},
                                            {129, "BI24_4SS1_DK12_V"},		{147, "BI24_4SS1_DK2_V"},			{154, "BI24_4SS1_DK9_V"},
                                            {189, "BI26_6SS1_DK10_V"},		{210, "BI26_6SS1_DK2_V"},			{216, "BI26_6SS1_DK8_V"},
                                            {235, "BI25_5SS1_DK2_V"},			{287, "BI28_8SS1_DK2_V"},			{288, "BI28_8SS1_DK1_V"}};


signals_by_detector signals_VLAG_Treshold1 = {{21, "BI22_2SS1_DT7_f1"},			{50, "BI23_3SS1_DT36_f1"},			{75, "BI23_3SS1_DT13_f1"},
                                              {116, "BI24_4SS1_DT15_f1"},		{124, "BI24_4SS1_DT16_f1"},			{175, "BI26_6SS1_DT18_f1"},
                                              {127, "BI24_4SS1_DT17_f1"},		{188, "BI26_6SS1_DT19_f1"},			{190, "BI26_6SS1_DT20_f1"},
                                              {297, "BI29_9SS1_DT1_f1"},		{300, "BI29_9SS1_DTV1_f1"},			{301, "BI29_9SS1_DTV2_f1"},
                                              {314, "BI30_10SS1_DT5_f1"},		{317, "BI30_10SS1_DT6_f1"}};


signals_by_detector signals_HAND_Treshold1 = {{10, "BI21_1SS1_RV1"},	{13, "BI22_2SS1_R2"},       {19, "BI22_2SS1_R3"},
                                              {27, "BI22_2SS1_R1"},     {39, "BI23_3SS1_R5"},       {41, "BI23_3SS1_R3"},
                                              {53, "BI23_3SS1_R9"},     {55, "BI23_3SS1_R4"},       {62, "BI23_3SS1_R6"},
                                              {64, "BI23_3SS1_R8"},     {69, "BI23_3SS1_R7"},       {73, "BI23_3SS1_R2"},
                                              {109, "BI23_3SS1_R1"},	{113, "BI24_4SS1_R5"},		{115, "BI24_4SS1_R3"},
                                              {121, "BI24_4SS1_R4"},	{132, "BI24_4SS1_R6"},		{141, "BI24_4SS1_R1"},
                                              {155, "BI24_4SS1_R2"},	{157, "BI26_6SS1_R7"},		{160, "BI26_6SS1_R8"},
                                              {162, "BI26_6SS1_R10"},	{165, "BI26_6SS1_R11"},		{171, "BI26_6SS1_R6"},
                                              {177, "BI26_6SS1_R5"},	{182, "BI26_6SS1_R9"},		{184, "BI26_6SS1_R4"},
                                              {192, "BI26_6SS1_R3"},	{196, "BI26_6SS1_R1"},		{217, "BI26_6SS1_R2"},
                                              {219, "BI25_5SS1_R2"},	{222, "BI25_5SS1_R3"},		{226, "BI25_5SS1_R4"},
                                              {231, "BI25_5SS1_R5"},	{239, "BI25_5SS1_R1"},		{263, "BI28_8SS1_R4"},
                                              {268, "BI28_8SS1_R5"},	{272, "BI28_8SS1_R3"},		{276, "BI28_8SS1_R2"},
                                              {278, "BI28_8SS1_R1"},	{290, "BI29_9SS1_R1"},		{292, "BI29_9SS1_R2"},
                                              {294, "BI29_9SS1_R3"},	{303, "BI29_9SS1_RV1"},		{304, "BI30_10SS1_R1"},
                                              {305, "BI30_10SS1_R2"},	{311, "BI30_10SS1_R5"},		{318, "BI30_10SS1_R4"},
                                              {320, "BI30_10SS1_R3"},	{333, "BI82_12SS1_RV1"},	{338, "BI82_12SS1_RV3"},
                                              {343, "BI82_12SS1_RV2"},  {348, "BI82_12SS1_RV4"},    {353, "BI82_12SS1_RV5"},
                                              {358, "BI82_12SS1_RV6"}};

signals_by_detector signals_HAND_Treshold1_Z = {{333, "BI82_12SS1_RV1_Z"},  {338, "BI82_12SS1_RV3_Z"},	{343, "BI82_12SS1_RV2_Z"},
                                                {348, "BI82_12SS1_RV4_Z"},  {353, "BI82_12SS1_RV5_Z"},	{358, "BI82_12SS1_RV6_Z"}};


//signals_by_detector signals_f_Treshold2 = {{126, "BI24_4SS1_DK14_f2"},		{145, "BI24_4SS1_DK1_f2"},			{156, "BI24_4SS1_DK8_f2"},
//                                           {191, "BI26_6SS1_DK11_f2"},		{209, "BI26_6SS1_DK1_f2"},			{238, "BI25_5SS1_DK1_f2"}};

signals_by_detector signals_HLADON_Treshold2 = {{145,  "BI24_4SS1_DK1_f2"},     {156,  "BI24_4SS1_DK8_f2"},     {191,  "BI26_6SS1_DK11_f2"},
                                                {209,  "BI26_6SS1_DK1_f2"},     {238,  "BI25_5SS1_DK1_f2"},     {126,  "BI24_4SS1_DK14_f2"}};

signals_by_detector signals_T_Treshold2 = {{4, "BI21_1SS1_DT1_T2"},     {14, "BI22_2SS1_DT9_T2"},     {15, "BI22_2SS1_DT10_T2"},
                                           {17, "BI22_2SS1_DT13_T2"},		{21, "BI22_2SS1_DT7_T2"},     {23, "BI22_2SS1_DT12_T2"},
                                           {24, "BI22_2SS1_DT11_T2"},		{29, "BI22_2SS1_DT8_T2"},     {30, "BI22_2SS1_DT4_T2"},
                                           {31, "BI22_2SS1_DT5_T2"},		{32, "BI22_2SS1_DT6_T2"},     {34, "BI22_2SS1_DT1_T2"},
                                           {35, "BI22_2SS1_DT2_T2"},		{36, "BI22_2SS1_DT3_T2"},     {42, "BI23_3SS1_DT26_T2"},
                                           {44, "BI23_3SS1_DT37_T2"},		{45, "BI23_3SS1_DT38_T2"},		{46, "BI23_3SS1_DT28_T2"},
                                           {47, "BI23_3SS1_DT29_T2"},		{50, "BI23_3SS1_DT36_T2"},		{51, "BI23_3SS1_DT35_T2"},
                                           {56, "BI23_3SS1_DT27_T2"},		{58, "BI23_3SS1_DT39_T2"},		{59, "BI23_3SS1_DT40_T2"},
                                           {60, "BI23_3SS1_DT30_T2"},		{65, "BI23_3SS1_DT34_T2"},		{66, "BI23_3SS1_DT33_T2"},
                                           {75, "BI23_3SS1_DT13_T2"},		{77, "BI23_3SS1_DT20_T2"},		{78, "BI23_3SS1_DT21_T2"},
                                           {79, "BI23_3SS1_DT22_T2"},		{80, "BI23_3SS1_DT23_T2"},		{83, "BI23_3SS1_DT24_T2"},
                                           {84, "BI23_3SS1_DT25_T2"},		{85, "BI23_3SS1_DT14_T2"},		{87, "BI23_3SS1_DT15_T2"},
                                           {88, "BI23_3SS1_DT16_T2"},		{90, "BI23_3SS1_DT17_T2"},		{91, "BI23_3SS1_DT18_T2"},
                                           {92, "BI23_3SS1_DT19_T2"},		{95, "BI23_3SS1_DT12_T2"},		{99, "BI23_3SS1_DT6_T2"},
                                           {100, "BI23_3SS1_DT7_T2"},		{102, "BI23_3SS1_DT8_T2"},		{103, "BI23_3SS1_DT3_T2"},
                                           {104, "BI23_3SS1_DT4_T2"},		{105, "BI23_3SS1_DT1_T2"},		{107, "BI23_3SS1_DT10_T2"},
                                           {108, "BI23_3SS1_DT9_T2"},		{110, "BI23_3SS1_DT11_T2"},		{116, "BI24_4SS1_DT15_T2"},
                                           {118, "BI24_4SS1_DT10_T2"},	{120, "BI24_4SS1_DT12_T2"},		{123, "BI24_4SS1_DT11_T2"},
                                           {124, "BI24_4SS1_DT16_T2"},	{175, "BI26_6SS1_DT18_T2"},		{127, "BI24_4SS1_DT17_T2"},
                                           {134, "BI24_4SS1_DT1_T2"},   {135, "BI24_4SS1_DT2_T2"},		{136, "BI24_4SS1_DT3_T2"},
                                           {137, "BI24_4SS1_DT4_T2"},   {138, "BI24_4SS1_DT5_T2"},		{139, "BI24_4SS1_DT6_T2"},
                                           {140, "BI24_4SS1_DT7_T2"},   {142, "BI24_4SS1_DT8_T2"},		{143, "BI24_4SS1_DT9_T2"},
                                           {167, "BI26_6SS1_DT12_T2"},	{168, "BI26_6SS1_DT13_T2"},		{173, "BI26_6SS1_DT17_T2"},
                                           {180, "BI26_6SS1_DT15_T2"},	{181, "BI26_6SS1_DT16_T2"},		{188, "BI26_6SS1_DT19_T2"},
                                           {190, "BI26_6SS1_DT20_T2"},	{195, "BI26_6SS1_DT9_T2"},		{197, "BI26_6SS1_DT1_T2"},
                                           {198, "BI26_6SS1_DT2_T2"},   {200, "BI26_6SS1_DT3_T2"},		{202, "BI26_6SS1_DT4_T2"},
                                           {204, "BI26_6SS1_DT5_T2"},   {205, "BI26_6SS1_DT6_T2"},		{207, "BI26_6SS1_DT7_T2"},
                                           {208, "BI26_6SS1_DT8_T2"},   {229, "BI25_5SS1_DT5_T2"},		{232, "BI25_5SS1_DT1_T2"},
                                           {233, "BI25_5SS1_DT2_T2"},   {236, "BI25_5SS1_DT3_T2"},		{242, "BI25_5SS1_DT4_T2"},
                                           {243, "BI27_7SS1_DT1_T2"},   {245, "BI27_7SS1_DT2_T2"},		{247, "BI27_7SS1_DT3_T2"},
                                           {249, "BI27_7SS1_DT4_T2"},   {251, "BI27_7SS1_DT5_T2"},		{253, "BI27_7SS1_DT6_T2"},
                                           {266, "BI28_8SS1_DT13_T2"},	{267, "BI28_8SS1_DT14_T2"},		{269, "BI28_8SS1_DT9_T2"},
                                           {270, "BI28_8SS1_DT10_T2"},	{273, "BI28_8SS1_DT5_T2"},		{274, "BI28_8SS1_DT4_T2"},
                                           {279, "BI28_8SS1_DT11_T2"},	{280, "BI28_8SS1_DT12_T2"},		{281, "BI28_8SS1_DT7_T2"},
                                           {282, "BI28_8SS1_DT8_T2"},   {283, "BI28_8SS1_DT6_T2"},		{284, "BI28_8SS1_DT1_T2"},
                                           {285, "BI28_8SS1_DT3_T2"},   {286, "BI28_8SS1_DT2_T2"},		{297, "BI29_9SS1_DT1_T2"},
                                           {300, "BI29_9SS1_DTV1_T2"},	{301, "BI29_9SS1_DTV2_T2"},		{307, "BI30_10SS1_DT3_T2"},
                                           {314, "BI30_10SS1_DT5_T2"},	{317, "BI30_10SS1_DT6_T2"},		{322, "BI30_10SS1_DT4_T2"},
                                           {323, "BI30_10SS1_DT2_T2"},	{325, "BI30_10SS1_DT1_T2"},		{331, "BI82_12SS1_DTV1_T2"},
                                           {334, "BI82_12SS1_DTV2_T2"},	{336, "BI82_12SS1_DTV5_T2"},	{339, "BI82_12SS1_DTV6_T2"},
                                           {341, "BI82_12SS1_DTV4_T2"},	{344, "BI82_12SS1_DTV3_T2"},	{346, "BI82_12SS1_DTV7_T2"},
                                           {349, "BI82_12SS1_DTV8_T2"},	{351, "BI82_12SS1_DTV9_T2"},	{354, "BI82_12SS1_DTV10_T2"},
                                           {356, "BI82_12SS1_DTV11_T2"},{359, "BI82_12SS1_DTV12_T2"}};

signals_by_detector signals_T_Treshold2_Z = {{331, "BI82_12SS1_DTV1_T2_Z"},	  {334, "BI82_12SS1_DTV2_T2_Z"},  {336, "BI82_12SS1_DTV5_T2_Z"},
                                             {339, "BI82_12SS1_DTV6_T2_Z"},	  {341, "BI82_12SS1_DTV4_T2_Z"},  {344, "BI82_12SS1_DTV3_T2_Z"},
                                             {346, "BI82_12SS1_DTV7_T2_Z"},	  {349, "BI82_12SS1_DTV8_T2_Z"},  {351, "BI82_12SS1_DTV9_T2_Z"},
                                             {354, "BI82_12SS1_DTV10_T2_Z"},  {356, "BI82_12SS1_DTV11_T2_Z"}, {359, "BI82_12SS1_DTV12_T2_Z"}};


signals_by_detector signals_VLAG_Treshold2 = {{21,  "BI22_2SS1_DT7_f2"},			{50,  "BI23_3SS1_DT36_f2"},			{75,  "BI23_3SS1_DT13_f2"},
                                              {116, "BI24_4SS1_DT15_f2"},			{124, "BI24_4SS1_DT16_f2"},			{175, "BI26_6SS1_DT18_f2"},
                                              {127, "BI24_4SS1_DT17_f2"},			{188, "BI26_6SS1_DT19_f2"},			{190, "BI26_6SS1_DT20_f2"},
                                              {297, "BI29_9SS1_DT1_f2"},			{300, "BI29_9SS1_DTV1_f2"},			{301, "BI29_9SS1_DTV2_f2"},
                                              {314, "BI30_10SS1_DT5_f2"},			{317, "BI30_10SS1_DT6_f2"}};



signals_table SIGNALS_TABLE = {{FACTOR_f_TRESHOLD1,     &signals_f_Treshold1},
                               {FACTOR_T_TRESHOLD1,     &signals_T_Treshold1},
                               {FACTOR_dT_TRESHOLD1,    &signals_dT_Treshold1},
                               {FACTOR_DP_TRESHOLD1,    &signals_DP_Treshold1},
                               {FACTOR_CO_TRESHOLD1,    &signals_CO_Treshold1},
                               {FACTOR_VLAG_TRESHOLD1,  &signals_VLAG_Treshold1},
                               {FACTOR_HAND_TRESHOLD1,  &signals_HAND_Treshold1},
                               {FACTOR_GOTV_TRESHOLD1,  &signals_HLADON_Treshold1},

//                               {FACTOR_f_TRESHOLD2,     &signals_f_Treshold2},  // сигналы перенесены в signals_HLADON_Treshold2
                               {FACTOR_T_TRESHOLD2,     &signals_T_Treshold2},
                               {FACTOR_VLAG_TRESHOLD2,  &signals_VLAG_Treshold2},
                               {FACTOR_GOTV_TRESHOLD2,  &signals_HLADON_Treshold2}};


signals_table SIGNALS_Z_TABLE = {{FACTOR_T_TRESHOLD1,     &signals_T_Treshold1_Z},
                                 {FACTOR_dT_TRESHOLD1,    &signals_dT_Treshold1_Z},
                                 {FACTOR_DP_TRESHOLD1,    &signals_DP_Treshold1_Z},
                                 {FACTOR_HAND_TRESHOLD1,  &signals_HAND_Treshold1_Z},
                                 {FACTOR_T_TRESHOLD2,     &signals_T_Treshold2_Z}};



///** ******  ТИП ДАННЫХ 0x19  ********* **/
#define FI20_SS1_119    "FI20_SS1_119"
#define FI20_SS1_122    "FI20_SS1_122"
#define FI82_12SS1_218  "FI82_12SS1_218"
#define FI82_12SS1_8    "FI82_12SS1_8"
#define FI82_12SS1_38   "FI82_12SS1_38"
#define FI82_12SS1_67   "FI82_12SS1_67"
#define FI82_12SS1_93   "FI82_12SS1_93"
#define FI82_12SS1_123  "FI82_12SS1_123"
#define FI82_12SS1_150  "FI82_12SS1_150"
 


///** ******  ТИП ДАННЫХ 0x03  ********* **/
const QMap<quint16, QString> SIGNALS_BY_ROOMS = {{1,  "FI20_MALJAR_KLAD_001"},          {2,  "FI20_TAKEL_KLAD_002"},            {3,  "FI20_FORPIK_003"},
                                                 {4,  "FI20_SHKIP_KLAD_004"},           {5,  "FI20_KLAD_SUH_PR_005"},           {6,  "FI20_POM_MOROZ_006"},
                                                 {7,  "FI20_POM_HOLOD_007"},            {8,  "FI20_POM_SHHITOV_2_008"},         {10,  "FI20_POST_PMO_2_009"},
                                                 {11,  "FI20_TAMBUR_4_010"},            {12,  "FI20_TAMBUR_5_011"},             {13,  "FI20_AGREGAT_PUS_012"},
                                                 {14,  "FI20_KLAD_ZIP_1_013"},          {15,  "FI20_POM_MEHAN_014"},            {16,  "FI20_POM_TR_PU_015"},
                                                 {17,  "FI20_TAMBUR_1_016"},            {18,  "FI20_ASU_DIEZ_017"},             {19,  "FI20_GALUN_MICHM_018"},
                                                 {20,  "FI20_KAJUTA_MICHM_1_019"},      {21,  "FI20_KAJUTA_MICHM_2_020"},       {22,  "FI20_KORIDOR_1_021"},
                                                 {23,  "FI20_KORIDOR_2_022"},           {24,  "FI20_POM_SHHITOV_3_023"},        {26,  "FI20_POSUDOMOECH_024"},
                                                 {27,  "FI20_RAZD_MICHMAN_025"},        {28,  "FI20_RUBKA_GIDROAK_026"},        {29,  "FI20_SPEC_ARHIV_027"},
                                                 {30,  "FI20_STOLOVAJA_028"},           {31,  "FI20_TAMBUR_6_029"},             {32,  "FI20_TAMBUR_7_030"},
                                                 {33,  "FI20_TAMBUR_8_031"},            {34,  "FI20_TAMBUR_9_032"},             {35,  "FI20_TP_033"},
                                                 {36,  "FI20_KUBRIK_1_034"},            {37,  "FI20_KUBRIK_2_035"},             {38,  "FI20_POM_SHHITOV_1_036"},
                                                 {40,  "FI20_SANUZEL_1_037"},           {41,  "FI20_SANUZEL_2_038"},            {42,  "FI20_TAMBUR_2_039"},
                                                 {43,  "FI20_SHAHTA_GAS_1_040"},        {44,  "FI20_AGREG_POU_GAS_041"},        {45,  "FI20_APPARAT_GIDROAK_042"},
                                                 {46,  "FI20_GIROPOST_043"},            {47,  "FI20_P_OBOR_SIS_PRES_044"},      {48,  "FI20_POM_SBORA_STOK_045"},
                                                 {49,  "FI20_HIMICH_KLAD_046"},         {50,  "FI20_KAMBUZ_047"},               {51,  "FI20_KLAD_ZIP_2_048"},
                                                 {52,  "FI20_KORIDOR_3_049"},           {53,  "FI20_KPEZH_050"},
                             /* повторяющиеся */ {54,  "FI20_POM_IZD_KDS703_051"},      {55,  "FI20_POM_IZD_KDS703_051"},
                             /* повторяющиеся */ {56,  "FI20_POM_GRSHH_1_052"},         {57,  "FI20_POM_GRSHH_1_052"},          {58,  "FI20_POM_GRSHH_1_052"},
                                                 {59,  "FI20_RASH_KLAD_053"},           {60,  "FI20_TAMBUR_10_054"},            {61,  "FI20_TAMBUR_11_055"},
                             /* повторяющиеся */ {62,  "FI20_MO_056"},                  {63,  "FI20_MO_056"},
                                                 {64,  "FI20_AGREG_PMO_2_057"},         {65,  "FI20_KORIDOR_5_058"},
                             /* повторяющиеся */ {66,  "FI20_POM_SHHITOV_4_059"},       {67,  "FI20_POM_SHHITOV_4_059"},
                                                 {68,  "FI20_POST_PMO_1_060"},          {69,  "FI20_PRACHECH_061"},             {70,  "FI20_STAN_OHT_1_062"},             {71,  "FI20_SUSHILNAJA_063"},
                                                 {72,  "FI20_TAMBUR_16_064"},           {73,  "FI20_TAMBUR_SHLJUZ_15_065"},     {74,  "FI20_ELLING_BT_066"},
                                                 {75,  "FI20_VODOLAZ_KLAD_2_067"},      {76,  "FI20_POM_DEZHURNOGO_068"},       {77,  "FI20_TAMBUR_14_69"},
                                                 {78,  "FI20_ELLING_SPA_070"},          {79,  "FI20_VODOLAZ_KLAD_1_071"},       {80,  "FI20_POM_NAS_AGREG_072"},
                             /* повторяющиеся */ {81,  "FI20_POM_GRSHH_2_073"},         {82,  "FI20_POM_GRSHH_2_073"},          {83,  "FI20_POM_GRSHH_2_073"},
                                                 {84,  "FI20_TAMBUR_3_074"},
                             /* повторяющиеся */ {85,  "FI20_DGO_075"},                 {86,  "FI20_DGO_075"},
                                                 {87,  "FI20_KORIDOR_4_076"},           {88,  "FI20_POM_BP_RHR_077"},           {89,  "FI20_RAZDEV_KOMAND_078"},
                                                 {90,  "FI20_SANUZEL_3_079"},           {91,  "FI20_TAMBUR_12_80"},             {92,  "FI20_TAMBUR_13_81"},
                                                 {93,  "FI20_SHAHTA_GAZOH_1_082"},      {94,  "FI20_SHAHTA_GAZOH_2_083"},
                             /* повторяющиеся */ {95,  "FI20_OVM_084"},                 {96,  "FI20_OVM_084"},
                                                 {97,  "FI20_POM_PPR_085"},             {98,  "FI20_POM_OBOR_STOCH_SIS_086"},   {99,  "FI20_AGREGAT_PMO_1_087"},
                                                 {100,  "FI20_KLAD_ASI_088"},           {101,  "FI20_POM_GIDROAGR_089"},        {102,  "FI20_POM_NASOS_STAN_KRAN"},
                                                 {103,  "FI20_RUMP_OTDEL_090"},         {104,  "FI20_TRAL_KLAD_091"},           {105,  "FI20_KAJUTA_KOMAND_092"},
                                                 {106,  "FI20_KAJUTA_OFICEROV_1_093"},  {107,  "FI20_KAJUTA_OFICEROV_2_094"},   {108,  "FI20_KAJUTA_OFICEROV_3_095"},
                                                 {109,  "FI20_KAJUT_KOMP_096"},         {110,  "FI20_KORIDOR_6_097"},           {111,  "FI20_KORIDOR_7_098"},
                                                 {112,  "FI20_RADIORUBKA_099"},         {113,  "FI20_SANUZEL_KOMAND_100"},      {114,  "FI20_STAN_OHT_2_101"},
                                                 {115,  "FI20_TAMBUR_17_102"},
                             /* повторяющиеся */ {116,  "FI20_AKKUMUL_SUD_BAT_103"},    {117,  "FI20_AKKUMUL_SUD_BAT_103"},
                                                 {118,  "FI20_GKP_HKP_104"},            {119,  "FI20_KLAD_SIGN_SR_1_105"},
                             /* повторяющиеся */ {120,  "FI20_POM_ZAR_AGREG_106"},		{121,  "FI20_POM_ZAR_AGREG_106"},
                                                 {122,  "FI20_SHTURMAN_POST_107"},      {123,  "FI20_AGREGAT_RTS_108"},
                                                 {124,  "FI20_BUFETNAJA_109"},          {125,  "FI20_GALJUN_OFICEROV_110"},     {126,  "FI20_KORIDOR_8_111"},
                                                 {127,  "FI20_POM_KONDIC_FVU_112"},
                             /* повторяющиеся */ {128,  "FI20_POM_IZD_67_113"},         {129,  "FI20_POM_IZD_67_113"},
                             /* повторяющиеся */ {130,  "FI20_POM_SHHITOV_5_114"},      {131,  "FI20_POM_SHHITOV_5_114"},
                                                 {132,  "FI20_RAZDEV_OFICEROV_115"},	{133,  "FI20_SPS_116"},                 {134,  "FI20_TAMBUR_18_117"},
                                                 {135,  "FI20_TAMBUR_19_118"},          {136,  "FI20_KLAD_SIGN_SR_2_119"},      {137,  "FI20_POM_SBORA_TV_OTHOD_120"},
                                                 {138,  "FI20_SHAHTA_GAZOH_3_121"},
                             /* повторяющиеся */ {139,  "FI20_POGREB_MIN_TRAL_BZ_122"}, {140,  "FI20_POGREB_MIN_TRAL_BZ_122"},
                             /* повторяющиеся */ {141,  "FI20_POM_STRELKORUZ_123"},     {142,  "FI20_POM_STRELKORUZ_123"},
                             /* повторяющиеся */ {143,  "FI20_POM_BZ_STR_DP64_124"},    {144,  "FI20_POM_BZ_STR_DP64_124"},
                             /* повторяющиеся */ {145,  "FI20_PODBASH_OTD_125"},        {146,  "FI20_PODBASH_OTD_125"},
                             /* повторяющиеся */ {147,  "FI20_POM_BZ_AK306_126"},       {148,  "FI20_POM_BZ_AK306_126"},
                             /* повторяющиеся */ {149,  "FI20_POM_IZD_9K38_127"},		{150,  "FI20_POM_IZD_9K38_127"}};


/* все повторяющиеся */
const QMap<quint16, QString> SIGNALS_Z_BY_ROOMS = {{139,  "FI20_POGREB_MIN_TRAL_BZ_122_Z"}, {140,  "FI20_POGREB_MIN_TRAL_BZ_122_Z"},
                                                   {141,  "FI20_POM_STRELKORUZ_123_Z"},     {142,  "FI20_POM_STRELKORUZ_123_Z"},
                                                   {143,  "FI20_POM_BZ_STR_DP64_124_Z"},    {144,  "FI20_POM_BZ_STR_DP64_124_Z"},
                                                   {145,  "FI20_PODBASH_OTD_125_Z"},        {146,  "FI20_PODBASH_OTD_125_Z"},
                                                   {147,  "FI20_POM_BZ_AK306_126_Z"},       {148,  "FI20_POM_BZ_AK306_126_Z"},
                                                   {149,  "FI20_POM_IZD_9K38_127_Z"},       {150,  "FI20_POM_IZD_9K38_127_Z"}};



///** ******  ТИП ДАННЫХ 0x04  ********* **/
#define BI25_5SS1_VD1  "BI25_5SS1_VD1"
#define BI25_5SS1_VD2  "BI25_5SS1_VD2"
#define BI26_6SS1_VD1  "BI26_6SS1_VD1"

#define BI25_5SS1_VD1_IDX 366
#define BI25_5SS1_VD2_IDX 461
#define BI26_6SS1_VD1_IDX 373

#define CALC_BI25_5SS1_VD1(VAL)  ((VAL >> 0) & 1)
#define CALC_BI25_5SS1_VD2(VAL)  ((VAL >> 1) & 1)
#define CALC_BI26_6SS1_VD1(VAL)  ((VAL >> 2) & 1)

///****** СОСТОЯНИЕ ЛИНИИ **********///
#define POMP_SS1_8_STATUS   "POMP_SS1_8_STATUS"
#define POMP_SS1_38_STATUS  "POMP_SS1_38_STATUS"
#define POMP_SS1_67_STATUS  "POMP_SS1_67_STATUS"
#define POMP_SS1_93_STATUS  "POMP_SS1_93_STATUS"
#define POMP_SS1_119_STATUS "POMP_SS1_119_STATUS"
#define POMP_SS1_122_STATUS "POMP_SS1_122_STATUS"
#define POMP_SS1_123_STATUS "POMP_SS1_123_STATUS"
#define POMP_SS1_150_STATUS "POMP_SS1_150_STATUS"
#define POMP_SS1_218_STATUS "POMP_SS1_218_STATUS"

#endif // OPA_DEFS_H




