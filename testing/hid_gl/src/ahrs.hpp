#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
    
struct IMUDat {
    glm::vec3 gyro, acc, mag;
}; 

class AHRS {
public:
    virtual void init(unsigned int sampleFreq) = 0;
    virtual void update(IMUDat rawIn) = 0;
    virtual glm::quat getQuat() = 0;
};

class StupidAHRS : public AHRS {
    unsigned int sampleFreq;
    glm::quat estimate;

public:
    void init(unsigned int sampleFreq);
    void update(IMUDat rawIn);
    glm::quat getQuat();
};

class MadgwickAHRS : public AHRS {
    unsigned int sampleFreq;
    float beta;

    glm::quat estimate;

public:
    void init(unsigned int sampleFreq);
    void update(IMUDat rawIn);

    void setBetaVal(float beta);
    glm::quat getQuat();
};

