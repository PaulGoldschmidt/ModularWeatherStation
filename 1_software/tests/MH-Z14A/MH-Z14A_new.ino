// On ESP8266:
// At 80MHz runs up 57600ps, and at 160MHz CPU frequency up to 115200bps with only negligible errors.
// Connect pin 12 to 14.

#include <SoftwareSerial.h>

#ifndef D5
#if defined(ESP8266)
#define D5 (14)
#define D6 (12)
#define D7 (13)
#define D8 (15)
#define TX (1)
#elif defined(ESP32)
#define D5 (18)
#define D6 (19)
#define D7 (23)
#define D8 (5)
#define TX (1)
#endif
#endif

byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};  // get gas command

char response[9];  // holds the recieved data
int CO2ppm = 1;

// Reminder: the buffer size optimizations here, in particular the isrBufSize that only accommodates
// a single 8N1 word, are on the basis that any char written to the loopback SoftwareSerial adapter gets read
// before another write is performed. Block writes with a size greater than 1 would usually fail.
SoftwareSerial swSer;

void setup() {
  Serial.begin(115200);
  swSer.begin(9600, SWSERIAL_8N1, D7, D8, false, 95, 11); // D5 D6 Default

  Serial.println("\nSoftware serial test started");
}

void loop() {
  getReadings();
  delay(5000);
}

void getReadings()
{
  Serial.println("Loop");
  while (swSer.available())  // this clears out any garbage in the RX buffer
  {
    int garbage = swSer.read();
  }

  swSer.write(cmd, 9);  // Sent out read command to the sensor
  swSer.flush();  // this pauses the sketch and waits for the TX buffer to send all its data to the sensor

  while (!swSer.available())  // this pauses the sketch and waiting for the sensor responce
  {
    delay(0);
  }

  swSer.readBytes(response, 9);  // once data is avilable, it reads it to a variable
  int responseHigh = (int)response[2];
  int responseLow = (int)response[3];
  CO2ppm = (256 * responseHigh) + responseLow;
  Serial.println("CO2: ");
  Serial.println(CO2ppm);
}
