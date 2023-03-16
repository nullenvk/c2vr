#include "hid.h"
#include <hidapi/hidapi.h>
#include <cstring>
#include <cmath>

#include <ctime>

#define REFRESH_RATE 1000

const float IMUBIAS[9] = {
    -0.027233f, -0.008759f, 0.106667f,    
    //-0.025538f, -0.009340f, 0.090206f, // Gyro
    13.1271643136f, 4.2865617693999996f, 3.1704234511f, // Accel
    67.3205818979f, 37.3787205402f, -67.5572598736f, // Magnet
};

void CHIDHandler::thread_loop() {
    while(!b_shouldQuit) {
        uint8_t buf[37] = {0};

        if(hid_read_timeout(dev, buf, 36, HID_TIMEOUT) <= 0) continue;

        float tq[9];
        memcpy(tq, buf, sizeof(tq));

        for(size_t i = 0; i < 9; i++)
            tq[i] -= IMUBIAS[i];

        madg.update(tq[0], tq[1], tq[2],
                tq[3], tq[4], tq[5],
                tq[6], tq[7], tq[8]);

        vr::HmdQuaternion_t curq;
        curq.w = madg.q0;
        curq.x = madg.q1;
        curq.y = madg.q2;
        curq.z = madg.q3;

        // Just for debugging
        //float rot = fmod((float)clock() / (float)CLOCKS_PER_SEC, 3.14159f);
        //curq.w = cosf(rot/2.f);
        //curq.x = 1.0f * sinf(rot/2.f);
        //curq.y = 0.0f * sinf(rot/2.f);
        //curq.z = 0.0f * sinf(rot/2.f);

        //quat.x = (double)tq[0] / IMU_SCALE;
        //quat.y = (double)tq[1] / IMU_SCALE;
        //quat.z = (double)tq[2] / IMU_SCALE;
        //quat.w = (double)tq[3] / IMU_SCALE;

        // Critical section
        mutex.lock();

        quat = curq;

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

    madg.begin(REFRESH_RATE);
    madg.beta = 0.04f;

    uint8_t buf[37] = {0};

    hid_read(dev, buf, 36);

    float tq[9];
    memcpy(tq, buf, sizeof(tq));

    for(size_t i = 0; i < 9; i++)
        tq[i] -= IMUBIAS[i];

    for(size_t i = 0; i < 100000; i++) {
        madg.update(0, 0, 0,
                    tq[3], tq[4], tq[5],
                    tq[6], tq[7], tq[8]);
    }

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
