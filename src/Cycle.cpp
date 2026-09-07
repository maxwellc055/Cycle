#include "Cycle.h"

#define MAX_CYCLE_SIZE 16


/*Phase Functions*/

//Empty constructor for array initialization
Cycle::Phase::Phase(){}; 


//Constructor; end_value in RPM, duration in ms. Shape as an order/rate. 1: linear, 2: squared
Cycle::Phase::Phase(float end_value, long duration_ms, float shape)
{
  this->end_value = end_value;
  this->duration_ms = duration_ms;
  this->shape = shape;
}


//Enable phase. Phases are enabled by default upon construction
void Cycle::Phase::enable()
{
  enabled = true;
}


//Disable phase so it will be skipped, but don't delete it
void Cycle::Phase::disable()
{
  enabled = false;
}


//Start phase, typically called by owner Cycle
void Cycle::Phase::begin(float curr_target)
{
  start_time = millis();
  start_value = curr_target;
  active = true;
}


//End phase, typically called by owner Cycle
float Cycle::Phase::exit()
{
  active = false;
  completed = true;
  return end_value; //Accounts for small errors at end of phase due to computational speed
}


//Find and return target value based on time since the start time
float Cycle::Phase::interp_Value()
{
  float time_fac = (millis() - start_time)/(duration_ms * 1.0); //Don't forget your type conversions eek
  
  if(time_fac >= 1) //Can change 1 to something like 0.98-0.1.02 for a small corrective factor based on program speed
  {
    completed = true;
    return end_value;
  }
  
  return start_value + (end_value - start_value) * pow(time_fac, shape);
}


/*Cycle Functions*/

Cycle::Cycle(){}; //Empty constructor for array initialization


//Runs cycle, manages phases, returns new target value
float Cycle::run_Cycle(float curr_target)
{
  if(!phases[curr_phase].enabled) //Skip disabled phases
  {
    if(curr_phase < num_phases - 1) //Switch to next phase if possible
    {
      curr_phase++;          
    }
    return curr_target;
  }

  if(num_phases == 0 || curr_phase > num_phases - 1) return 0;

  if(active == false && completed == false) //Start cycle
  {
    active = true;
    start_time = millis();
  }

  if(!phases[curr_phase].active && !phases[curr_phase].completed) //If phase needs to be started
  {
    phases[curr_phase].begin(curr_target);
    return curr_target;
  }

  else if(!phases[curr_phase].completed) //Reached and entered anytime phase is active and not complete. Update target value
  {
    return phases[curr_phase].interp_Value();
  }

  else
  {
    if(curr_phase < num_phases - 1) //Switch to next phase if possible
    {
      curr_phase++;
      return phases[curr_phase - 1].exit();          
    }
  }
  
  end();
  return 0;
}


//Adds phase to the end of the list
//Returns 1 if there is room for another phase, 0 otherwise
bool Cycle::add_Phase(Phase phase)
{
  if(num_phases < MAX_CYCLE_SIZE)
  {
    phases[num_phases] = phase;
    num_phases++;
    total_time += phase.duration_ms;
    return 1;
  }

  return 0;
}


//Adds phase to the end of the list
//Returns 1 if there is room for another phase, 0 otherwise
bool Cycle::add_Phase(float end_value, long duration_ms, float shape)
{
  Phase phase = Phase(end_value, duration_ms, shape);
  return add_Phase(phase);
}


//Effectively 'removes' phase. Actually just rearranges the array so the 'removed' phase is moved to the back to be ignored and overwritten at next addition
//Returns 1 if index to remove was valid, 0 otherwise
bool Cycle::remove_Phase(uint8_t index)
{
  total_time -= phases[index].duration_ms;

  if(index > -1 && index < num_phases)
  {
    for(int i = index; i < num_phases - 1; i++)
    {
      phases[i] = phases[i+1];
    }

    phases[num_phases - 1] = Phase();
    num_phases--;
    return 1;
  }

  return 0;
}


//Swaps phases at positions i1 and i2 if they exist.
//Returns 1 if indices are valid. 0 otherwise
bool Cycle::swap_Phases(uint8_t i1, uint8_t i2)
{
  if(i1 > -1 && i1 < num_phases && i2 > -1 && i2 < num_phases)
  {
    Phase temp = phases[i1];
    phases[i1] = phases[i2];
    phases[i2] = temp;
    return 1;
  }
  
  return 0;
}

//Set variables to indicate the cycle has been run to completion
bool Cycle::is_Complete()
{
  return completed;
}


void Cycle::end()
{
  curr_phase++;
  active = false;
  completed = true;
}
