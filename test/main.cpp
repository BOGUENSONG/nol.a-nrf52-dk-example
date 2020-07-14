#include <cox.h>

Timer tPrint;
Timer tPrint2;

uint8_t countNoInit __attribute__((section(".noinit")));
int count = 0;
int calc = 0;

//                      //
// Byte data function  //

uint8_t* snap(uint8_t format){
  static uint8_t abc[5];
  abc[0] = 0x00; // Type snap
  abc[1] = 0x01; // Length LSB
  abc[2] = 0x00; // Length LSB
  abc[3] = format; // format
  abc[4] = abc[0] + abc[1] + abc[2] + abc[3];
  return abc;
}




//                     //

static void receivemsg(SerialPort &p) {
  System.ledToggle(0);

  while (p.available() > 0){
    // uint8_t* buf;
    // p.readBytes(buf,17);
    // printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
    // printf("[0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]\r\n",buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]);
    char c = p.read();
    if (count < 8){
      printf("[0x%x]\r\n",c);
      count++;
      calc += c;
    }
    else{
      if (count < 16){
        printf("[%c]",c);
        count++;
        calc += c;
      }
      else{
        printf("checksum = %x\r\n",c);
        printf("calcchecksum = %x\r\n",calc);
        count = 0;
      }

    }

  }
}

static void eventButtonPressed(uint8_t n) {
  System.ledToggle(n);
  Serial.printf("[%lu usec] User button [%u] pressed\n", micros(), n);
}

static void eventButton1Pressed() {
  eventButtonPressed(0);
  printf("button 1 selected. send Snap\r\n");
  uint8_t* abc = snap(0x01);
  printf("[0x%x] [0x%x] [0x%x] [0x%x] [0x%x]\r\n",abc[0],abc[1],abc[2],abc[3],abc[4]);
  printf("send ok [%d]\r\n",Serial2.write(abc,5));
}

static void eventButton2Pressed() {
  eventButtonPressed(1);
}

static void eventButton3Pressed() {
  eventButtonPressed(2);
}

static void eventButton4Pressed() {
  reboot();
}




void setup() {
  System.ledOn(0);
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.printf("\n*** [nRF52-DK] Basic Functions ***\n");


  // Serial.listen();

  Serial2.listen();
  Serial2.onReceive(receivemsg);

  System.onButtonPressed(0, eventButton1Pressed);
  System.onButtonPressed(1, eventButton2Pressed);
  System.onButtonPressed(2, eventButton3Pressed);
  System.onButtonPressed(3, eventButton4Pressed);

  pinMode(3, INPUT);

}
