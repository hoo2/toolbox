 /*

	Sensors.c - Is the sensors input resistance to temperature, lux etx translator
      API file, for various sensor types.
   includes:
      J-type Thermocouple, PT100, PT1000, NTC10k (B25/85: 3977K),
      PerkinElmer A9060 11 Photocell

   Copyright (C) 2011 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       8/2011
   Version:    0.1
 */

#include <Sensors.h>


/* 
 * =================================================================================   
 *                          Sensor Lookup Tables
 * =================================================================================
 */

//     J-Type Sensor mV LUT with the temperature correlation
//   ------------------------------------------------------------------------
static mVLUT_t const J_Type_mV[] =
{
  -7.890, -7.659, -7.403, -7.123, -6.821, -6.500, -6.159, -5.801, -5.426, -5.037,
  -4.633, -4.215, -3.786, -3.344, -2.893, -2.431, -1.961, -1.482, -0.955, -0.501,
   0.000,  0.507,  1.019,  1.537,  2.059,  2.585,  3.116,  3.65,   4.187,  4.726,
   5.269,  5.814,  6.360,  6.909,  7.459,  8.010,  8.562,  9.115,  9.669, 10.224,
  10.779, 11.334, 11.889, 12.445, 13.000, 13.555, 14.110, 14.655, 15.219, 15.773,
  16.327, 16.881, 17.434, 17.986, 18.538, 19.090, 19.642, 20.194, 20.745, 21.297,
  21.848, 22.400, 22.952, 23.504, 24.057, 24.610, 25.164, 25.720, 26.276, 26.834,
  27.393, 27.953, 28.516, 29.080, 29.647, 30.216, 30.788, 31.362, 31.939, 32.519,
  33.102, 33.689, 34.279, 34.873, 35.470, 36.071, 36.675, 37.284, 37.896, 38.512,
  39.132, 39.755, 40.382, 41.012, 41.645, 42.281
};

static TempLUT_t const J_Type_TEMP[] =
{
  -200,-190,-180,-170,-160,-150,-140,-130,-120,-110,
  -100, -90, -80, -70, -60, -50, -40, -30, -20, -10,  
     0,  10,  20,  30,  40,  50,  60,  70,  80,  90,
   100, 110, 120, 130, 140, 150, 160, 170, 180, 190,
   200, 210, 220, 230, 240, 250, 260, 270, 280, 290,
   300, 310, 320, 330, 340, 350, 360, 370, 380, 390,
   400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
   500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
   600, 610, 620, 630, 640, 650, 660, 670, 680, 690,
   700, 710, 720, 730, 740, 750, INT_MAX
};

//     PT100 and PT1000 Resistance LUT with the temperature correlation
//   ------------------------------------------------------------------------
static ResLUT_t const PT100x_RES[] =
{
   18.52, 22.83, 27.10, 31.33, 35.54, 39.72, 43.88, 48.00, 52.11, 56.19,            // -200 ~ -110 
   60.26, 64.30, 68.33, 72.33, 76.33, 80.31, 84.27, 88.22, 92.16, 96.09,            // -100 ~ -10
   100.00, 103.90, 107.79, 111.67, 115.54, 119.40, 123.24, 127.08, 130.90, 134.71,  //    0 ~ 90
   138.51, 142.29, 146.07, 149.83, 153.58, 157.33, 161.05, 164.77, 168.48, 172.17,  //  100 ~  190
   175.86, 179.53, 183.19, 186.84, 190.47, 194.10, 197.71, 201.31, 204.90, 208.48,  //  200 ~  290
   212.05, 215.61, 219.15, 222.69, 226.21, 229.72, 233.21, 236.70, 240.18, 243.64,  //  300 ~  390
   247.09, 250.53, 253.96, 257.38, 260.78, 264.18, 267.56, 270.93, 274.29, 277.64,  //  400 ~  490
   280.98, 284.30, 287.62, 290.92, 294.21, 297.49, 300.75, 304.01, 307.25, 310.49,  //  500 ~  590
   313.71
};
static TempLUT_t const PT100x_TEMP[]=
{
   -200, -190, -180, -170, -160, -150, -140, -130, -120, -110,
   -100,  -90,  -80,  -70,  -60,  -50,  -40,  -30,  -20,  -10,
      0,   10,   20,   30,   40,   50,   60,   70,   80,   90,
    100,  110,  120,  130,  140,  150,  160,  170,  180,  190,
    200,  210,  220,  230,  240,  250,  260,  270,  280,  290,
    300,  310,  320,  330,  340,  350,  360,  370,  380,  390,
    400,  410,  420,  430,  440,  450,  460,  470,  480,  490,
    500,  510,  520,  530,  540,  550,  560,  570,  580,  590,
    600, INT_MAX
};


//     KTY81-121 PTC Thermistor Resistance LUT and temperature correlation
//   ----------------------------------------------------------------------------
static ResLUT_t const KTY81_121_RES[] =
{
   485, 510, 562, 617, 677, 740, 807, 877, 951, 990,
   1029, 1111, 1196, 1286, 1378, 1475, 1575, 1679, 1786, 1896,
   2003, 2103, 2189, 0
};

//     KTY81-122 PTC Thermistor Resistance LUT and temperature correlation
//   ----------------------------------------------------------------------------
static ResLUT_t const KTY81_122_RES[] = 
{
   495, 520, 573, 630, 690, 755, 823, 895, 971, 1010,
   1050, 1134, 1221, 1312, 1406, 1505, 1607, 1713, 1823, 1934,
   2044, 2146, 2233, 0
};

//     KTY11-6 PTC Thermistor Resistance LUT and temperature correlation1
//   ----------------------------------------------------------------------------
static ResLUT_t const KTY11_6_RES[] =
{
   950, 1036, 1140, 1250, 1370, 1496, 1630, 1772, 1922, 2000,
   2080, 2245, 2418, 2599, 2788, 2984, 3188, 3400, 3620, 3847,
   4083, 4256, 4469, 0
};

static TempLUT_t const KTYx_x_TEMP[] =
{
   -55, -50, -40, -30, -20, -10,   0,  10,  20,  25,
    30,  40,  50,  60,  70,  80,  90, 100, 110, 120,
   130, 140, 150, INT_MAX
};

//      NTC 10k (B25/85 : 3997K) Resistance LUT and temperature correlation
//   ----------------------------------------------------------------------------
static ResLUT_t const NTC10k_RES[] = 
{
   332100, 240000, 175200, 129300, 96360, 72500, 55050, 42160, 32560, 25340,
    19870,  15700,  12490,  10000,  8059,  6535,  5330,  4372,  3606,  2989,
     2490,   2084,   1753,   1481,  1256,  1070, 915.4, 786.0, 677.3, 585.8,
    508.3,  442.6,  386.6,  338.7, 297.7, 262.4, 231.9, 205.5, 182.6
};
static TempLUT_t const NTC10k_TEMP[] = 
{
   -40, -35, -30, -25, -20, -15, -10,  -5,   0,   5,
    10,  15,  20,  25,  30,  35,  40,  45,  50,  55,
    60,  65,  70,  75,  80,  85,  90,  95, 100, 105,
   110, 115, 120, 125, 130, 135, 140, 145, 150, INT_MAX
};

//                      PerkiElmer Photocell A 906011
//   ----------------------------------------------------------------------------
static ResLUT_t const A906011_RES[] = 
{
   55000, 24000, 3500, 50
};
static LuxLUT_t const A906011_LUX[] = 
{
   1, 10, 100, 1000, INT_MAX
};



/* 
 * =================================================================================   
 *                                Static Functions
 * =================================================================================
 */

/*
   SensorValue_t
   Get_NC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T)

  * @brief  This function calculates the Sensor Value
            from the given measured value using a Negative
            coefficient LUT and a first order linear
            interpolation equation
                           v - F[i-1]
            T = T[i] -  ---------------- * (F[i] - F[i-1]) 
                          F[i] - F[i-1]
            example:
                      Rt - R10
            T = 10 - ---------- * (20-10)
                      R20 - R10
            Use this for NTC Thermistors etc..

  * @param  Measurement_t v  : The measured value
            FROM_RefLUT_t *V : The LUT Sensor FROM values
            TO_RefLUT_t *S   : The LUT Sensor TO values
  * @retval SensorValue_t    : Sensor return value     
 */
static SensorValue_t
Get_NC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T)
{
   int t = T[0], i = 0, j;

   //Find end point
   for (j=0 ; T[j]!= INT_MAX ; ++j);

   //Boundary cheking
   if (v < F[j-1] || v > F[0]) return TEMP_ERROR;

   //First match (for now)
   while (v <= F[i])
      t=T[i++];

   return (t + ((F[i-1] - v) / (F[i-1] - F[i])) * (T[i] - T[i-1]) );
}


/*
   SensorValue_t
   Get_PC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T)

  * @brief  This function calculates the Sensor Value
            from the given measured value using a Positive
            coefficient LUT and a first order linear
            interpolation equation
                           v - F[i-1]
            T = T[i] -  ---------------- * (F[i] - F[i-1]) 
                          F[i] - F[i-1]
            example:
                      Rt - R10
            T = 10 - ---------- * (20-10)
                      R20 - R10
            Use this for PT100, PT1000, PTC thermistors etc..

  * @param  Measurement_t v  : The measured value
            FROM_RefLUT_t *V : The LUT Sensor FROM values
            TO_RefLUT_t *S   : The LUT Sensor TO values
  * @retval SensorValue_t    : Sensor return value     
 */
static SensorValue_t
Get_PC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T)
{
   int i = 0, j, t =0;

   //Find end point
   for (j=0 ; T[j]!= INT_MAX ; ++j);

   //Boundary cheking
   if (v < F[0] || v > F[j-1])  return TEMP_ERROR;

   //First match (for now)
   while (v >= F[i])
      t=T[i++];

   return (t + ((v - F[i-1]) / (F[i] - F[i-1])) * (T[i] - T[i-1]) );
}


/* 
 * =================================================================================   
 *                      Temperature Sensor Exported Functions
 * =================================================================================
 */


Temp_t SENSOR_J_Type_mV2Temp (int Vadc, float g, int Vrefadc, float Vref)
{
   Temp_t t;
   float mV;
   
   mV = (1000.0 * ((Vadc - 0x0800) * Vref) / Vrefadc) / g;

   t = (Temp_t)Get_PC_SensorValue(mV , J_Type_mV, J_Type_TEMP);
   if (t != TEMP_ERROR)
      return t + Temp_GetIntTemp ();
   else
      return TEMP_ERROR;
}


/*
   Temp_t SENS_xxxxx_Res2Temp(Res_t r)

  * @brief  These functions calculate the temperature
            from the resistance of a sensor using
            Get_PC_SensorValue () or Get_NC_SensorValue ()
  * @param  Res_t r : The resistance in Ohm
  * @retval Temp_t Temperature in 'C
 */
Temp_t SEN_PT100 (Res_t r)
{
    return (Temp_t)Get_PC_SensorValue(r, PT100x_RES, PT100x_TEMP);
}

Temp_t SEN_PT1000 (Res_t r)
{
    return (Temp_t)Get_PC_SensorValue(r/10, PT100x_RES, PT100x_TEMP);
}

Temp_t SEN_KTY8x_121 (Res_t r)
{
    return (Temp_t)Get_PC_SensorValue(r, KTY81_121_RES, KTYx_x_TEMP);
}

Temp_t SEN_KTY8x_122 (Res_t r)
{
    return (Temp_t)Get_PC_SensorValue(r, KTY81_122_RES, KTYx_x_TEMP);
}

Temp_t SEN_KTY11_6 (Res_t r)
{
    return (Temp_t)Get_PC_SensorValue(r, KTY11_6_RES, KTYx_x_TEMP);
}

Temp_t SEN_NTC10k (Res_t r)
{
    return (Temp_t)Get_NC_SensorValue(r, NTC10k_RES, NTC10k_TEMP);
}

/* 
 * =================================================================================   
 *                      Light Sensor Exported Functions
 * =================================================================================
 */
/*
   Lux_t SENS_xxxxx_Res2Lux(Res_t r)

  * @brief  These functions calculate the temperature
            from the resistance of a sensor using
            TS_NTC_Res2Temp () or TS_PTC_Res2Temp ()
  * @param  Res_t r : The resistance in Ohm
  * @retval Temp_t Temperature in 'C
 */
Lux_t SENS_A906011_Res2Lux(Res_t r)
{
   return (Lux_t)Get_NC_SensorValue(r, A906011_RES, A906011_LUX);
}


