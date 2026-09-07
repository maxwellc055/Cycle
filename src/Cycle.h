#ifndef CYCLE_H
#define CYCLE_H
#endif

#define MAX_CYCLE_SIZE 16

#include <Arduino.h>


class Cycle
{
  public:
    /*Phase Sub-Class*/
    class Phase
    {
      public:
        float shape; //Order of curve, 1: linear, 2: squared
        float start_value; //Set when phase begins, see begin()
        float end_value; //Final target value
        long start_time; //Set when phase is begins, see begin()
        long duration_ms; //Milliseconds
        bool enabled = true; //Whether or not phase is used
        bool active = false; //True when phase is presently running
        bool completed = false; //Flagged true when phase has exited or by interp_Value when completed
      
        /*Phase Functions*/
        Phase(); //Empty constructor for array initialization

        //Constructor. end_value in RPM, duration in ms. Shape as an order/rate. 1: linear, 2: squared
        Phase(float end_value, long duration_ms, float shape);

        //Enable phase. Phases are enabled upon construction
        void enable();

        //Disable phase so it will be skipped, but don't delete it
        void disable();

        //Start phase, typically called by owner Cycle
        void begin(float curr_target);

        //End phase, typically called by owner Cycle
        float exit();

        //Find and return target value based on time since the start time
        float interp_Value();
    };
    
    /*Cycle Functions*/

    //Empty constructor for array initialization
    Cycle();

    //Runs cycle, manages phases, returns new target value
    float run_Cycle(float curr_target);

    //Adds phase to the end of the list
    //Returns 1 if there is room for another phase when requested, 0 otherwise
    bool add_Phase(Phase phase);

    //Adds phase to the end of the list
    //Returns 1 if there is room for another phase when requested, 0 otherwise
    bool add_Phase(float end_value, long duration_ms, float shape);

    ////Effectively 'removes' phase. Actually just rearranges the array so the 'removed' phase is moved to the back to be ignored and overwritten at next addition
    //Returns 1 if index to remove was valid, 0 otherwise
    bool remove_Phase(uint8_t index);

    //Swaps phases at positions i1 and i2 if they exist.
    //Returns 1 if indices are valid. 0 otherwise
    bool swap_Phases(uint8_t i1, uint8_t i2);

    //Returns 1 if the cycle is complete, 0 otherwise
    bool is_Complete();

    //Set variables to indicate the cycle has been run to completion
    void end();

  public:
    /*Cycle Fields*/
    Phase phases[MAX_CYCLE_SIZE]; //Stores phases, define maximum number at top of page
    unsigned long total_time = 0; //Milliseconds, changes when phases are added
    unsigned long start_time; //Set when cycle begins
    uint8_t num_phases = 0; //Number of phases, also next open index for phase addition
    uint8_t curr_phase = 0; //Index of phase currently running, if cycle is active
    bool active = false; //True when cycle is presently running
    bool completed = false; //Triggered when last phase is complete
};
