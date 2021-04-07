#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "i2c.h"
#include "minmea.h"

extern void Grove3Axis_setup(int fp);
extern void Grove3Axis_loop(int fp, float *xAxis);
extern int SetDirection (float CurDegree, float TarDegree);
extern int Neo_6M_setup(int *fd);
extern int Read_GGA (int fd, char *buffer);

#define BUF_SIZE    256
#define INPUT_SIZE  50

int CalTargetDegree (
	struct minmea_sentence_gga *frame,
	float TarLat,
	float TarLong,
	float *TarDegree
) {
	float lat1, long1, lat2, long2;
	float dy, dx, angle;
	int /*lati, longi,*/ degrees;
//test >
/*
	frame->latitude.value = 25066655;  //25.066655
	frame->latitude.scale = 100000;
	frame->longitude.value = 121535699;  //121.535699
	frame->longitude.scale = 100000;
	TarLat = 25.042348;
	TarLong = 121.523631;
*/
//test <

	// lat1, long1 is start point
	// lat2, long2 is end point
    lat1 = (float)(frame->latitude.value) / (frame->latitude.scale * 10);
	long1 = (float)(frame->longitude.value) / (frame->longitude.scale * 10);

	lat2 = TarLat;
	long2 = TarLong;

	printf ("lat1 = %f, long1 = %f\n", lat1, long1);
	printf ("lat2 = %f, long2 = %f\n", lat2, long2);

/*
 * Example:
 * const y = Math.sin(λ2-λ1) * Math.cos(φ2);
 * const x = Math.cos(φ1)*Math.sin(φ2) - Math.sin(φ1)*Math.cos(φ2)*Math.cos(λ2-λ1);
 * const θ = Math.atan2(y, x);
 * const brng = (θ*180/Math.PI + 360) % 360; // in degrees
 * */
	
	dy = sinf(long2 - long1) * cosf(lat2);
	dx = cosf(lat1) * sinf(lat2) - sinf(lat1) * cosf(lat2) * cosf (long2 - long1);
	angle = atan2f (dy, dx);
	degrees = (int)(angle * 180 / M_PI + 360) % 360;
	*TarDegree = angle * 180 / M_PI;

	printf ("dy = %f, dx = %f, angle = %f, degrees = %d targetDeg = %f", dy, dx, angle, degrees, *TarDegree);

	return 0;
}

int main (int argc, char *argv[]) {

	int file;
	float CurrentX = 0.0;
	float TargetX = 0.0;
	int fd = 0;
	char buffer[BUF_SIZE];
	char Latitude[INPUT_SIZE] = {0};
	char Longitude[INPUT_SIZE] = {0};
	float Lat = 0.0, Long = 0.0;
	struct minmea_sentence_gga frame;
	char DirNChar[] = "N", DirEChar[] = "E";
	int DirN = 0, DirE = 0;
	printf ("Direction\n");

	if (argc == 1) return 1;

    file = I2C_open(I2C_INTERFACE_ADDR, I2C_DEFAULT_ADDR);
	printf ("Direction1\n");

	if (!strcmp(argv[1], "gro")) {
		printf ("Enter Latitude: ");
		scanf ("%s", Latitude);
		printf ("\nDirection N/S: (N,S)");
		scanf ("%s", DirNChar);

		printf ("\nEnter Longitude: ");
		scanf ("%s", Longitude);
		printf ("\nDirection E/W: (E,W)");
		scanf ("%s", DirEChar);

		Lat = atof(Latitude);
		Long = atof(Longitude);
		DirN = (!strcmp(DirNChar,"N") || !strcmp(DirNChar,"n"))?1:-1;
		DirE = (!strcmp(DirEChar,"E") || !strcmp(DirEChar,"e") )?1:-1;

		Lat *= DirN;
		Long *= DirE;

		printf ("\nLat=%f Long=%f\n", Lat, Long);

		Grove3Axis_setup(file);
		Neo_6M_setup(&fd);

		while (1) {
			Grove3Axis_loop (file, &CurrentX);
			if (fd != 0) {
					printf ("GGA Before\n");
				Read_GGA (fd, buffer);
					printf ("GGA after\n");
				if (minmea_parse_gga(&frame, buffer)) {
					printf ("GGA: %s\n", buffer);
					CalTargetDegree(&frame, Lat, Long, &TargetX);
				} else {
		                    printf("Minmea Parse failed\n");
				}
			}
			SetDirection (CurrentX, TargetX);
		}
	}
    I2C_close(file);

	return 0;
}
