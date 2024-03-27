/*
 UDPSendReceiveString

 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and receive messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */


#include <Ethernet.h>
#include <EthernetUdp.h>
#include <TimerOne.h>

// Settings for Frequency Input control
#define FREQ_OUTPUT_PIN 2

unsigned long init_time_us = 145;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0xF0, 0xD2
};
IPAddress ip(169, 254, 94, 83);

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
// uint16_t packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet, 
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// The interrupt will blink the LED, and keep
// track of how many times it has blinked.
int ledState = LOW;
volatile int signalState = 0;
const int led = LED_BUILTIN;  // the pin with a LED
// volatile unsigned long blinkCount = 0; // use volatile for shared variables

void freq_control_callback(void)
{
  if (ledState == LOW) {
    ledState = HIGH;
    signalState = 1;
  } else {
    ledState = LOW;
    signalState = 0;
  }
  digitalWrite(led, ledState);
  digitalWrite(FREQ_OUTPUT_PIN, signalState);
}

double calculate_freq(const uint16_t crankshaft_speed_rpm, const uint8_t ppr) {
  Serial.println((double)crankshaft_speed_rpm * 60);
  double frequency = ( (double)crankshaft_speed_rpm * 60 ) / ppr;
  return frequency; // Hz
} 

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(1000000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // Setup Frequency Input control
  pinMode(FREQ_OUTPUT_PIN, OUTPUT);
  Timer1.initialize(init_time_us);
  Timer1.attachInterrupt(freq_control_callback);

  // start UDP
  Serial.println("UDP Initialized!");
  Udp.begin(localPort);
}

void loop() {
  // if there's data available, read a packet
  unsigned long t_start;
  unsigned long t_end;
  t_start = micros();
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    // IPAddress remote = Udp.remoteIP();
    // for (int i=0; i < 4; i++) {
    //   Serial.print(remote[i], DEC);
    //   if (i < 3) {
    //     Serial.print(".");
    //   }
    // }
    // Serial.print(", port ");
    // Serial.println(Udp.remotePort());

    // read the packet into packetBuffer
    byte packetBuffer[2];
    int available_packets = Udp.available();
    Udp.read(packetBuffer, 2);
    // Extract uint16_t value
    uint16_t crankshaftSpeed_val = (uint16_t)packetBuffer[1] << 8 | packetBuffer[0];
    t_end = micros();
    unsigned long t_elapsed = t_end - t_start;
    
  
    Serial.print("Contents: ");
    Serial.println(crankshaftSpeed_val);
    Serial.println(t_elapsed/1000);

    double signal_freq_hz = calculate_freq(crankshaftSpeed_val, 26);
    double signal_period_us = ( 1/signal_freq_hz ) * 1e6; // Convert to microseconds

    // The main reason why I need to half this is because when you're sending the 
    // Digital Output signals, you need to make sure that the 1s are hold 50% of the 
    // duration! So, if you want 1 Hz, then for 0.5s you hold 1s, and another 0.5s, you hold 0
    Timer1.setPeriod(signal_period_us/2); 

    Serial.print("Frequency: ");
    Serial.println(signal_freq_hz);
  
    // send a reply to the IP address and port that sent us the packet we received
    // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    // Udp.write(ReplyBuffer);
    // Udp.endPacket();
  }
  // delay(1);
}


/*
  Processing sketch to run with this example
 =====================================================

 // Processing UDP example to send and receive string data from Arduino
 // press any key to send the "Hello Arduino" message


 import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true ); 		// <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 String ip       = "192.168.1.177";	// the remote IP address
 int port        = 8888;		// the destination port

 udp.send("Hello World", ip, port );   // the message to send

 }

 void receive( byte[] data ) { 			// <-- default handler
 //void receive( byte[] data, String ip, int port ) {	// <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
 */
