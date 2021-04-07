#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <wiringSerial.h>

#define BUF_SIZE    256
char CRLF[] = {0x0D,0x0A};

char GGA_STR[] = "$GPGGA";

int Neo_6M_setup(int *fd) {
    *fd = 0;
    if ((*fd = serialOpen("/dev/ttyS0", 9600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return 1 ;
    }
//    serialPuts(*fd, "uart send test, just by launcher");
    
    return 0;
}

int Read_GGA (int fd, char *buffer) {

    char *bufptr;
    char *startptr;
    int idx = 0;
    bool started = false;

    memset(buffer, 0, sizeof (char) * BUF_SIZE);
    bufptr = buffer;
    
    for (;;) {
        
        if (serialDataAvail(fd) > 0) {
            *bufptr = serialGetchar(fd);
            printf ("%c", *bufptr);
            bufptr++;
            idx++;
            
            if (idx == 6) {
                if (!strncmp ((char *)((int)bufptr - 6), GGA_STR, strlen(GGA_STR))) {
                    started = true;
                    startptr = (char *)((int)bufptr - 6);
                }
            }
            if (!strncmp ((char *)((int)bufptr - 2), CRLF, strlen(CRLF))) {
                *bufptr = 0;
                bufptr = buffer;
                idx = 0;
                if (started == true) {
                    break;
                }
            }
        }
//        break; //pryd - test
    }
    
    printf ("G = %s\n", startptr);

    return 0;
}

