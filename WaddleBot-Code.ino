#include <Servo.h>

// ----------------------
// Servo Objects
// ----------------------
Servo leftThigh, rightThigh;
Servo leftFoot, rightFoot;

// ----------------------
// Smooth Motion Engine
// ----------------------
struct ServoMotion {
  Servo* s;
  int startPos;
  int targetPos;
  unsigned long startTime;
  unsigned long duration;
  bool active;
} motions[4];

void moveServo(Servo &servo, int fromPos, int toPos, unsigned long duration) {
  for (int i = 0; i < 4; i++) {
    if (!motions[i].active) {
      motions[i].s = &servo;
      motions[i].startPos = fromPos;
      motions[i].targetPos = toPos;
      motions[i].startTime = millis();
      motions[i].duration = duration;
      motions[i].active = true;
      return;
    }
  }
}

void updateMotions() {
  unsigned long t = millis();

  for (int i = 0; i < 4; i++) {
    if (motions[i].active) {
      float progress = float(t - motions[i].startTime) / motions[i].duration;
      if (progress >= 1.0f) progress = 1.0f;

      int pos = motions[i].startPos + (motions[i].targetPos - motions[i].startPos) * progress;
      motions[i].s->write(pos);

      if (progress >= 1.0f) motions[i].active = false;
    }
  }
}

bool allServosIdle() {
  for (int i = 0; i < 4; i++) {
    if (motions[i].active) return false;
  }
  return true;
}


// ----------------------
// GAIT STATE MACHINE
// ----------------------
enum GaitState {
  SHIFT_RIGHT,
  LIFT_LEFT,
  SWING_LEFT,
  PLACE_LEFT,
  SHIFT_LEFT,
  LIFT_RIGHT,
  SWING_RIGHT,
  PLACE_RIGHT
};

GaitState state = SHIFT_RIGHT;

// servo resting positions:
int LT = 90, RT = 90, LF = 90, RF = 90;

void doGaitStep() {
  if (!allServosIdle()) return;

  switch (state) {

    case SHIFT_RIGHT:
      moveServo(rightThigh, RT, RT + 10, 300);  // right moves outward
      moveServo(leftThigh, LT, LT + 10, 300);     //left thigh inwards
      state = LIFT_LEFT;
      break;

    case LIFT_LEFT:
      moveServo(leftFoot, LF, LF + 30, 300);
      state = SWING_LEFT;
      break;

    case SWING_LEFT:
      moveServo(leftThigh, LT + 10, LT - 10, 300);
      state = PLACE_LEFT;
      break;

    case PLACE_LEFT:
      moveServo(leftFoot, LF + 30, LF, 300);
      state = SHIFT_LEFT;
      break;

    case SHIFT_LEFT:
      moveServo(leftThigh, LT + 10, LT - 10, 300);  // right moves outward
      moveServo(rightThigh, RT + 10, RT - 10, 300);
      state = LIFT_RIGHT;
      break;

    case LIFT_RIGHT:
      moveServo(rightFoot, RF, RF - 30, 300);
      state = SWING_RIGHT;
      break;

    case SWING_RIGHT:
      moveServo(rightThigh, RT - 10, RT + 10, 300);
      state = PLACE_RIGHT;
      break;

    case PLACE_RIGHT:
      moveServo(rightFoot, RF - 30, RF, 300);
      state = SHIFT_RIGHT;
      break;
  }
}


// ----------------------
// SETUP
// ----------------------
void setup() {
//  leftThigh.attach(8);
//  rightThigh.attach(10);
//  leftFoot.attach(4);
//  rightFoot.attach(6);

//left and right are reversed
  leftThigh.attach(11);
  rightThigh.attach(5);
  leftFoot.attach(3);
  rightFoot.attach(9);

  leftThigh.write(LT);
  rightThigh.write(RT);
  leftFoot.write(LF);
  rightFoot.write(RF);

  delay(1500);
}


// ----------------------
// MAIN LOOP
// ----------------------
void loop() {
  updateMotions();   // runs the smooth motion engine
  doGaitStep();      // state machine for walking
}
