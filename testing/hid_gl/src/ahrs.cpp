#include "ahrs.hpp"
#include <iostream>

void StupidAHRS::init(unsigned int sampleFreq) {
    this->sampleFreq = sampleFreq;
    this->estimate = glm::quat(1.0f, 0.f, 0.f, 0.f);
}

void StupidAHRS::update(IMUDat rawdat) {
    // Only integrates the gyroscope data
    
    glm::quat curRot = glm::quat(rawdat.gyro / ((float)sampleFreq));
    this->estimate = curRot * this->estimate; 
}

glm::quat StupidAHRS::getQuat() {
    return this->estimate;
}
