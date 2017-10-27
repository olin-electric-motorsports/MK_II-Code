// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include "mcp_can.h"


// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int LED=8;
boolean ledON=1;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(115200);
    pinMode(LED,OUTPUT);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_250KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}


void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned char canId = CAN.getCanId();
        
        Serial.print("CAN:");
        //Serial.print(canId, HEX);
        PrintHex8(canId);
        Serial.print(" MSG:");

        for(int i = 0; i<len; i++)    // print the data
        {
            //Serial.print(buf[i],HEX);
            PrintHex8(buf[i]);
            Serial.print(",");         
        }
        Serial.println();
    }
}

void PrintHex8(uint8_t data) // prints 8-bit data in hex with leading zeroes
{
       Serial.print("0x"); 
       if (data<0x10) {Serial.print("0");} 
       Serial.print(data,HEX);    
}

