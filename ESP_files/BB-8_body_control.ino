#include"driver\pcnt.h"
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define PCNT_TEST_UNIT_1 PCNT_UNIT_1
#define PCNT_INPUT_SIG_IO_1 33 // Pulse Input GPIO
#define PCNT_INPUT_CTRL_IO_1 32 // Control GPIO HIGH=count up, LOW=count down

#define PCNT_TEST_UNIT_2 PCNT_UNIT_2
#define PCNT_INPUT_SIG_IO_2 13 // Pulse Input GPIO
#define PCNT_INPUT_CTRL_IO_2 14 // Control GPIO HIGH=count up, LOW=count down

#define PCNT_TEST_UNIT_3 PCNT_UNIT_3
#define PCNT_INPUT_SIG_IO_3 35 // Pulse Input GPIO
#define PCNT_INPUT_CTRL_IO_3 34 // Control GPIO HIGH=count up, LOW=count down

int16_t count1 = 0;
int16_t count1_old = 0;

int16_t count2 = 0;
int16_t count2_old = 0;

int16_t count3 = 0;
int16_t count3_old = 0;

double prevErr1 = 0; // to store previous value of the error (in closed loop)
double prevU1 = 0;
double prevErr2 = 0;
double prevU2 = 0;
double prevErr3 = 0;
double prevU3 = 0;

char auth[] = "4191ba0acfbb4d3583f420c3e";
char ssid[] = "J.A.R.V.I.S.";
char pass[] = "tonystarkdiesinendgame";
int x;
int y;

//Motor A
int motor3pin1 = 16;
int motor3pin2 = 17;
int en3 = 4;

//Motor B
int motor1pin1 = 26;
int motor1pin2 = 25;
int en1 = 27;

//Motor C
int motor2pin1 = 18;
int motor2pin2 = 19;
int en2 = 5;

volatile int interruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR( & timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR( & timerMux);

}

// Setting PWM properties
const int freq = 1000;
const int pwmCh1 = 1;
const int pwmCh2 = 2;
const int pwmCh3 = 3;
const int resolution = 8;
int dutyCycle = 0;

int led = 2;
double R = 0.15;
double f1 = 0.52359878; //0.52359878 rad = 30 degrees
double f2 = 2.61799388; //2.61799388 rad = 150 degrees 
double f3 = 4.71238898; //4.71238898 rad = 270 degrees 
double v1 = 0.0;
double v2 = 0.0;
double v3 = 0.0;
void calc_speeds(double * v1, double * v2, double * v3, int x, int y, int w) {
  * v1 = -sin(f1) * x + cos(f1) * y + R * w;
  * v2 = -sin(f2) * x + cos(f2) * y + R * w;
  * v3 = -sin(f3) * x + R * w;

}

uint8_t calc_dir(double v) {

  if (v <= 0)
    return LOW;

  if (v > 0)
    return HIGH;

}

BLYNK_WRITE(V0) {
  dutyCycle = param.asInt();
}

BLYNK_WRITE(V1) {
  x = param[0].asInt();
  y = param[1].asInt();

  calc_speeds( & v1, & v2, & v3, x, y, 0);

}

void setup() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(motor3pin1, OUTPUT);
  pinMode(motor3pin2, OUTPUT);
  pinMode(en3, OUTPUT);
  pinMode(led, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmCh1, freq, resolution);
  ledcSetup(pwmCh2, freq, resolution);
  ledcSetup(pwmCh3, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(en1, pwmCh1);
  ledcAttachPin(en2, pwmCh2);
  ledcAttachPin(en3, pwmCh3);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, & onTimer, true);
  timerAlarmWrite(timer, 20000, true);
  timerAlarmEnable(timer);

  Encoder_config();

  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

}

void loop() {
  Blynk.run();

  if (interruptCounter > 0) {

    portENTER_CRITICAL( & timerMux);
    interruptCounter--;
    portEXIT_CRITICAL( & timerMux);

    // plant input
    double U1 = 0;
    double U2 = 0;
    double U3 = 0;

    count1_old = count1;
    count2_old = count2;
    count3_old = count3;
    pcnt_get_counter_value(PCNT_UNIT_1, & count1);
    pcnt_get_counter_value(PCNT_UNIT_2, & count2);
    pcnt_get_counter_value(PCNT_UNIT_3, & count3);

    int16_t speedint1 = count1 - count1_old;
    double Speed1 = speedint1;

    int16_t speedint2 = count2 - count2_old;
    double Speed2 = speedint2;

    int16_t speedint3 = count3 - count3_old;
    double Speed3 = speedint3;

    // error and reference variables
    double Err1 = 0;
    double Ref1 = 0;
    double Err2 = 0;
    double Ref2 = 0;
    double Err3 = 0;
    double Ref3 = 0;
    // button one generates step reference
    if (1) {
      // maximal absolute value of the reference (in rad/s) is approx 180
      Ref1 = double(v1);
      Ref2 = double(v2);
      Ref3 = double(v3);

      // velocity controller for MOTOR 1
      Err1 = Ref1 - Speed1; // calculate current error
      U1 = prevU1 + 1.1 * Err1 - 0.925 * prevErr1; // difference equation of a PI controller
      // update pervious values
      prevErr1 = Err1;

      if (U1 > 255) {
        U1 = 255;
      }
      if (U1 < -255) {
        U1 = -255;
      }

      if (Ref1 == 0)
        U1 = 0;

      prevU1 = U1;

      // velocity controller for MOTOR 2
      Err2 = Ref2 - Speed2; // calculate current error
      U2 = prevU2 + 1.1 * Err2 - 0.925 * prevErr2; // difference equation of a PI controller
      // update pervious values
      prevErr2 = Err2;

      if (U2 > 255) {
        U2 = 255;
      }
      if (U2 < -255) {
        U2 = -255;
      }

      if (Ref2 == 0)
        U2 = 0;

      prevU2 = U2;

      // velocity controller for MOTOR 2
      Err3 = Ref3 - Speed3; // calculate current error
      U3 = prevU3 + 1.1 * Err3 - 0.925 * prevErr3; // difference equation of a PI controller
      // update pervious values
      prevErr3 = Err3;

      if (U3 > 255) {
        U3 = 255;
      }
      if (U3 < -255) {
        U3 = -255;
      }

      if (Ref3 == 0)
        U3 = 0;

      prevU3 = U3;
    } else {
      // zero all
      Err1 = 0;
      U1 = 0;
      prevErr1 = 0;
      prevU1 = 0;
      Err2 = 0;
      U2 = 0;
      prevErr2 = 0;
      prevU2 = 0;
      Err3 = 0;
      U3 = 0;
      prevErr3 = 0;
      prevU3 = 0;

    }

    digitalWrite(motor1pin1, calc_dir(U1));
    digitalWrite(motor1pin2, calc_dir(-1 * U1));
    digitalWrite(motor2pin1, calc_dir(U2));
    digitalWrite(motor2pin2, calc_dir(-1 * U2));
    digitalWrite(motor3pin1, calc_dir(U3));
    digitalWrite(motor3pin2, calc_dir(-1 * U3));
    ledcWrite(pwmCh1, abs(U1));
    ledcWrite(pwmCh2, abs(U2));
    ledcWrite(pwmCh3, abs(U3));
  }
}

static void Encoder_config(void) {

  //////// Motor1 encoder config ////////
  pcnt_config_t pcnt_config_1;

  pcnt_config_1.pulse_gpio_num = PCNT_INPUT_SIG_IO_1;
  pcnt_config_1.ctrl_gpio_num = PCNT_INPUT_CTRL_IO_1;
  pcnt_config_1.channel = PCNT_CHANNEL_0;
  pcnt_config_1.unit = PCNT_TEST_UNIT_1;

  // What to do on the positive / negative edge of pulse input?
  pcnt_config_1.pos_mode = PCNT_COUNT_INC; // Count up on the positive edge
  pcnt_config_1.neg_mode = PCNT_COUNT_DEC; // Keep the counter value on the negative edge

  // What to do when control input is low or high?
  pcnt_config_1.lctrl_mode = PCNT_MODE_REVERSE; // Reverse counting direction if low
  pcnt_config_1.hctrl_mode = PCNT_MODE_KEEP; // Keep the primary counter mode if high

  // Set the maximum and minimum limit values to watch
  // pcnt_config_1.counter_h_lim = PCNT_H_LIM_VAL;
  // pcnt_config_1.counter_l_lim = PCNT_L_LIM_VAL;

  pcnt_unit_config( & pcnt_config_1);

  pcnt_config_1.pulse_gpio_num = PCNT_INPUT_CTRL_IO_1;
  pcnt_config_1.ctrl_gpio_num = PCNT_INPUT_SIG_IO_1;
  pcnt_config_1.channel = PCNT_CHANNEL_1;
  pcnt_config_1.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config_1.hctrl_mode = PCNT_MODE_REVERSE;

  pcnt_unit_config( & pcnt_config_1);

  // Initialize PCNT's counter
  pcnt_counter_pause(PCNT_TEST_UNIT_1);
  pcnt_counter_clear(PCNT_TEST_UNIT_1);
  // pcnt_intr_enable(PCNT_TEST_UNIT_1);

  // Start counter
  pcnt_counter_resume(PCNT_TEST_UNIT_1);

  //////// Motor2 encoder config ////////
  pcnt_config_t pcnt_config_2;

  pcnt_config_2.pulse_gpio_num = PCNT_INPUT_SIG_IO_2;
  pcnt_config_2.ctrl_gpio_num = PCNT_INPUT_CTRL_IO_2;
  pcnt_config_2.channel = PCNT_CHANNEL_0;
  pcnt_config_2.unit = PCNT_TEST_UNIT_2;

  // What to do on the positive / negative edge of pulse input?
  pcnt_config_2.pos_mode = PCNT_COUNT_INC; // Count up on the positive edge
  pcnt_config_2.neg_mode = PCNT_COUNT_DEC; // Keep the counter value on the negative edge

  // What to do when control input is low or high?
  pcnt_config_2.lctrl_mode = PCNT_MODE_REVERSE; // Reverse counting direction if low
  pcnt_config_2.hctrl_mode = PCNT_MODE_KEEP; // Keep the primary counter mode if high

  // Set the maximum and minimum limit values to watch
  //pcnt_config_1.counter_h_lim = PCNT_H_LIM_VAL;
  //pcnt_config_1.counter_l_lim = PCNT_L_LIM_VAL;

  pcnt_unit_config( & pcnt_config_2);

  pcnt_config_2.pulse_gpio_num = PCNT_INPUT_CTRL_IO_2;
  pcnt_config_2.ctrl_gpio_num = PCNT_INPUT_SIG_IO_2;
  pcnt_config_2.channel = PCNT_CHANNEL_1;
  pcnt_config_2.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config_2.hctrl_mode = PCNT_MODE_REVERSE;

  pcnt_unit_config( & pcnt_config_2);

  // Initialize PCNT's counter
  pcnt_counter_pause(PCNT_TEST_UNIT_2);
  pcnt_counter_clear(PCNT_TEST_UNIT_2);
  //pcnt_intr_enable(PCNT_TEST_UNIT_2);

  // Start counter
  pcnt_counter_resume(PCNT_TEST_UNIT_2);

  //////// Motor3 encoder config ////////
  pcnt_config_t pcnt_config_3;

  pcnt_config_3.pulse_gpio_num = PCNT_INPUT_SIG_IO_3;
  pcnt_config_3.ctrl_gpio_num = PCNT_INPUT_CTRL_IO_3;
  pcnt_config_3.channel = PCNT_CHANNEL_0;
  pcnt_config_3.unit = PCNT_TEST_UNIT_3;

  // What to do on the positive / negative edge of pulse input?
  pcnt_config_3.pos_mode = PCNT_COUNT_INC; // Count up on the positive edge
  pcnt_config_3.neg_mode = PCNT_COUNT_DEC; // Keep the counter value on the negative edge

  // What to do when control input is low or high?
  pcnt_config_3.lctrl_mode = PCNT_MODE_REVERSE; // Reverse counting direction if low
  pcnt_config_3.hctrl_mode = PCNT_MODE_KEEP; // Keep the primary counter mode if high

  // Set the maximum and minimum limit values to watch
  //pcnt_config_1.counter_h_lim = PCNT_H_LIM_VAL;
  //pcnt_config_1.counter_l_lim = PCNT_L_LIM_VAL;

  pcnt_unit_config( & pcnt_config_3);

  pcnt_config_3.pulse_gpio_num = PCNT_INPUT_CTRL_IO_3;
  pcnt_config_3.ctrl_gpio_num = PCNT_INPUT_SIG_IO_3;
  pcnt_config_3.channel = PCNT_CHANNEL_1;
  pcnt_config_3.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config_3.hctrl_mode = PCNT_MODE_REVERSE;
  pcnt_unit_config( & pcnt_config_3);

  // Initialize PCNT's counter
  pcnt_counter_pause(PCNT_TEST_UNIT_3);
  pcnt_counter_clear(PCNT_TEST_UNIT_3);
  //pcnt_intr_enable(PCNT_TEST_UNIT_3);

  // Start counter
  pcnt_counter_resume(PCNT_TEST_UNIT_3);
}
