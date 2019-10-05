#include <Arduino.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <Stepper.h>

const int stepper1StepsPerRevolution = 200;
const int stepper2StepsPerRevolution = 200;

#define MOTOR_A_ENABLE 2
#define MOTOR_B_ENABLE 7
#define MOTOR_C_ENABLE 8
#define MOTOR_D_ENABLE 10

#define COIL_A_1 3
#define COIL_A_2 4
#define COIL_B_1 5
#define COIL_B_2 6
#define COIL_C_1 9
#define COIL_C_2 13
#define COIL_D_1 12
#define COIL_D_2 11

#define DEBUG

void receiveEvent(int bytes);

AccelStepper stepper1(AccelStepper::FULL4WIRE, COIL_A_1, COIL_A_2, COIL_B_1, COIL_B_2);
AccelStepper stepper2(AccelStepper::FULL4WIRE, COIL_C_1, COIL_C_2, COIL_D_1, COIL_D_2);

// if stepper 1 is not used, uses motors a and b, if stepper 2 is not used, uses motor c and d
boolean usingStepper1 = true;
boolean usingStepper2 = true;

enum commandType : uint16_t
{
  // motor definitions
  MOTOR_A_FORWARD,
  MOTOR_A_ENABLED,
  MOTOR_B_FORWARD,
  MOTOR_B_ENABLED,
  MOTOR_C_FORWARD,
  MOTOR_C_ENABLED,
  MOTOR_D_FORWARD,
  MOTOR_D_ENABLED,

  // stepper definitions
  STEPPER_1_STEPS_PER_REVOLUTION,
  STEPPER_1_SPEED,
  STEPPER_1_ENABLED,
  STEPPER_1_USED,
  STEPPER_2_STEPS_PER_REVOLUTION,
  STEPPER_2_SPEED,
  STEPPER_2_ENABLED,
  STEPPER_2_USED,
};

union payload_t {
  int16_t integer;
  boolean boolean_v;
};

struct command
{
  commandType type;
  payload_t payload;
};

command lastCommand;

void setup()
{
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);

  // stepper 1 and motor 1 and 2
  pinMode(COIL_A_1, OUTPUT);
  pinMode(COIL_A_2, OUTPUT);
  pinMode(COIL_B_1, OUTPUT);
  pinMode(COIL_B_2, OUTPUT);

  // stepper 2 and motor 3 and 4
  pinMode(COIL_A_1, OUTPUT);
  pinMode(COIL_A_2, OUTPUT);
  pinMode(COIL_B_1, OUTPUT);
  pinMode(COIL_B_2, OUTPUT);

  // enable pins
  pinMode(MOTOR_A_ENABLE, OUTPUT);
  pinMode(MOTOR_B_ENABLE, OUTPUT);
  pinMode(MOTOR_C_ENABLE, OUTPUT);
  pinMode(MOTOR_D_ENABLE, OUTPUT);

  digitalWrite(MOTOR_A_ENABLE, LOW);
  digitalWrite(MOTOR_B_ENABLE, LOW);
  digitalWrite(MOTOR_C_ENABLE, LOW);
  digitalWrite(MOTOR_D_ENABLE, LOW);

  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);

  stepper1.setSpeed(0);
  stepper2.setSpeed(0);
}

void loop()
{
  if (usingStepper1)
  {
    stepper1.runSpeed();
  }

  if (usingStepper2)
  {
    stepper2.runSpeed();
  }
}

void receiveEvent(int bytes)
{
  while (0 < Wire.available())
  {
    Wire.readBytes((byte *)&lastCommand, sizeof lastCommand);

    #ifdef DEBUG
    Serial.print("Received command ");
    Serial.print(lastCommand.type);
    Serial.print(", payload: ");
    Serial.println(lastCommand.payload.integer);
    #endif

    switch (lastCommand.type)
    {
      // motor definitions
    case MOTOR_A_FORWARD:
      if (!usingStepper1)
      {
        if (lastCommand.payload.boolean_v)
        {
          digitalWrite(COIL_A_1, LOW);
          digitalWrite(COIL_A_2, HIGH);
        }
        else
        {
          digitalWrite(COIL_A_2, LOW);
          digitalWrite(COIL_A_1, HIGH);
        }
      }
      break;
    case MOTOR_A_ENABLED:
      if (!usingStepper1)
      {
        digitalWrite(MOTOR_A_ENABLE, lastCommand.payload.boolean_v);
      }
      break;
    case MOTOR_B_FORWARD:
      if (!usingStepper1)
      {
        if (lastCommand.payload.boolean_v)
        {
          digitalWrite(COIL_B_1, LOW);
          digitalWrite(COIL_B_2, HIGH);
        }
        else
        {
          digitalWrite(COIL_B_2, LOW);
          digitalWrite(COIL_B_1, HIGH);
        }
      }
      break;
    case MOTOR_B_ENABLED:
      if (!usingStepper1)
      {
        digitalWrite(MOTOR_B_ENABLE, lastCommand.payload.boolean_v);
      }
      break;
    case MOTOR_C_FORWARD:
      if (!usingStepper2)
      {
        if (lastCommand.payload.boolean_v)
        {
          digitalWrite(COIL_C_1, LOW);
          digitalWrite(COIL_C_2, HIGH);
        }
        else
        {
          digitalWrite(COIL_C_2, LOW);
          digitalWrite(COIL_C_1, HIGH);
        }
      }
      break;
    case MOTOR_C_ENABLED:
      if (!usingStepper2)
      {
        digitalWrite(MOTOR_C_ENABLE, lastCommand.payload.boolean_v);
      }
      break;
    case MOTOR_D_FORWARD:
      if (!usingStepper2)
      {
        if (lastCommand.payload.boolean_v)
        {
          digitalWrite(COIL_D_1, LOW);
          digitalWrite(COIL_D_2, HIGH);
        }
        else
        {
          digitalWrite(COIL_D_2, LOW);
          digitalWrite(COIL_D_1, HIGH);
        }
      }
      break;
    case MOTOR_D_ENABLED:
      if (!usingStepper2)
      {
        digitalWrite(MOTOR_D_ENABLE, lastCommand.payload.boolean_v);
      }
      break;

    // stepper definitions
    case STEPPER_1_SPEED:
      stepper1.setSpeed(lastCommand.payload.integer);
      break;
    case STEPPER_1_ENABLED:
      if (usingStepper1)
      {
        digitalWrite(MOTOR_A_ENABLE, lastCommand.payload.boolean_v);
        digitalWrite(MOTOR_B_ENABLE, lastCommand.payload.boolean_v);
      }
      break;
    case STEPPER_1_USED:
      usingStepper1 = lastCommand.payload.boolean_v;
      break;
    case STEPPER_2_SPEED:
      stepper2.setSpeed(lastCommand.payload.integer);
      break;
    case STEPPER_2_ENABLED:
      if (usingStepper1)
      {
        digitalWrite(MOTOR_C_ENABLE, lastCommand.payload.boolean_v);
        digitalWrite(MOTOR_D_ENABLE, lastCommand.payload.boolean_v);
      }
      break;
    case STEPPER_2_USED:
      usingStepper2 = lastCommand.payload.boolean_v;
      break;
    default:
      Serial.println("Unknown command. Ignoring.");
    }
  }

  #ifdef DEBUG
  Serial.println("Finished processing.");
  #endif
}