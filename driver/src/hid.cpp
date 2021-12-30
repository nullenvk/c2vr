#include "hid.h"
#include <hidapi/hidapi.h>
#include <cstring>

void CHIDHandler::thread_loop() {
    while(!b_shouldQuit) {
        uint8_t buf[17] = {0};
        
        // Request state 
        buf[0] = 0x0;
        buf[0] = 0x81;
        hid_write(dev, buf, 17);

        // Read requested state
        hid_read_timeout(dev, buf, 17, HID_TIMEOUT);

        int32_t tq[4];
        memcpy(tq, buf+1, 16);

        // Critical section
        mutex.lock();

        quat.x = (double)tq[0] / IMU_SCALE;
        quat.y = (double)tq[1] / IMU_SCALE;
        quat.z = (double)tq[2] / IMU_SCALE;
        quat.w = (double)tq[3] / IMU_SCALE;

        // End of critical section
        mutex.unlock();
    }
}

bool CHIDHandler::start(unsigned short vid, unsigned short pid) {
    this->vid = vid;
    this->pid = pid;
    
    hid_init();
    dev = hid_open(vid, pid, nullptr);

    if(dev == nullptr)
        return true;

    thrd.reset(new std::thread(&CHIDHandler::thread_loop, this));

    return false;
}

void CHIDHandler::stop() {
    b_shouldQuit = true;
    thrd->join();
    
    hid_close(dev);
    hid_exit();

    thrd.reset(nullptr);
}

vr::HmdQuaternion_t CHIDHandler::getHMDQuat() {
    vr::HmdQuaternion_t r;
    mutex.lock(); 

    r = quat;

    mutex.unlock(); 
    return r;
}
