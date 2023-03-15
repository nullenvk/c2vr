#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <hidapi/hidapi.h>

#include "ahrs.hpp"

class IMU {
    IMUDat lastdat;
    float sensor_bias[9] = {0};
    AHRS *ahrs;

    hid_device *dev_hndl;

    void readData();

public:
    IMU(hid_device *dev_hndl, unsigned int sampleFreq, AHRS *ahrs);

    bool readBiasFile(const char *filepath);

    void update();
    glm::quat getQuat();
};

class IMUThread {
    std::unique_ptr<AHRS> ahrs;
    std::unique_ptr<IMU> imu;
    std::unique_ptr<std::thread> thread;
    hid_device *dev_hndl; // TODO: Handle at exit

    std::mutex mutex;
    bool should_stop;

    glm::quat out_quat;

    void mainLoop();
public:
    bool start();
    void stop();

    glm::quat getQuat();
};
