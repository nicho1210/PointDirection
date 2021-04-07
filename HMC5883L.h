/*
HMC5883L.h - Header file for the HMC5883L Triple Axis Digital Compass Arduino Library.

Version: 1.1.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HMC5883L_h
#define HMC5883L_h

#define PI M_PI

#define HMC5883L_ADDRESS              (0x1E)
#define HMC5883L_REG_CONFIG_A         (0x00)
#define HMC5883L_REG_CONFIG_B         (0x01)
#define HMC5883L_REG_MODE             (0x02)
#define HMC5883L_REG_OUT_X_M          (0x03)
#define HMC5883L_REG_OUT_X_L          (0x04)
#define HMC5883L_REG_OUT_Z_M          (0x05)
#define HMC5883L_REG_OUT_Z_L          (0x06)
#define HMC5883L_REG_OUT_Y_M          (0x07)
#define HMC5883L_REG_OUT_Y_L          (0x08)
#define HMC5883L_REG_STATUS           (0x09)
#define HMC5883L_REG_IDENT_A          (0x0A)
#define HMC5883L_REG_IDENT_B          (0x0B)
#define HMC5883L_REG_IDENT_C          (0x0C)

#define HMC5883L_TOTAL_DATA           6

#define ERRORCODE_1 "Entered scale was not valid, valid gauss values are: 0.88, 1.3, 1.9, 2.5, 4.0, 4.7, 5.6, 8.1"
#define ERRORCODE_1_NUM 1

typedef enum
{
    HMC5883L_SAMPLES_8     = 0b11,
    HMC5883L_SAMPLES_4     = 0b10,
    HMC5883L_SAMPLES_2     = 0b01,
    HMC5883L_SAMPLES_1     = 0b00
} hmc5883l_samples_t;

typedef enum
{
    HMC5883L_DATARATE_75HZ       = 0b110,
    HMC5883L_DATARATE_30HZ       = 0b101,
    HMC5883L_DATARATE_15HZ       = 0b100,
    HMC5883L_DATARATE_7_5HZ      = 0b011,
    HMC5883L_DATARATE_3HZ        = 0b010,
    HMC5883L_DATARATE_1_5HZ      = 0b001,
    HMC5883L_DATARATE_0_75_HZ    = 0b000
} hmc5883l_dataRate_t;

typedef enum
{
    HMC5883L_RANGE_8_1GA     = 0b111,
    HMC5883L_RANGE_5_6GA     = 0b110,
    HMC5883L_RANGE_4_7GA     = 0b101,
    HMC5883L_RANGE_4GA       = 0b100,
    HMC5883L_RANGE_2_5GA     = 0b011,
    HMC5883L_RANGE_1_9GA     = 0b010,
    HMC5883L_RANGE_1_3GA     = 0b001,
    HMC5883L_RANGE_0_88GA    = 0b000
} hmc5883l_range_t;

typedef enum
{
    HMC5883L_IDLE          = 0b10,
    HMC5883L_SINGLE        = 0b01,
    HMC5883L_CONTINOUS     = 0b00
} hmc5883l_mode_t;

typedef enum
{
    HMC5883L_LOCK          = 0b10,
    HMC5883L_RDY           = 0b01
} hmc5883l_status_t;

typedef enum
{
    HMC5883L_NORMAL_MC     = 0b00,
    HMC5883L_POSITIVE_MC   = 0b01,
    HMC5883L_NEGATIVE_MC   = 0b10
} hmc5883l_mc_t;

#ifndef VECTOR_STRUCT_H
#define VECTOR_STRUCT_H
typedef struct {
    float XAxis;
    float YAxis;
    float ZAxis;
} Vector;
#endif

typedef struct {
    float XAxis;
    float YAxis;
    float ZAxis;
} MagnetometerScaled;

typedef struct {
    int16_t XAxis;
    int16_t YAxis;
    int16_t ZAxis;
} MagnetometerRaw;

bool hmc5883l_begin(int fp);

Vector readRaw(int fp);
Vector readNormalize(int fp);

void  setOffset(int xo, int yo);
void  setRange(int fp, hmc5883l_range_t range);
hmc5883l_range_t getRange(int fp);

void  setMeasurementMode(int fp, hmc5883l_mode_t mode);
hmc5883l_mode_t getMeasurementMode(int fp);

void  setDataRate(int fp, hmc5883l_dataRate_t dataRate);
hmc5883l_dataRate_t getDataRate(int fp);

void  setSamples(int fp, hmc5883l_samples_t samples);
hmc5883l_samples_t getSamples(int fp);
hmc5883l_status_t getStatus(int fp);
void setPositiveNegative(int fp, uint8_t ms);
uint8_t getPositiveNegative(int fp);
void setToFirstDataOutput (int fp);
void getAllDataOutput (int fp, uint8_t *buf);
void getDataXYZ (int fp, uint16_t *x, uint16_t *y, uint16_t *z);

uint16_t setScale(int fp, float gauss);
char* getErrorText(uint16_t errorCode);
void compassCalibrate(int fp);
MagnetometerRaw readRawAxis(int fp);
MagnetometerScaled readScaledAxis(int fp);
#endif
