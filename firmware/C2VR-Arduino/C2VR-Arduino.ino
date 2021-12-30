#include <HID.h>
#include <MPU9250.h>
#include <EEPROM.h>

#define EEPROM_MPU_ADDR 0x0
#define QUAT_SCALE 10000.f
const int REFRESH_RATE_MS = 1000 / 120; // 120 HZ
const int INIT_DELAY_MS = 200;
const int CALIBRATION_DELAY_MS = 500;

//#define CALIBRATION

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

struct EEPROMData {
    float AccBias[3], GyroBias[3], MagBias[3], MagScale[3];
};

MPU9250 mpu;
MPU9250Setting mpu_settings;
EEPROMData eeprom_dat;

inline void mpu_read_eeprom() {
    EEPROM.get(EEPROM_MPU_ADDR, eeprom_dat);

    mpu.setAccBias(eeprom_dat.AccBias[0], eeprom_dat.AccBias[1], eeprom_dat.AccBias[2]);
    mpu.setGyroBias(eeprom_dat.GyroBias[0], eeprom_dat.GyroBias[1], eeprom_dat.GyroBias[2]);
    mpu.setMagBias(eeprom_dat.MagBias[0], eeprom_dat.MagBias[1], eeprom_dat.MagBias[2]);
    mpu.setMagScale(eeprom_dat.MagScale[0], eeprom_dat.MagScale[1], eeprom_dat.MagScale[2]);
}

inline void mpu_write_eeprom() {
    eeprom_dat.AccBias[0] = mpu.getAccBiasX();
    eeprom_dat.AccBias[1] = mpu.getAccBiasY();
    eeprom_dat.AccBias[2] = mpu.getAccBiasZ();
    
    eeprom_dat.GyroBias[0] = mpu.getGyroBiasX();
    eeprom_dat.GyroBias[1] = mpu.getGyroBiasY();
    eeprom_dat.GyroBias[2] = mpu.getGyroBiasZ();
   
    eeprom_dat.MagBias[0] = mpu.getMagBiasX();
    eeprom_dat.MagBias[1] = mpu.getMagBiasY();
    eeprom_dat.MagBias[2] = mpu.getMagBiasZ();
    
    eeprom_dat.MagScale[0] = mpu.getMagScaleX();
    eeprom_dat.MagScale[1] = mpu.getMagScaleY();
    eeprom_dat.MagScale[2] = mpu.getMagScaleZ();

    EEPROM.put(EEPROM_MPU_ADDR, eeprom_dat);
}

void setup() {
    static HIDSubDescriptor node(USB_HID_Descriptor,
                                 sizeof(USB_HID_Descriptor));
    HID().AppendDescriptor(&node);

    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    delay(INIT_DELAY_MS);

    mpu_settings.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
    mpu_settings.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
    mpu_settings.gyro_fchoice = 0x03;
    mpu_settings.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ;

    mpu.setup(0x68, mpu_settings);
    //mpu.setFilterIterations(10);
    mpu_read_eeprom();

#ifdef CALIBRATION
    Serial.println(
        "Calibrating gyroscope and accelerometer, MPU should stay in place");
    mpu.calibrateAccelGyro();
    delay(CALIBRATION_DELAY_MS);

    Serial.println(
        "Calibrating magnetometer, MPU should be rotated in 8-figure");
    mpu.calibrateMag();
    delay(CALIBRATION_DELAY_MS);

    mpu_write_eeprom();
#endif

    Serial.println("OK");
}

void loop() {
    if (mpu.update()) {
        int32_t quat[4] = {
            mpu.getQuaternionX() * QUAT_SCALE,
            mpu.getQuaternionY() * QUAT_SCALE,
            mpu.getQuaternionZ() * QUAT_SCALE,
            mpu.getQuaternionW() * QUAT_SCALE,
        };

        /*
        Serial.print(quat[0]);
        Serial.print(",");
        Serial.print(quat[1]);
        Serial.print(",");
        Serial.print(quat[2]);
        Serial.print(",");
        Serial.println(quat[3]);
        */

        HID().SendReport(1, quat, 16);

        delay(REFRESH_RATE_MS);
    }
}
