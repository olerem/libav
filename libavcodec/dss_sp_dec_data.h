/*
 * dss2_decoder
 *
 *  Created on: 03.06.2014
 *      Autor: lex
 */

#ifndef DSS__
#define DSS__

#define SUBFRAMES		4
#define PULSE_MAX		8
#define DSS_CBUF_SIZE	21
#define DSS_SP_FRAME_SIZE 42
#define DSS_SP_SAMPLE_COUNT (66 * SUBFRAMES)

struct dss2_subframe {
	int16_t gain;
	int16_t field_2;
	int32_t combined_pulse_pos;
	int16_t pulse_pos[7]; /* this values was calculate */
	int16_t pulse_val[7]; /* this values was get directly from decompressor */
};

struct struc_1 {
	int16_t	field_0;
	int16_t	array14_stage0[14];

	int16_t subframe_something[SUBFRAMES];
	int16_t array_20[SUBFRAMES];

	struct dss2_subframe sf[SUBFRAMES];
};

struct struc_6 {
	int32_t array14_stage1[14];
	int32_t field_38;
};

struct struc_8 {
	int32_t field_0;
	int32_t array14_stage2[14]; // generally it is array14,
	// first field is used for some thing else and should be moved to filed_0
	int32_t field_38;
};

/*
 * Comment from: ./libavcodec/g723_1_data.h
 * Used for the coding/decoding of the pulses positions
 * for the MP-MLQ codebook
 */
static const uint32_t dss2_combinatorial_table[PULSE_MAX][72] = {
    {       0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0,
            0,         0,         0,          0,          0,          0 },
    {       0,         1,         2,          3,          4,          5,
            6,         7,         8,          9,         10,         11,
           12,        13,        14,         15,         16,         17,
           18,        19,        20,         21,         22,         23,
           24,        25,        26,         27,         28,         29,
           30,        31,        32,         33,         34,         35,
           36,        37,        38,         39,         40,         41,
           42,        43,        44,         45,         46,         47,
           48,        49,        50,         51,         52,         53,
           54,        55,        56,         57,         58,         59,
           60,        61,        62,         63,         64,         65,
           66,        67,        68,         69,         70,         71 },
    {       0,         0,         1,          3,          6,         10,
           15,        21,        28,         36,         45,         55,
           66,        78,        91,        105,        120,        136,
          153,       171,       190,        210,        231,        253,
          276,       300,       325,        351,        378,        406,
          435,       465,       496,        528,        561,        595,
          630,       666,       703,        741,        780,        820,
          861,       903,       946,        990,       1035,       1081,
         1128,      1176,      1225,       1275,       1326,       1378,
         1431,      1485,      1540,       1596,       1653,       1711,
         1770,      1830,      1891,       1953,       2016,       2080,
         2145,      2211,      2278,       2346,       2415,       2485 },
    {       0,         0,         0,          1,          4,         10,
           20,        35,        56,         84,        120,        165,
          220,       286,       364,        455,        560,        680,
          816,       969,      1140,       1330,       1540,       1771,
         2024,      2300,      2600,       2925,       3276,       3654,
         4060,      4495,      4960,       5456,       5984,       6545,
         7140,      7770,      8436,       9139,       9880,      10660,
        11480,     12341,     13244,      14190,      15180,      16215,
        17296,     18424,     19600,      20825,      22100,      23426,
        24804,     26235,     27720,      29260,      30856,      32509,
        34220,     35990,     37820,      39711,      41664,      43680,
        45760,     47905,     50116,      52394,      54740,      57155 },
    {       0,         0,         0,          0,          1,          5,
           15,        35,        70,        126,        210,        330,
          495,       715,      1001,       1365,       1820,       2380,
         3060,      3876,      4845,       5985,       7315,       8855,
        10626,     12650,     14950,      17550,      20475,      23751,
        27405,     31465,     35960,      40920,      46376,      52360,
        58905,     66045,     73815,      82251,      91390,     101270,
       111930,    123410,    135751,     148995,     163185,     178365,
       194580,    211876,    230300,     249900,     270725,     292825,
       316251,    341055,    367290,     395010,     424270,     455126,
       487635,    521855,    557845,     595665,     635376,     677040,
       720720,    766480,    814385,     864501,     916895,     971635 },
    {       0,         0,         0,          0,          0,          1,
            6,        21,        56,        126,        252,        462,
          792,      1287,      2002,       3003,       4368,       6188,
         8568,     11628,     15504,      20349,      26334,      33649,
        42504,     53130,     65780,      80730,      98280,     118755,
       142506,    169911,    201376,     237336,     278256,     324632,
       376992,    435897,    501942,     575757,     658008,     749398,
       850668,    962598,   1086008,    1221759,    1370754,    1533939,
      1712304,   1906884,   2118760,    2349060,    2598960,    2869685,
      3162510,   3478761,   3819816,    4187106,    4582116,    5006386,
      5461512,   5949147,   6471002,    7028847,    7624512,    8259888,
      8936928,   9657648,  10424128,   11238513,   12103014,   13019909 },
    {       0,         0,         0,          0,          0,          0,
            1,         7,        28,         84,        210,        462,
          924,      1716,      3003,       5005,       8008,      12376,
        18564,     27132,     38760,      54264,      74613,     100947,
       134596,    177100,    230230,     296010,     376740,     475020,
       593775,    736281,    906192,    1107568,    1344904,    1623160,
      1947792,   2324784,   2760681,    3262623,    3838380,    4496388,
      5245786,   6096454,   7059052,    8145060,    9366819,   10737573,
     12271512,  13983816,  15890700,   18009460,   20358520,   22957480,
     25827165,  28989675,  32468436,   36288252,   40475358,   45057474,
     50063860,  55525372,  61474519,   67945521,   74974368,   82598880,
     90858768,  99795696, 109453344,  119877472,  131115985,  143218999 },
    {       0,         0,         0,          0,          0,          0,
            0,         1,         8,         36,        120,        330,
          792,      1716,      3432,       6435,      11440,      19448,
        31824,     50388,     77520,     116280,     170544,     245157,
       346104,    480700,    657800,     888030,    1184040,    1560780,
      2035800,   2629575,   3365856,    4272048,    5379616,    6724520,
      8347680,  10295472,  12620256,   15380937,   18643560,   22481940,
     26978328,  32224114,  38320568,   45379620,   53524680,   62891499,
     73629072,  85900584,  99884400,  115775100,  133784560,  154143080,
    177100560, 202927725, 231917400,  264385836,  300674088,  341149446,
    386206920, 436270780, 491796152,  553270671,  621216192,  696190560,
    778789440, 869648208, 969443904, 1078897248, 1198774720, 1329890705 },
};

static const int16_t g_unc_array_3C84F0[14][32] = {
    { -32653, -32587, -32515, -32438, -32341, -32216, -32062, -31881,
      -31665, -31398, -31080, -30724, -30299, -29813, -29248, -28572,
      -27674, -26439, -24666, -22466, -19433, -16133, -12218,  -7783,
       -2834,   1819,   6544,  11260,  16050,  20220,  24774,  28120 },

    { -27503, -24509, -20644, -17496, -14187, -11277,  -8420,  -5595,
       -3013,   -624,   1711,   3880,   5844,   7774,   9739,  11592,
       13364,  14903,  16426,  17900,  19250,  20586,  21803,  23006,
       24142,  25249,  26275,  27300,  28359,  29249,  30118,  31183 },

    { -27827, -24208, -20943, -17781, -14843, -11848,  -9066,  -6297,
       -3660,   -910,   1918,   5025,   8223,  11649,  15086,  18423,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -17128, -11975,  -8270,  -5123,  -2296,    183,   2503,   4707,
        6798,   8945,  11045,  13239,  15528,  18248,  21115,  24785,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -21557, -17280, -14286, -11644,  -9268,  -7087,  -4939,  -2831,
        -691,   1407,   3536,   5721,   8125,  10677,  13721,  17731,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -15030, -10377,  -7034,  -4327,  -1900,    364,   2458,   4450,
        6422,   8374,  10374,  12486,  14714,  16997,  19626,  22954,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -16155, -12362,  -9698,  -7460,  -5258,  -3359,  -1547,    219,
        1916,   3599,   5299,   6994,   8963,  11226,  13716,  16982,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -14742,  -9848,  -6921,  -4648,  -2769,  -1065,    499,   2083,
        3633,   5219,   6857,   8580,  10410,  12672,  15561,  20101,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -11099,  -7014,  -3855,  -1025,   1680,   4544,   7807,  11932,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    {  -9060,  -4570,  -1381,   1419,   4034,   6728,   9865,  14149,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -12450,  -7985,  -4596,  -1734,    961,   3629,   6865,  11142,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -11831,  -7404,  -4010,  -1096,   1606,   4291,   7386,  11482,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -13404,  -9250,  -5995,  -3312,   -890,   1594,   4464,   8198,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },

    { -11239,  -7220,  -4040,  -1406,    971,   3321,   6006,   9697,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0,
           0,      0,      0,      0,      0,      0,      0,      0 },
};

static const int16_t  dss2_fixed_cb_gain[64] = {
       0,    4,    8,   13,   17,   22,   26,   31,
      35,   40,   44,   48,   53,   58,   63,   69,
      76,   83,   91,   99,  109,  119,  130,  142,
     155,  170,  185,  203,  222,  242,  265,  290,
     317,  346,  378,  414,  452,  494,  540,  591,
     646,  706,  771,  843,  922, 1007, 1101, 1204,
    1316, 1438, 1572, 1719, 1879, 2053, 2244, 2453,
    2682, 2931, 3204, 3502, 3828, 4184, 4574, 5000,
};

static const int16_t g_unc_array_3C8870[4] = {
    -8166, -2419, 1997, 6705
};

static const int16_t  dss2_pulse_val[8] = {
    -31182, -22273, -13364, -4455, 4455, 13364, 22273, 31182
};

static const int32_t binary_decreasing_array[] = {
		32767, 16384, 8192, 4096, 2048, 1024, 512, 256,
		128, 64, 32, 16, 8, 4, 2,
};

static const int32_t dss_sp_unc_decreasing_array[] = {
		32767, 26214, 20972, 16777, 13422, 10737, 8590, 6872,
		5498, 4398, 3518, 2815, 2252, 1801, 1441,
};

static const int16_t g_unc_array_3C88F8[] = {
		 102,  231,  360,  488,  617,  746,  875, 1004,
		1133, 1261, 1390, 1519, 1648, 1777, 1905, 2034,
		2163, 2292, 2421, 2550, 2678, 2807, 2936, 3065,
		3194, 3323, 3451, 3580, 3709, 3838, 3967, 4096,
};

static const int16_t g_unc_array_3C8938[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
		20, 22, 24, 26, 28, 30, 34, 40, 46, 52, 60, 72, 84,
};

static const int32_t g_unc_array_3C9288[67] = {
		  262,   293,   323,   348,   356,   336,   269,   139,
		  -67,  -358,  -733, -1178, -1668, -2162, -2607, -2940,
		-3090, -2986, -2562, -1760,  -541,  1110,  3187,  5651,
		 8435, 11446, 14568, 17670, 20611, 23251, 25460, 27125,
	                  	  28160, 28512, 28160,
		27125, 25460, 23251, 20611, 17670, 14568, 11446,  8435,
		 5651,  3187,  1110,  -541, -1760, -2562, -2986, -3090,
		-2940, -2607, -2162, -1668, -1178,  -733,  -358,   -67,
		  139,   269,   336,   356,   348,   323,   293,   262,
};

#endif /* DSS__ */
