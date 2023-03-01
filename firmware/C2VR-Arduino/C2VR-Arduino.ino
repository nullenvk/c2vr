#include <HID-Project.h>
#include <mpu9250.h>
#include <EEPROM.h>

#define BIGENDIAN 1

const int INIT_DELAY_MS = 200;

bfs::Mpu9250 mpu(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
uint8_t hidRecvBuf[255];

void catch_fire() {
    while(1) delay(5000);
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    RawHID.begin(hidRecvBuf, sizeof(hidRecvBuf));
    
    if(!mpu.Begin()) {
        Serial.println("IMU INIT ERROR");
        catch_fire();
    }

    // 4 SRD = 200 Hz Gyro
    // Otherwise 1000 Hz
    /*
    if(!mpu.ConfigSrd(4)) {
        Serial.println("IMU SRD ERROR");
        catch_fire();

    }*/

    delay(INIT_DELAY_MS);

    Serial.println("OK");
}

void convertBytes(float *in, uint32_t *out, size_t n) {
    uint32_t *inb = (uint32_t)in;
    for(size_t i = 0; i < n; i++)
        out[i] = BIGENDIAN ? __builtin_bswap32(inb[i]) : inb[i];
}

void loop() {
    if(!mpu.Read())
        return;

    if(!mpu.new_imu_data())
        return;

    float out[9] = {
        mpu.gyro_x_radps() ,
        mpu.gyro_y_radps() ,
        mpu.gyro_z_radps() ,

        mpu.accel_x_mps2() ,
        mpu.accel_y_mps2() ,
        mpu.accel_z_mps2() ,

        mpu.mag_x_ut() ,
        mpu.mag_y_ut() ,
        mpu.mag_z_ut() ,
    };

    RawHID.write((uint8_t*)out, sizeof(out));
}
