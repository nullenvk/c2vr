#pragma once
#include <string_view>
#include <thread>
#include <mutex>
#include <openvr/openvr_driver.h>

class CHIDHandler {
private:
    std::string hmd_vid, hmd_pid;

    std::unique_ptr<std::thread> thrd = nullptr;
    std::mutex mutex;
    vr::HmdQuaternion_t quat;

    bool b_shouldQuit = false;

    void thread_loop();
public:
    void start(std::string_view vid, std::string_view pid);
    void stop();
    vr::HmdQuaternion_t getHMDQuat();
};
