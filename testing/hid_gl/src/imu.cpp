#include "imu.hpp"
#include <fstream>
    
IMU::IMU(hid_device *dev_hndl, unsigned int sampleFreq, AHRS *ahrs) {
    this->dev_hndl = dev_hndl;
    this->ahrs = ahrs;
    this->ahrs->init(sampleFreq);
};

void IMU::readData() {
    float buf[9];

    hid_read(dev_hndl, (unsigned char*)buf, sizeof(buf));

    for(size_t i = 0; i < 9; i++)
        buf[i] -= sensor_bias[i];

    lastdat.gyro.x = buf[0];
    lastdat.gyro.y = buf[1];
    lastdat.gyro.z = buf[2];

    lastdat.acc.x = buf[3];
    lastdat.acc.y = buf[4];
    lastdat.acc.z = buf[5];
    
    lastdat.mag.x = buf[6];
    lastdat.mag.y = buf[7];
    lastdat.mag.z = buf[8];

}

bool IMU::readBiasFile(const char *filepath) {
    std::ifstream f(filepath);
    if(f.fail())
        return true;

    float tmp;
    for(size_t i = 0; i < 9; i++) {
        f >> tmp;
        sensor_bias[i] = tmp;
    }

    return false;
}

void IMU::update() {
    readData();
    ahrs->update(lastdat);
}

glm::quat IMU::getQuat() {
    return ahrs->getQuat();
}


bool IMUThread::start() {
    uint8_t hidbuf[17] = {0};

    if(thread.get() != nullptr)
        return true;

    should_stop = false;

    hid_init();
    dev_hndl = hid_open(0x1b4f, 0x9206, NULL);

    if(dev_hndl == NULL) {
        printf("Connection failed\n");
        return true;
    }
    
    hidbuf[0] = 0x0;
    hidbuf[1] = 0x81;
    hid_write(dev_hndl, (unsigned char*)hidbuf, 17);

    ahrs.reset(new StupidAHRS());
    imu.reset(new IMU(dev_hndl, 1000, ahrs.get()));

    imu->readBiasFile("bias.csv");

    thread.reset(new std::thread(&IMUThread::mainLoop, this));

    return false;
}

void IMUThread::stop() {
    if(thread.get() == nullptr)
        return;

    should_stop = true;

    thread->join();
    thread.reset(nullptr);
}

void IMUThread::mainLoop() {
    while(!should_stop) {
        imu->update();

        mutex.lock();
        out_quat = imu->getQuat();
        mutex.unlock();
    }
}

glm::quat IMUThread::getQuat() { 
    glm::quat ret;

    mutex.lock();
    ret = out_quat;
    mutex.unlock();

    return ret;
}
