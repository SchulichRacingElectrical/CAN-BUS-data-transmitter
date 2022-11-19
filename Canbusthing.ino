// CAN-BUS data transmitter
//Specifically for transmitting data from pressure sensor

#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>

//Variables
byte _status;
unsigned int P_dat;
unsigned int T_dat;
double P;
double PR;
double TR;
double V;
double VV;
float out1;


MCP_CAN CAN0(10);     // Set CS to pin 10

void setup() // Someone smarter than me did this bit up so cheers for that, whoever did this :)
{
  Serial.begin(115200);
  Wire.begin();

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}


void loop()
{
  PR = (double)((P_dat-819.15)/(14744.7)) ;
  PR = (PR - 0.49060678) ;
  PR = abs(PR);
  
  P = (double) P_dat*.0009155;
  V = ((PR*13789.5144)/1.225);
  VV = (sqrt((V)));

  TR = (double)((T_dat*0.09770395701));
  TR = TR-50;

  P = (double)(P_dat);
  P = (P-8090)/1350*6;

  out1 = -P/10*256;
 
  // Commenting this bit out because it isn't needed but you may want to use it for debugging purposes if things end up being wonky
  // Serial.print("raw Pressure:");  
  // Serial.println(P_dat);
  // Serial.print("pressure psi:");
  // Serial.println(P,10);
  // Serial.println(out1);

  byte data[1] = {P_dat}; 
  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send

  byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data); // CAN-BUS will transmit the P_dat, change the P-dat in the line above to something else if you want to transmit something else. You can see the available variables at the start of the loop function.

  if(sndStat == CAN_OK){ // Get rid of this bit too it you want, its just to check if the CAN-BUS is doing CAN-BUS stuff
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  delay(1000);   // send data per second
}

byte fetch_pressure(unsigned int *p_P_dat, unsigned int *p_T_dat) // Haven't the foggiest what's going on here, was taken from code of someone else on the team who likely knows what they're doing.
{
  byte address, Press_H, Press_L, _status;
  unsigned int P_dat;
  unsigned int T_dat;

  address = 0x28;
  Wire.beginTransmission(address);
  Wire.endTransmission();
  delay(100);

  Wire.requestFrom((int)address, (int) 4);//Request 4 bytes need 4 bytes are read
  Press_H = Wire.read();
  Press_L = Wire.read();
  byte Temp_H = Wire.read();
  byte  Temp_L = Wire.read();
  Wire.endTransmission();

  _status = (Press_H >> 6) & 0x03;
  Press_H = Press_H & 0x3f;
  P_dat = (((unsigned int)Press_H) << 8) | Press_L;
  *p_P_dat = P_dat;

  Temp_L = (Temp_L >> 5);
  T_dat = (((unsigned int)Temp_H) << 3) | Temp_L;
  *p_T_dat = T_dat;
  return (P_dat); // Did change this bit because I needed that P_dat

}

// End of file bye bye