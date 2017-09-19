// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include <SD.h>
#include "mcp_can.h"


// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int LED=8;
boolean ledON=1;
const int chipSelect = 4;
int filenum = 1;
String filename = "1.txt";
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
File datafile;

void setup()
{
    Serial.begin(115200);
    pinMode(LED,OUTPUT);

START_SD:

    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      datafile.println("Card failed, or not present");
      // don't do anything more:
      goto START_SD;
    }
    Serial.println("card initialized.");

    while(SD.exists(filename))
    {
      filenum = filenum + 1;
      filename = String(filenum);
      filename.concat(".txt");
    }

    Serial.println(filename);
    
    datafile = SD.open(filename, FILE_WRITE);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_250KBPS))                   // init can bus : baudrate = 500k
    {
        datafile.println("CAN BUS Shield init ok!");
    }
    else
    {
        datafile.println("CAN BUS Shield init fail");
        datafile.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}


void loop()
{

START_LOOP:
    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned char canId = CAN.getCanId();
        String out = "0x";
        if (canId < 16) out += "0";
        out += String(canId, HEX);
        out += " : ";


        /*  CAN Address to be displayed in the serial monitor*/
        if (canId != 16) goto START_LOOP;
        
        for(int i = 0; i<len; i++)    // print the data
        {
            out += String(buf[i],HEX)+" ";
        }

        out += "time: ";
        out += millis();
        out += "\n";

        datafile = SD.open(filename, FILE_WRITE);
        datafile.print(out);
        datafile.close();
        
        Serial.print(out);
    }
}
 
