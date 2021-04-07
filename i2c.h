#ifndef _I2C_H_
#define _I2C_H_

#define I2C_INTERFACE_ADDR       1
#define I2C_DEFAULT_ADDR         0x1E

typedef enum
{
  I2C_OK = 0,
  I2C_CRC_CHECK_FAILED = 1,
  I2C_BAD = 2,
  I2C_READ_FAILED = 3, 
  I2C_WRITE_FAILED = 4
} i2crtn;

int I2C_open(int i2c_address, uint8_t I2C_address);
int I2C_close(int file);

void delay (unsigned int howLong);
uint8_t crc8(const uint8_t *data, int len);

void writeReg8 (int fp, uint8_t reg, uint8_t value);
uint8_t readReg8 (int fp, uint8_t reg);
uint16_t readReg16 (int fp, uint8_t reg);

void writeRegNoData (int fp, uint8_t reg);
void readRegCnt (int fp, uint8_t *buf, uint8_t cnt);

#endif /* _I2C_H_ */
