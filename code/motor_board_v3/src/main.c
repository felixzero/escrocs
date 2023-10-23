#include "stepper.h"

#include <avr/delay.h>

#define TOP_SPEED 40
#define ACCELERATION_DURATION 1000
#define FLAT_DURATION 1600

int main()
{
    init_stepper();

    struct acceleration_step* initial_step = build_acceleration_step(NO_MOTION, TOP_SPEED, ACCELERATION_DURATION, POLARITY_CCW);
    struct acceleration_step* constant_step = build_acceleration_step(TOP_SPEED, TOP_SPEED, FLAT_DURATION, POLARITY_CCW);
    struct acceleration_step* deceleration_step = build_acceleration_step(TOP_SPEED, NO_MOTION, ACCELERATION_DURATION, POLARITY_CCW);
    struct acceleration_step* initial2_step = build_acceleration_step(NO_MOTION, TOP_SPEED, ACCELERATION_DURATION, POLARITY_CW);
    struct acceleration_step* constant2_step = build_acceleration_step(TOP_SPEED, TOP_SPEED, FLAT_DURATION, POLARITY_CW);
    struct acceleration_step* deceleration2_step = build_acceleration_step(TOP_SPEED, NO_MOTION, ACCELERATION_DURATION, POLARITY_CW);

    _delay_ms(10);

    initial_step->next_step = constant_step;
    constant_step->next_step = deceleration_step;
    deceleration_step->next_step = initial2_step;
    initial2_step->next_step = constant2_step;
    constant2_step->next_step = deceleration2_step;
    deceleration2_step->next_step = &stopped_step;


    struct acceleration_step* b_initial_step = build_acceleration_step(NO_MOTION, TOP_SPEED, ACCELERATION_DURATION, POLARITY_CCW);
    struct acceleration_step* b_constant_step = build_acceleration_step(TOP_SPEED, TOP_SPEED, FLAT_DURATION, POLARITY_CCW);
    struct acceleration_step* b_deceleration_step = build_acceleration_step(TOP_SPEED, NO_MOTION, ACCELERATION_DURATION, POLARITY_CCW);
    struct acceleration_step* b_initial2_step = build_acceleration_step(NO_MOTION, TOP_SPEED, ACCELERATION_DURATION, POLARITY_CW);
    struct acceleration_step* b_constant2_step = build_acceleration_step(TOP_SPEED, TOP_SPEED, FLAT_DURATION, POLARITY_CW);
    struct acceleration_step* b_deceleration2_step = build_acceleration_step(TOP_SPEED, NO_MOTION, ACCELERATION_DURATION, POLARITY_CW);

    _delay_ms(10);

    b_initial_step->next_step = b_constant_step;
    b_constant_step->next_step = b_deceleration_step;
    b_deceleration_step->next_step = b_initial2_step;
    b_initial2_step->next_step = b_constant2_step;
    b_constant2_step->next_step = b_deceleration2_step;
    b_deceleration2_step->next_step = &stopped_step;

    start_motion_immediately(initial_step, b_initial_step, &stopped_step);

    while(1);
}
