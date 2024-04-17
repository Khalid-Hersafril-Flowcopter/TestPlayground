// demo: CAN-BUS Shield, send data
// loovee@seeed.cc


#include <SPI.h>

#define CAN_2515
// #define CAN_2518FD

// Set SPI CS Pin according to your hardware

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;
#else

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;
#endif


#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif

unsigned char len = 0;
unsigned char buf[8];
unsigned long service_1_id = 6;

// Setup motor configuration
int motor_1_pin = 3;
int motor_2_pin = 5;
int motor_3_pin = 9;
int motor_4_pin = 10;

// Define a structure to hold motor control data
typedef struct {
    bool received;
    float fd_value;
} MotorControlData;

MotorControlData motorData[4]; // Array for each motor

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){};

    while (CAN_OK != CAN.begin(CAN_1000KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");

    pinMode(motor_1_pin, OUTPUT);
    pinMode(motor_2_pin, OUTPUT);
    pinMode(motor_3_pin, OUTPUT);
    pinMode(motor_4_pin, OUTPUT);
}

int scale_fd_to_pwm(double Fd) {
  int pwm_val = Fd * 255;
  return pwm_val;
}

// Function to convert the last 4 bytes of an array to a float
float convertBytesToFloat(unsigned char* arr, int size) {
    if (size < 4) {
        // Not enough data, return zero or NaN to indicate error
        return 0.0f; // or use NaN: return NAN;
    }
    
    // Union to help with byte to float conversion
    union {
        uint32_t i;
        float f;
    } u;

    // Pointer to the first of the last four elements
    unsigned char* ptr = arr + size - 4;

    // Copy bytes into the uint32_t part of the union
    u.i = ((uint32_t)ptr[0]) | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) | ((uint32_t)ptr[3] << 24);

    // Now the float part of the union holds the reconstructed float
    return u.f;
}

unsigned char get_pump_fd_byte(unsigned char *buf) {
  unsigned char *new_buf = buf + 4;
  return new_buf;
}

unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
void loop() {

    // Reset the reception flags at the start of each loop
    bool allReceived = true;
    for (int i = 0; i < 4; i++) {
        if (!motorData[i].received) {
            allReceived = false;
            break;
        }
    }

    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    // Indicator that arduino is alive
    // stmp[7] = stmp[7] + 1;
    // if (stmp[7] == 100) {
    //     stmp[7] = 0;
    //     stmp[6] = stmp[6] + 1;

    //     if (stmp[6] == 100) {
    //         stmp[6] = 0;
    //         stmp[5] = stmp[5] + 1;
    //     }
    // }

    // CAN.sendMsgBuf(0x00, 0, 8, stmp);
    // SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");

    // ---------------------
        
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
    // read data,  len: data length, buf: data buf
        // SERIAL_PORT_MONITOR.println("checkReceive");
        CAN.readMsgBuf(&len, buf);

        uint32_t curr_id = CAN.getCanId();

        if (curr_id == 1351 || curr_id == 1352 || curr_id == 1353 || curr_id == 1354) {
          Serial.println(curr_id);
          float fd_value = convertBytesToFloat(buf, 8);
          int pwm_value = scale_fd_to_pwm(fd_value); 

          switch (curr_id) {
            case 1351:
              analogWrite(motor_1_pin,  pwm_value);
              break;
            case 1352:
              analogWrite(motor_2_pin,  pwm_value);
              break;
            case 1353:
              analogWrite(motor_3_pin,  pwm_value);
              break;
            case 1354:
              analogWrite(motor_4_pin,  pwm_value);
              break;
          }
        }

    }
    // SERIAL_PORT_MONITOR.println("---LOOP END---");
}

// END FILE
