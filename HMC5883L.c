#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "HMC5883L.h"
#include "i2c.h"

float mgPerDigit;
Vector v;
int xOffset, yOffset;
float m_Scale;

bool hmc5883l_begin(int fp)
{
    if ((readReg8(fp, HMC5883L_REG_IDENT_A) != 0x48)
    || (readReg8(fp, HMC5883L_REG_IDENT_B) != 0x34)
    || (readReg8(fp, HMC5883L_REG_IDENT_C) != 0x33))
    {
	return false;
    }
    printf("- hmc5883l_begin -\n");

    setRange(fp, HMC5883L_RANGE_1_3GA);
    setMeasurementMode(fp, HMC5883L_CONTINOUS);
    setDataRate(fp, HMC5883L_DATARATE_15HZ);
    setSamples(fp, HMC5883L_SAMPLES_1);
    
    mgPerDigit = 0.92f;

    return true;
}

Vector readRaw(int fp)
{
    v.XAxis = readReg16(fp, HMC5883L_REG_OUT_X_M) - xOffset;
    v.YAxis = readReg16(fp, HMC5883L_REG_OUT_Y_M) - yOffset;
    v.ZAxis = readReg16(fp, HMC5883L_REG_OUT_Z_M);

    printf("readRawX %lf\n", v.XAxis);
    printf("readRawY %lf\n", v.YAxis);
    printf("readRawZ %lf\n", v.ZAxis);

    return v;
}

Vector readNormalize(int fp)
{
    v.XAxis = ((float)readReg16(fp, HMC5883L_REG_OUT_X_M) - xOffset) * mgPerDigit;
    v.YAxis = ((float)readReg16(fp, HMC5883L_REG_OUT_Y_M) - yOffset) * mgPerDigit;
    v.ZAxis = (float)readReg16(fp, HMC5883L_REG_OUT_Z_M) * mgPerDigit;

    printf("readNormalizeX %lf\n", v.XAxis);
    printf("readNormalizeY %lf\n", v.YAxis);
    printf("readNormalizeZ %lf\n", v.ZAxis);

    return v;
}

void setOffset(int xo, int yo)
{
    xOffset = xo;
    yOffset = yo;
}

uint16_t setScale(int fp, float gauss) {
    uint8_t regValue = 0x00;

    /*  Some of these values; e.g. 1.3 - cause comparison
        issues with the compiler that the Arduino IDE uses.
    */
#define CLOSEENOUGH(x,y) (fabs(x-y)<0.001)

    if (CLOSEENOUGH(gauss, 0.88)) {
        regValue = 0x00;
        m_Scale = 0.73;
    } else if (CLOSEENOUGH(gauss, 1.3)) {
        regValue = 0x01;
        m_Scale = 0.92;
    } else if (CLOSEENOUGH(gauss, 1.9)) {
        regValue = 0x02;
        m_Scale = 1.22;
    } else if (CLOSEENOUGH(gauss, 2.5)) {
        regValue = 0x03;
        m_Scale = 1.52;
    } else if (CLOSEENOUGH(gauss, 4.0)) {
        regValue = 0x04;
        m_Scale = 2.27;
    } else if (CLOSEENOUGH(gauss, 4.7)) {
        regValue = 0x05;
        m_Scale = 2.56;
    } else if (CLOSEENOUGH(gauss, 5.6)) {
        regValue = 0x06;
        m_Scale = 3.03;
    } else if (CLOSEENOUGH(gauss, 8.1)) {
        regValue = 0x07;
        m_Scale = 4.35;
    } else {
        return ERRORCODE_1_NUM;
    }

    // Setting is in the top 3 bits of the register.
    regValue = regValue << 5;
    writeReg8(fp, HMC5883L_REG_CONFIG_B, regValue);
    
    return 0;
}

void setRange(int fp, hmc5883l_range_t range)
{
    switch(range)
    {
	case HMC5883L_RANGE_0_88GA:
	    mgPerDigit = 0.073f;
	    break;

	case HMC5883L_RANGE_1_3GA:
	    mgPerDigit = 0.92f;
	    break;

	case HMC5883L_RANGE_1_9GA:
	    mgPerDigit = 1.22f;
	    break;

	case HMC5883L_RANGE_2_5GA:
	    mgPerDigit = 1.52f;
	    break;

	case HMC5883L_RANGE_4GA:
	    mgPerDigit = 2.27f;
	    break;

	case HMC5883L_RANGE_4_7GA:
	    mgPerDigit = 2.56f;
	    break;

	case HMC5883L_RANGE_5_6GA:
	    mgPerDigit = 3.03f;
	    break;

	case HMC5883L_RANGE_8_1GA:
	    mgPerDigit = 4.35f;
	    break;

	default:
	    break;
    }

    writeReg8(fp, HMC5883L_REG_CONFIG_B, range << 5);
}

hmc5883l_range_t getRange(int fp)
{
    return (hmc5883l_range_t)((readReg8(fp, HMC5883L_REG_CONFIG_B) >> 5));
}

void setMeasurementMode(int fp, hmc5883l_mode_t mode)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_MODE);
    value &= 0b11111100;
    value |= mode;

    writeReg8(fp, HMC5883L_REG_MODE, value);
}

hmc5883l_mode_t getMeasurementMode(int fp)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_MODE);
    value &= 0b00000011;

    return (hmc5883l_mode_t)value;
}

void setDataRate(int fp, hmc5883l_dataRate_t dataRate)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_CONFIG_A);
    value &= 0b11100011;
    value |= (dataRate << 2);

    writeReg8(fp, HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_dataRate_t getDataRate(int fp)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_CONFIG_A);
    value &= 0b00011100;
    value >>= 2;

    return (hmc5883l_dataRate_t)value;
}

void setPositiveNegative(int fp, uint8_t ms)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_CONFIG_A);
    value &= 0b11111100;
    value |= ms;

    writeReg8(fp, HMC5883L_REG_CONFIG_A, value);
}

uint8_t getPositiveNegative(int fp)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_CONFIG_A);
    value &= 0b00000011;
    return value;
}

void setSamples(int fp, hmc5883l_samples_t samples)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_CONFIG_A);
    value &= 0b10011111;
    value |= (samples << 5);

    writeReg8(fp, HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_samples_t getSamples(int fp)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_CONFIG_A);
    value &= 0b01100000;
    value >>= 5;

    return (hmc5883l_samples_t)value;
}

hmc5883l_status_t getStatus(int fp)
{
    uint8_t value;

    value = readReg8(fp, HMC5883L_REG_STATUS);
    value &= 0b00000011;

    return (hmc5883l_status_t)value;
}

void setToFirstDataOutput (int fp) {
    writeRegNoData (fp, HMC5883L_REG_OUT_X_M);
    return;
}

void getAllDataOutput (int fp, uint8_t *buf) {
    readRegCnt (fp, buf, HMC5883L_TOTAL_DATA);
    return;
}

void getDataXYZ (int fp, uint16_t *x, uint16_t *y, uint16_t *z) {
    
    uint8_t buf[6] = {0};
    
    readRegCnt (fp, buf, HMC5883L_TOTAL_DATA);
    *x = (buf[0] << 8) | buf[1];
    *z = (buf[2] << 8) | buf[3];
    *y = (buf[4] << 8) | buf[6];
    
    return;
}

MagnetometerRaw readRawAxis(int fp) {
    uint8_t buffer[HMC5883L_TOTAL_DATA] = {0};
    MagnetometerRaw raw = {0, 0, 0};
    setToFirstDataOutput (fp);
    getAllDataOutput (fp, buffer);
    raw.XAxis = (buffer[0] << 8) | buffer[1];
    raw.ZAxis = (buffer[2] << 8) | buffer[3];
    raw.YAxis = (buffer[4] << 8) | buffer[5];
    return raw;
}

MagnetometerScaled readScaledAxis(int fp) {
    MagnetometerRaw raw = {0, 0, 0};
    MagnetometerScaled scaled = {0, 0, 0};
    raw = readRawAxis(fp);
    scaled.XAxis = raw.XAxis * m_Scale;
    scaled.ZAxis = raw.ZAxis * m_Scale;
    scaled.YAxis = raw.YAxis * m_Scale;
    return scaled;
}

char* getErrorText(uint16_t errorCode) {
    if (ERRORCODE_1_NUM == 1) {
        return ERRORCODE_1;
    }
    return "Error not defined.";
}

