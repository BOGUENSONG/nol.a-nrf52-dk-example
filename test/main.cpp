#include <cox.h>
#include <stdlib.h>
// unknwon

Timer tPrint;
Timer tPrint2;

uint8_t countNoInit __attribute__((section(".noinit")));
//                      //
uint8_t flag = 0;
volatile int counter = 0;
//bLE setting
#include <BLEDevice.hpp>

#define SERVICE_UUID        "f64f0000-7fdf-4b2c-ad31-e65ca15bef6b"
#define CH_GETIMAGE_UUID "f64f0100-7fdf-4b2c-ad31-e65ca15bef6b"
#define CH_SNAP_UUID "f64f0200-7fdf-4b2c-ad31-e65ca15bef6b"
#define CH_SETRTC_UUID "f64f0300-7fdf-4b2c-ad31-e65ca15bef6b"

class getImageCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pChar){
    printf("read request on!\r\n");
  }
  void onWrite(BLECharacteristic *pChar) {
    std::string value = pChar->getValue();
    printf("write request on!\r\n");
    if (value.length() > 0) {
      printf("*******\n");
      printf("%s\n", value.c_str());
      printf("*******\n");
    }
  };
} getImageCB;

class snapCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pChar){
    printf("read request on!\r\n");
  }
  // void onWrite(BLECharacteristic *pChar) {
  //   std::string value = pChar->getValue();
  //   printf("write request on!\r\n");
  //   if (value.length() > 0) {
  //     printf("*******\n");
  //     printf("%s\n", value.c_str());
  //     printf("*******\n");
  //   }
  // };
} snapCB;
class setRTCCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pChar){
    printf("read request on!\r\n");
  }
  void onWrite(BLECharacteristic *pChar) {
    std::string value = pChar->getValue();
    printf("write request on!\r\n");
    if (value.length() > 0) {
      printf("*******\n");
      printf("%s\n", value.c_str());
      printf("*******\n");
    }
  };
} setRTCCB;


// Byte data function  //
uint8_t* int_tobyte(int value){
  static uint8_t temp[2];
  uint8_t *p = (uint8_t *)&value;
  printf("value = 0x%x, byte list = ", value);
  for (int i = 0 ; i < 2; i++){
    temp[i] =  *(p+i);
  }
  return temp;
}

uint8_t* snap(uint8_t format){
  static uint8_t abc[5];
  abc[0] = 0x00; // Type snap
  abc[1] = 0x01; // Length LSB
  abc[2] = 0x00; // Length LSB
  abc[3] = format; // format
  abc[4] = abc[0] + abc[1] + abc[2] + abc[3];
  return abc;
}

uint8_t* getImage(char* filename, int filesize, uint32_t offset, uint16_t chunk_length){
  // type(1) length(2) offset(4) chunkLen(2) fnsize(1) fname(variable) checksum(1)
  uint8_t type = 0x02; // getImage Type

  uint8_t fn_size = strlen(filename);
  uint8_t checksum = 0;

  int size = 11 + fn_size;
  uint16_t length = size-4; // total size - checksum size(1) - type size(1) - lengthsize(2)
  uint8_t *imgFormat = new uint8_t[size];
  imgFormat[0] = type;

  imgFormat[1] = 0xff & (length);
  imgFormat[2] = 0xff & (length >> 8);

  imgFormat[3] = 0xff & offset;
  imgFormat[4] = 0xff & (offset >> 8);
  imgFormat[5] = 0xff & (offset >> 16);
  imgFormat[6] = 0xff & (offset >> 24);

  imgFormat[7] = 0xff & (chunk_length);
  imgFormat[8] = 0xff & (chunk_length >> 8);

  imgFormat[9] = fn_size;
  for (int i = 0 ; i < fn_size; i++){
    imgFormat[10 + i] = filename[i];
  }

  for (int j = 0 ; j < size; j++){
    checksum = checksum + imgFormat[j];
  }
  imgFormat[10 + fn_size] = checksum;
  return imgFormat;

}

uint8_t* setRTC(uint16_t year , uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
  static uint8_t rtcmsg[11];
  uint8_t* yeartemp = int_tobyte(year);
  rtcmsg[0] = 0x04;
  rtcmsg[1] = 0x07;
  rtcmsg[2] = 0x00;
  rtcmsg[3] = yeartemp[0];
  rtcmsg[4] = yeartemp[1];
  rtcmsg[5] = month;
  rtcmsg[6] = day;
  rtcmsg[7] = hour;
  rtcmsg[8] = minute;
  rtcmsg[9] = second;
  rtcmsg[10] = 0;
  for (int i = 0; i <10 ; i++){
    rtcmsg[10] += rtcmsg[i];
  }
  return rtcmsg;
}

//                     //

static void receivemsg(SerialPort &p) {
  while (p.available() > 0){
    p.read();
    counter += 1;
  }
  printf("[%d]\r\n",counter);
}

static void eventButtonPressed(uint8_t n) {
  System.ledToggle(n);
  Serial.printf("[%lu usec] User button [%u] pressed\n", micros(), n);
}

static void eventButton1Pressed() {
  eventButtonPressed(0);
  printf("button 1 selected. send Snap\r\n");
  uint8_t* abc = snap(0x01);
  // printf("[0x%x] [0x%x] [0x%x] [0x%x] [0x%x]\r\n",abc[0],abc[1],abc[2],abc[3],abc[4]);
  printf("send ok [%d]\r\n",Serial2.write(abc,5));
  uint8_t *buffer = NULL;
  int num = Serial2.readBytes(buffer,0xff);
  printf("num = [%d]\r\n",num);
}

static void eventButton2Pressed() {
  eventButtonPressed(1);
  uint8_t* rtc = setRTC(2020,7,14,11,10,12);
  printf("send ok [%d]\r\n",Serial2.write(rtc,11));
  uint8_t *buffer = NULL;
  int num = Serial2.readBytes(buffer,0xff);
  printf("num = [%d]\r\n",num);

}

static void eventButton3Pressed() {
  eventButtonPressed(2);
  printf("button 1 selected. send getImage\r\n");
  char* fname = "0001.jpg";
  int msgsize = 11 + strlen(fname);
  uint8_t* imgchunk =  getImage(fname, 2048, 0x00, 1024);
  for (int i = 0 ; i < msgsize; i++){
    printf("[0x%x] , ",imgchunk[i]);
  }
  printf("\r\n");
  printf("send ok [%d]\r\n",Serial2.write(imgchunk,msgsize));
  delete[] imgchunk;
}

static void eventButton4Pressed() {
  int tempsize = 95;
  uint8_t abc[95];
  for (uint8_t i = 0 ; i < tempsize ; i++){
    abc[i] = i;
  }
  Serial2.write(abc,95);
}


void setup() {
  System.ledOn(0);
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.printf("\n*** [nRF52-DK] Serial & BLE GATT server ***\n");

  // BLE settings
  BLEDevice::init("My nRF52-DK");
  BLEServer *server = BLEDevice::createServer();
  printf("- Server created: %p\n", server);

  BLEService *service = server->createService(SERVICE_UUID);
  printf("- Service created\n");

  BLECharacteristic *ch_getimage = service->createCharacteristic(CH_GETIMAGE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE_NR);
  ch_getimage->setValue("get_image");
  ch_getimage->setCallbacks(&getImageCB);

  BLECharacteristic *ch_snap = service->createCharacteristic(CH_SNAP_UUID, BLECharacteristic::PROPERTY_READ);
  ch_snap->setValue("Snap");
  ch_snap->setCallbacks(&snapCB);

  BLECharacteristic *ch_setrtc = service->createCharacteristic(CH_SETRTC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE_NR);
  ch_setrtc->setValue("setRTC");
  ch_setrtc->setCallbacks(&setRTCCB);

  printf("- Characteristic created\n");
  service->start();
  printf("- Characteristic added\n");

// add advertising Data
// reffer => https://www.coxlab.kr/doxygen/Nol.A-SDK/classBLEAdvertisementData.html
//
  BLEAdvertisementData advdata;
  advdata.setName("Coxlab, Inc.");
  advdata.setManufacturerData("123456789");
  BLEAdvertising *adv = server->getAdvertising();

  adv->addServiceUUID(SERVICE_UUID);
  adv->setMinIntervalMicros(1000000); //시간 단위 설정 10^-6 까지 측정
  adv->setAdvertisementData(advdata);
  adv->start();
  printf("- Advertising started...\n");


  // Serial.listen();

  Serial2.listen();
  Serial2.onReceive(receivemsg);

  System.onButtonPressed(0, eventButton1Pressed);
  System.onButtonPressed(1, eventButton2Pressed);
  System.onButtonPressed(2, eventButton3Pressed);
  System.onButtonPressed(3, eventButton4Pressed);

  pinMode(3, INPUT);

}
