#include <cox.h>

Timer tPrint;
Timer tPrint2;

static const char *weekday[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
uint8_t countNoInit __attribute__((section(".noinit")));

static void printTask(void *) {
  System.ledToggle(0);
  struct timeval t;
  gettimeofday(&t, nullptr);
  Serial.printf("[%lu.%06lu] Hi!\n", t.tv_sec, t.tv_usec);
  Serial.printf("* Systick: 0x%08lx\n", System.getSystickCount());
  Serial.printf("* Random number:%lu\n", random());
  Serial.printf("* System voltage: %ld mV\n", System.getSupplyVoltage());
  Serial.printf("* A0: %ld mV\n", map(analogRead(3), 0, 0x0FFF, 0, 3600));
  Serial.printf("* countNoInit:%lu\n", countNoInit++);

  struct tm t2;
  System.getDateTime(t2);
  Serial.printf(
    "* Now: %u-%u-%u %s %02u:%02u:%02u\n",
    t2.tm_year + 1900,
    t2.tm_mon + 1,
    t2.tm_mday,
    weekday[t2.tm_wday],
    t2.tm_hour,
    t2.tm_min,
    t2.tm_sec
  );
}

static void printTask2(void *) {
  Serial.printf("[%lu usec] Hi! 5-seconds periodic task\n", micros());
  System.ledToggle(1);
}

static void keyboard(SerialPort &p) {
  System.ledToggle(0);
  Serial.printf("[%lu usec] Keyboard input:", micros());
  while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  Serial.println();
}

static void eventButtonPressed(uint8_t n) {
  System.ledToggle(n);
  Serial.printf("[%lu usec] User button [%u] pressed\n", micros(), n);
}

static void eventButton1Pressed() {
  eventButtonPressed(0);
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

static void eventDateTimeAlarm() {
  struct tm t;
  System.getDateTime(t);
  printf(
    "* Alarm! Now: %u-%u-%u %s %02u:%02u:%02u\n",
    t.tm_year + 1900,
    t.tm_mon + 1,
    t.tm_mday,
    weekday[t.tm_wday],
    t.tm_hour,
    t.tm_min,
    t.tm_sec
  );
}

void setup() {
  System.ledOn(0);
  Serial.begin(115200);
  Serial.printf("\n*** [nRF52-DK] Basic Functions ***\n");

  // tPrint.onFired(printTask, NULL);
  // tPrint.startPeriodic(1000);
  //
  // tPrint2.onFired(printTask2, NULL);
  // tPrint2.startPeriodic(5000);

  Serial.listen();
  Serial.onReceive(keyboard);

  System.onButtonPressed(0, eventButton1Pressed);
  System.onButtonPressed(1, eventButton2Pressed);
  System.onButtonPressed(2, eventButton3Pressed);
  System.onButtonPressed(3, eventButton4Pressed);

  pinMode(3, INPUT);

  // System.onDateTimeAlarm(eventDateTimeAlarm);
  // System.setTimeAlarm(0, 1); // To alarm at 00:01:00.000000
}
