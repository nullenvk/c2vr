#include <HID.h>
#include <mpu9250.h>
#include <EEPROM.h>

#define QUAT_SCALE 10000.f
const int INIT_DELAY_MS = 200;

bfs::Mpu9250 mpu(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);

// Descriptor made by okawo80085
static const uint8_t USB_HID_Descriptor[] PROGMEM = {
    0x06, 0x03, 0x00, // USAGE_PAGE (vendor defined)
    0x09, 0x00,       // USAGE (Undefined)
    0xa1, 0x01,       // COLLECTION (Application)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
    0x85, 0x01,       //   REPORT_ID (1)
    0x75, 0x08,       //   REPORT_SIZE (16)

    0x95, 0x3f, //   REPORT_COUNT (1)

    0x09, 0x00, //   USAGE (Undefined)
    0x81, 0x02, //   INPUT (Data,Var,Abs) - to the host
    0xc0};

void catch_fire() {
    while(1) delay(5000);
}

void setup() {
    static HIDSubDescriptor node(USB_HID_Descriptor,
                                 sizeof(USB_HID_Descriptor));
    HID().AppendDescriptor(&node);

    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);
    
    if(!mpu.Begin()) {
        Serial.println("IMU INIT ERROR");
        catch_fire();
    }
    
    if(!mpu.ConfigGyroRange(bfs::Mpu9250::GYRO_RANGE_500DPS)) {
        Serial.println("IMU GYRO RANGE ERROR");
        catch_fire();
    }

    delay(INIT_DELAY_MS);

    Serial.println("OK");
}

void loop() {
    if(!mpu.Read())
        return;

    if(!mpu.new_imu_data())
        return;

    int32_t quat[4] = {
        mpu.gyro_x_radps() * QUAT_SCALE,
        mpu.gyro_y_radps() * QUAT_SCALE,
        mpu.gyro_z_radps() * QUAT_SCALE,
        0,
    };
        
    /*
    Serial.print(mpu.gyro_x_radps());
    Serial.print(" ");
    Serial.print(mpu.gyro_y_radps());
    Serial.print(" ");
    Serial.println(mpu.gyro_z_radps());
    */

    HID().SendReport(1, quat, 16);
}
