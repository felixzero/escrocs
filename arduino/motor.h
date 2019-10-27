 
#define ACCELERATION_RATE 1 // in ticks/s2
#define TARGET_SPEED 20 // in ticks/s
#define SLOW_DOWN_DISTANCE 200 // in ticks

#define AXLES_WIDTH 311500 // in um
#define MOTOR_ENC_TICKS_PER_TURN 360 // in ticks
#define MOTOR_FWD_MOTION_PER_TICK (628319 / MOTOR_ENC_TICKS_PER_TURN) // (2*pi*10cm) in um/tick
#define MOTOR_ROTATION_PER_TICK (57296 * MOTOR_FWD_MOTION_PER_TICK / AXLES_WIDTH) // in millideg/tick

#define IDLE_MODE 0
#define FORWARD_MODE 1
#define TURNING_MODE 2

void initialize_motor();
void set_raw_motor_target(int mode, long raw_pos);
void set_motor_target(int mode, long pos);
void poll_motor_regulation();
void stop_motor_motion();
int motor_is_idle();
long get_last_motor_error();
