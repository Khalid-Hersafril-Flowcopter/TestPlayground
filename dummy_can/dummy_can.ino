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

    analogWrite(motor_1_pin, 0);
    analogWrite(motor_2_pin, 0);
    analogWrite(motor_3_pin, 0);
    analogWrite(motor_4_pin, 0);
}

float scale_fd_to_pwm(float Fd) {
  float pwm_val = Fd * 255;
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

    // Check if all data has been received
    if (allReceived) {
        // If all data is received, execute control

        // Serial.println();

        analogWrite(motor_1_pin, (uint8_t)(motorData[0].fd_value * 255));
        analogWrite(motor_2_pin, (uint8_t)(motorData[1].fd_value * 255));
        analogWrite(motor_3_pin, (uint8_t)(motorData[2].fd_value * 255));
        analogWrite(motor_4_pin, (uint8_t)(motorData[3].fd_value * 255));

        Serial.print((uint8_t)(motorData[0].fd_value * 255));
        Serial.print(" ");
        Serial.print((uint8_t)(motorData[1].fd_value * 255));
        Serial.print(" ");
        Serial.print((uint8_t)(motorData[2].fd_value * 255));
        Serial.print(" ");
        Serial.print((uint8_t)(motorData[3].fd_value * 255));
        Serial.println(" ");
        
        
        // Reset the received flags
        for (int i = 0; i < 4; i++) {
            motorData[i].received = false;
        }
    } else {
        // Continue to read CAN messages if not all data has been received
        if (CAN_MSGAVAIL == CAN.checkReceive()) {
            CAN.readMsgBuf(&len, buf);
            uint32_t curr_id = CAN.getCanId();
            int motor_index = -1;

            // Determine which motor should receive the data
            switch (curr_id) {
                case 1351:
                    motor_index = 0;
                    break;
                case 1352:
                    motor_index = 1;
                    break;
                case 1353:
                    motor_index = 2;
                    break;
                case 1354:
                    motor_index = 3;
                    break;
                default:
                    // If the ID is not relevant, ignore this message
                    break;
            }

            if (motor_index != -1) {
                motorData[motor_index].fd_value = convertBytesToFloat(buf, 8);
                motorData[motor_index].received = true;
            }
        }
    }
    // Optional: Output status for debugging
    // SERIAL_PORT_MONITOR.println("---LOOP END---");
}