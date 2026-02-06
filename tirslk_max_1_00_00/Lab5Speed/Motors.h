//Initializes the 6 GPIO lines and puts DRV8838 drivers to sleep
//Returns right away
void Motor_Init(void);

//Stops both motors, puts DRV8838 drivers to sleep
//Returns right away
void Motor_Stop(void);

//Drives both motors forward at full speed
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_ForwardSimple(uint32_t time);

//Drives both motors backward at full speed
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_ReverseSimple(uint32_t time);

//Drives just the right motor forward at full speed
//Left motor is stopped (sleeping)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_LeftSimple(uint32_t time);

//Drives just the left motor forward at full speed
//Right motor is stopped (sleeping)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_RightSimple(uint32_t time);

//Drives both motors forward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_Forward(uint16_t duty, uint32_t time);

//Drives both motors backward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_Reverse(uint16_t duty, uint32_t time);

//Drives the right motor forward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Left motor is going in reverse at duty to complete a ‘zero-turn’
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_Left(uint16_t duty, uint32_t time);

//Drives the left motor forward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Right motor is going in reverse at duty (100 to 9900) to complete a ‘zero-turn’
//Runs for time duration (units=10ms), and then stops
//Returns after time*10m
void Motor_Right(uint16_t duty, uint32_t time);


