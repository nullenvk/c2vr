#include <MPU9250.h>
#include <HID.h>

const int INIT_DELAY_MS = 1000;
const int MPU_INIT_MS = 500;

// Descriptor made by okawo80085
static const uint8_t USB_HID_Descriptor[] PROGMEM = {
  0x06, 0x03, 0x00,   // USAGE_PAGE (vendor defined)
  0x09, 0x00,         // USAGE (Undefined)
  0xa1, 0x01,         // COLLECTION (Application)
  0x15, 0x00,         //   LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x00,   //   LOGICAL_MAXIMUM (255)
  0x85, 0x01,         //   REPORT_ID (1)
  0x75, 0x08,         //   REPORT_SIZE (16)

  0x95, 0x3f,         //   REPORT_COUNT (1)

  0x09, 0x00,         //   USAGE (Undefined)
  0x81, 0x02,         //   INPUT (Data,Var,Abs) - to the host
  0xc0
};

MPU9250 mpu;

void setup() {
  static HIDSubDescriptor node(USB_HID_Descriptor, sizeof(USB_HID_Descriptor));
  HID().AppendDescriptor(&node);
  
  Serial.begin(115200);
  Wire.begin();
  delay(INIT_DELAY_MS);

  mpu.setup(0x68);
  delay(MPU_INIT_MS);
  mpu.calibrateAccelGyro();
  mpu.calibrateMag();
}

void loop() {
  if(mpu.update()) {
    Serial.print(mpu.getYaw());
    Serial.print(", ");
    Serial.print(mpu.getPitch());
    Serial.print(", ");
    Serial.println(mpu.getRoll());

    float quat[4] = {
      mpu.getQuaternionX(),
      mpu.getQuaternionY(),
      mpu.getQuaternionZ(),
      mpu.getQuaternionW(),
    };

    HID().SendReport(1,quat,63);
  }
}
