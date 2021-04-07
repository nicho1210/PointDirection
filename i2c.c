#include <stdint.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <elf.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "i2c.h"


/*
 * delay:
 *	Wait for some number of milliseconds
 *********************************************************************************
 */

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}

/*
*
* CRC-8 formula from page 14 of SHT spec pdf
*
* Test data 0xBE, 0xEF should yield 0x92
*
* Initialization data 0xFF
* Polynomial 0x31 (x8 + x5 +x4 +1)
* Final XOR 0x00
*/
uint8_t crc8(const uint8_t *data, int len)
{
  const uint8_t POLYNOMIAL = 0x31;
  uint8_t crc = 0xFF;
  int j;
  int i;
  
  for (j = len; j; --j ) {
    crc ^= *data++;

    for ( i = 8; i; --i ) {
      crc = ( crc & 0x80 )
            ? (crc << 1) ^ POLYNOMIAL
            : (crc << 1);
    }
  }
  return crc;
}

/*
 * I2C_open:
 *	Open a file pointer to the appropiate I2C bus and i2c address
 *********************************************************************************
 */
int I2C_open(int i2c_address, uint8_t I2C_address)
{
  char filename[20];
  int fp;
  
  snprintf(filename, 19, "/dev/i2c-%d", i2c_address);
  fp = open(filename, O_RDWR);
  if (fp < 0) {
    return fp;
  }

  if (ioctl(fp, I2C_SLAVE, I2C_address) < 0) {
    close(fp);
    return -1;
  }
  
  
  return fp;
}

int I2C_close(int fp)
{
  return close(fp);
}
/*
void writeReg8 (int fp, uint8_t reg, uint8_t value)
{
  int sendsize = 1;
  
  write(fp, &reg, sendsize);
  write(fp, &value, sendsize);
  delay(10);

  return;
}
*/

void writeRegNoData (int fp, uint8_t reg)
{
  int sendsize = 1;
  uint8_t buffer;
  buffer = reg;
  
  write(fp, &buffer, sendsize);
  delay(10);

  return;
}

void readRegCnt (int fp, uint8_t *buf, uint8_t cnt)
{
  read(fp, buf, cnt);
  delay(10);
  
  return;
}


void writeReg8 (int fp, uint8_t reg, uint8_t value)
{
  int sendsize = 2;
  uint8_t buffer[2];
  buffer[0] = reg;
  buffer[1] = value;
  
  write(fp, &buffer, sendsize);
  delay(10);

  return;
}

uint8_t readReg8 (int fp, uint8_t reg)
{
  int sendsize = 1, readsize = 1;
  uint8_t buffer;
  write(fp, &reg, sendsize);

  delay(10);
  read(fp, &buffer, readsize);
  return buffer;
}

uint16_t readReg16 (int fp, uint8_t reg)
{
  int sendsize = 1, readsize = 2;
  uint8_t buffer[2] = {0};

  write(fp, &reg, sendsize);
  delay(10);
  read(fp, &buffer, readsize);
  
  return ((uint16_t)buffer[0] << 8 | buffer[1]);
}
