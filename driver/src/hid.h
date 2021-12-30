#pragma once
#include <string_view>
#include <thread>
#include <mutex>
#include <openvr/openvr_driver.h>
#include <hidapi/hidapi.h>

const int HID_TIMEOUT = 8000;
const double IMU_SCALE = 10000;

class CHIDHandler {
private:
    unsigned short vid, pid;

    std::unique_ptr<std::thread> thrd = nullptr;
    std::mutex mutex;
    vr::HmdQuaternion_t quat;

    bool b_shouldQuit = false;
    hid_device *dev;

    void thread_loop();
public:
    bool start(unsigned short vid, unsigned short pid);
    void stop();
    vr::HmdQuaternion_t getHMDQuat();
};
