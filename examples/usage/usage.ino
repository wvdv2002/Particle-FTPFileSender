// Example usage for FTPFileSender library by wvdv2002.
#include "SdFat.h"
#include "FTPFileSender.h"

// Pick an SPI configuration.
// See SPI configuration section below (comments are for photon).
#define SPI_CONFIGURATION 3
//------------------------------------------------------------------------------
// Setup SPI configuration.
#if SPI_CONFIGURATION == 0
// Primary SPI with DMA
// SCK => A3, MISO => A4, MOSI => A5, SS => A2 (default)
SdFat sd;
const uint8_t chipSelect = SS;
#elif SPI_CONFIGURATION == 1
// Secondary SPI with DMA
// SCK => D4, MISO => D3, MOSI => D2, SS => D1
SdFat sd(1);
const uint8_t chipSelect = D1;
#elif SPI_CONFIGURATION == 2
// Primary SPI with Arduino SPI library style byte I/O.
// SCK => A3, MISO => A4, MOSI => A5, SS => A2 (default)
SdFatLibSpi sd;
const uint8_t chipSelect = SS;
#elif SPI_CONFIGURATION == 3
// Software SPI.  Use any digital pins.
// MISO => D5, MOSI => D6, SCK => D7, SS => D0
SdFatSoftSpi<D5, D6, D7> sd;
const uint8_t chipSelect = D0;
#endif  // SPI_CONFIGURATION
//------------------------------------------------------------------------------


FileSender fileSend("0.0.0.0","USERNAME","PASSWORD",21);

void setup() {
	
	Particle.function("sendFile",funcPostFile);
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
    Particle.publish("sysmessage","No SD card Found");
  }
}

void loop() {
  fileSend.task();
}

//Call this function using particle call "name" sendFile "/path/to/file" to send a file. It will create the same directory structure on the FTP server.
//Call this function using particle call "name" sendFile "status" to get the status string.
int funcPostFile(String command){
  if (command=="status"){
    Particle.publish("sysmessage",fileSend.toString());
    return 0;
  }
  return (int) fileSend.sendFile(command);
}