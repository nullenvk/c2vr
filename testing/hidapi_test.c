#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <stdint.h>

#define MAX_STR 255
#define QUAT_SCALE 1000.f

int main() {
    uint8_t raport_id;
    uint8_t buf[17] = {0};

    int res = hid_init();
    hid_device *dev_hndl = hid_open(0x1b4f, 0x9206, NULL);

    if(dev_hndl == NULL) {
        printf("Connection failed\n");
        exit(1);
    }

    buf[0] = 0x0;
    buf[1] = 0x81;
    res = hid_write(dev_hndl, (unsigned char*)buf, 17);

    while(1) {
        hid_read(dev_hndl, &raport_id, 1);
        hid_read(dev_hndl, (unsigned char*)buf, 16);

        float quat[4];
        memcpy(quat, buf, 16);

        for(int i = 0; i < 4; i++)
            //printf("%d ", buf[i]);
            printf("%f ", quat[i]);

        puts("");
    }

    hid_close(dev_hndl);
    res = hid_exit();
}
