CC = gcc

INC = -I ./include
LIB = -l wiringPi -lm
OBJ = HMC5883L.OBJ i2c.OBJ Grove3Axis.OBJ ServoMotor.OBJ Neo_6m.OBJ minmea.OBJ

# define any compile-time flags
CFLAGS = -Wall $(OBJ) -g -O

.PHONY: clean

all: PointDirection $(OBJ)

PointDirection : main.c $(OBJ)
	$(CC) -Wall $(OBJ) -o $@ $< $(LIB)

HMC5883L.OBJ: HMC5883L.c
	$(CC) -Wall -c -o $@ $< $(LIB)

i2c.OBJ: i2c.c
	$(CC) -Wall -c -o $@ $< $(LIB)

Grove3Axis.OBJ: Grove3Axis.c
	$(CC) -Wall -c -o $@ $< $(LIB)

ServoMotor.OBJ: ServoMotor.c
	$(CC) -Wall -c -o $@ $< $(LIB)

Neo_6m.OBJ: Neo_6m.c
	$(CC) -Wall -c -o $@ $< $(LIB)

minmea.OBJ: minmea.c
	$(CC) -Wall -c -o $@ $< $(LIB)

clean:
	rm -f PointDirection $(OBJ)
