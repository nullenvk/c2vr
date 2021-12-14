#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

#define MAX_STR 255

int main() {
    unsigned char buf[65] = {0};

    int res = hid_init();
    hid_device *dev_hndl = hid_open(0x1b4f, 0x9206, NULL);

    if(dev_hndl == NULL) {
        printf("Connection failed\n");
        exit(1);
    }

    buf[0] = 0x0;
    buf[1] = 0x81;
    res = hid_write(dev_hndl, buf, 65);

    while(1) {
        hid_read(dev_hndl, buf, 65);

        for(int i = 0; i < 4; i++) {
            float val = *((float*)(buf + sizeof(float) * i)) / 32767;
            printf("%f ", val);

        }
    }

    hid_close(dev_hndl);
    res = hid_exit();
}
