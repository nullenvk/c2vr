#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 255
#define QUAT_SCALE 10000.f

int main() {
    uint8_t buf[17] = {0};

    hid_init();
    hid_device *dev_hndl = hid_open(0x1b4f, 0x9206, NULL);

    if(dev_hndl == NULL) {
        printf("Connection failed\n");
        exit(1);
    }

    buf[0] = 0x0;
    buf[1] = 0x81;
    hid_write(dev_hndl, (unsigned char*)buf, 17);

    for(int t = 0;; t++) {
        hid_read(dev_hndl, (unsigned char*)buf, 17);

        int32_t quat[4];
        memcpy(quat, buf+1, 16);


        if(t >= 60) {
            for(int i = 0; i < 4; i++)
                printf("%f ", (float)quat[i] / QUAT_SCALE);
            puts("");

            t = 0;
        }
    }

    hid_close(dev_hndl);
    hid_exit();
}
