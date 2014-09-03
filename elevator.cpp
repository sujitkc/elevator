#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
/* Constants */
const int FLOORDISTANCE = 100; /* distance between floors = 100 units */
const int STOPDELAY     =   5; /* delay after the issue of stop command when the elevator stops */
const int SPEED         =  20; /* elevator moves at constant speed of 20 units/step */
const int UP            =   1;
const int STATIONARY    =   0;
const int DOWN          =  -1;

/* Input variables - BEGIN */
/* Input variables */
/* Request flags */
bool ReqF0;  /* Request from Ground floor    */
bool ReqF1;  /* Request from First floor     */
bool ReqF2;  /* Request from Second floor    */
bool ReqC0;  /* Car Request for Ground floor */
bool ReqC1;  /* Car Request for First floor  */
bool ReqC2;  /* Car Request for Second floor */
/* Input variables - END */

/* Internal variables - BEGIN */
int position; /* position of the elevator car */
int direction; /* current direction of the car */
                 /*  1 = UP         */
                 /*  0 = STATIONARY */
                 /* -1 = DOWN       */

/* The following flags indicate pending request at the floors */
/* NOTE
   ----
There are three sets of request variables:
- Reqn:  To be read 'Request for nth floor.'
- ReqFn: To be read 'Request originating from nth floor.'
- ReqCn: To be read 'Request for nth floor originating from the car.'

ReqFn and ReqCn are sets of input variables and they are set through requests
originating from the user.
Reqn is set of internal variables and their values are set through internal computation.
*/
bool Req0; /* Request Flag for Ground floor */
bool Req1; /* Request Flag for First floor */
bool Req2; /* Request Flag for Second floor */

bool stopCommand;      /* stop command */

bool isFloorApproaching0; /* is ground floor approaching? */
bool isFloorApproaching1; /* is first floor approaching?  */
bool isFloorApproaching2; /* is second floor approaching? */

int timeAtCurrentFloor;
/* Internal variables - END */

/* Delay variables - BEGIN */
bool ReqF0_prev;  /* Request from Ground floor    */
bool ReqF1_prev;  /* Request from First floor     */
bool ReqF2_prev;  /* Request from Second floor    */
bool ReqC0_prev;  /* Car Request for Ground floor */
bool ReqC1_prev;  /* Car Request for First floor  */
bool ReqC2_prev;  /* Car Request for Second floor */

int direction_prev;
bool stopCommand_prev; /* stop command status in the last step */
/* Delay variables - END */

void init() {
  position    = 0;
  direction   = STATIONARY;
  stopCommand = true;
  Req0        = false;
  Req1        = false;
  Req2        = false;
  ReqF0_prev  = false;
  ReqF1_prev  = false;
  ReqF2_prev  = false;
  ReqC0_prev  = false;
  ReqC1_prev  = false;
  ReqC2_prev  = false;

  direction_prev = STATIONARY;
  stopCommand_prev = false;
}

bool input() {
	return rand() % 100 == 1; /* stub input function to provide true with 1% probability. */
}

void inputs() {
  ReqF0 = input();
  ReqF1 = input();
  ReqF2 = input();
  ReqC0 = input();
  ReqC1 = input();
  ReqC2 = input();
}

int getDelayToReach(const int p1, const int p2) { return (p2 - p1) / SPEED; }
int getFloorHeight(const int f) { return f * FLOORDISTANCE; }

bool isFloorApproaching(const int f) {
  const int floorHeight = getFloorHeight(f);
  const int delayToReachFloor = getDelayToReach(position, floorHeight);
  if(delayToReachFloor <= STOPDELAY && delayToReachFloor >= 0) {
    return true;
  }
  else {
    return false;
  }
}

// Return true if the car is at floor f.
bool isCarAtFloor(const int f) {
  return position == getFloorHeight(f);
}

bool isCarMoving() { return direction != STATIONARY; }

/*
  Set Reqn to true if the car is not stationary on floor f.
  And there is either ReqFn or ReqCn true.
bool computeReqn(const int f) {
  return !isCarAtFloor(f) || isCarMoving(); 
}
*/

/*
For each of the floors, check if the ReqFn or ReqCn has just gone true.
If yes, then compute Reqn for that floor.
*/
void updateReqns() {
  if((ReqF0_prev == false && ReqF0 == true) || (ReqC0_prev == false && ReqC0 == true)) {
    Req0 = !isCarAtFloor(0) || isCarMoving(); 
  }
  if((ReqF1_prev == false && ReqF1 == true) || (ReqC1_prev == false && ReqC1 == true)) {
    Req1 = !isCarAtFloor(1) || isCarMoving(); 
  }
  if((ReqF2_prev == false && ReqF2 == true) || (ReqC2_prev == false && ReqC2 == true)) {
    Req2 = !isCarAtFloor(2) || isCarMoving(); 
  }
}

bool computeStopCommand() {
  bool floorApproaching0 = isFloorApproaching(0);
  bool floorApproaching1 = isFloorApproaching(1);
  bool floorApproaching2 = isFloorApproaching(2);

  if(floorApproaching0 && Req0) {
    return true;
  }
  if(floorApproaching1 && Req1) {
    return true;
  }
  if(floorApproaching2 && Req2) {
    return true;
  }
  return false;
}

int updatePosition() {
  if(direction_prev == STATIONARY) {
    return position;
  }
  else if(direction_prev == UP) {
    return position + SPEED;
  }
  else {
    return position - SPEED;
  }
}

bool isFloorAbove(int f) {
  return position < f * FLOORDISTANCE;
}

bool isFloorBelow(int f) {
  return position > f * FLOORDISTANCE;
}

void updateDirection() {
  if(direction_prev == STATIONARY) {
    if(floorTimerUp() == false) {
      direction = STATIONARY;
    }
    else if(isFloorAbove(1) && Req1) {
      direction = UP;
    }
    else if(isFloorAbove(2) && Req2) {
      direction = UP;
    }
    else if(isFloorBelow(1) && Req1) {
      direction = DOWN;
    }
    else if(isFloorBelow(2) && Req2) {
      direction = DOWN;
    }
    else {
      direction = STATIONARY;
    }
  }
  else if(direction == UP) {
    if(isFloorAbove(1) && Req1) {
      direction = UP;
    }
    else if(isFloorAbove(2) && Req2) {
      direction = UP;
    }
    else if(!isCarAtFloor(0) && !isCarAtFloor(1) && !isCarAtFloor(2)) {
      direction = UP;
    }
    else {
      direction = STATIONARY;
    }
  }
  else if(direction == DOWN) {
  }
}

void resetRequest() {
  if(isCarAtFloor(0) && !isCarMoving() && Req0 == true) {
    Req0 = false;
  }
  if(isCarAtFloor(1) && !isCarMoving() && Req1 == true) {
    Req1 = false;
  }
  if(isCarAtFloor(2) && !isCarMoving() && Req2 == true) {
    Req2 = false;
  }
}

void forwardCompute() {
  position = updatePosition();
  updateReqns();
  stopCommand = computeStopCommand();
  if(stopCommand) {
    direction = STATIONARY;
  }
  resetRequest();
}

void feedback_delay() {
  stopCommand_prev = stopCommand;
  direction_prev = direction;
  ReqF0_prev = ReqF0;
  ReqF1_prev = ReqF1;
  ReqF2_prev = ReqF2;
  ReqC0_prev = ReqC0;
  ReqC1_prev = ReqC1;
  ReqC2_prev = ReqC2;
}

void step() {
  inputs();
  forwardCompute();
  feedback_delay();
}

void print() {
  cout << "{ R0:" << Req0 << " R1:" << Req1 << " R2:" << Req2 " }, "
    "position = " << position << "; direction = " << direction << endl;
}

void execute() {
  init();
  while(true) {
    step();
    print();
  }
}

void timer() {
  while(true) {
    x1 = x2 - 1;
    x4 = x3 - start;
    if(x5 == true) {
      out = c;
    }
    else if(x2 != 0) {
      out = x2 - 1;
    }
    else {
      out = x2;
    }
    t_on = (out != 0);
    x5 = start;
  }
}

int main() {
  execute();
  return 0;
}
