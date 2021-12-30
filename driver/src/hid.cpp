#include "hid.h"

void CHIDHandler::thread_loop() {
    while(!b_shouldQuit) {
        // TODO: Read HID data, lock the mutex, write quat, unlock mutex
    }
}

void CHIDHandler::start(std::string_view vid, std::string_view pid) {
    hmd_vid = vid.substr(0, 4);
    hmd_pid = pid.substr(0, 4);

    thrd.reset(new std::thread(&CHIDHandler::thread_loop, this));
}

void CHIDHandler::stop() {
    b_shouldQuit = true;
    thrd->join();
    thrd.reset(nullptr);
}

vr::HmdQuaternion_t CHIDHandler::getHMDQuat() {
    vr::HmdQuaternion_t q;

    q.x = 1;
    q.y = 0;
    q.z = 0;
    q.w = 0;

    // TODO

    return q;
}
