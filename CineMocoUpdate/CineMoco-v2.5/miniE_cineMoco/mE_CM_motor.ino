/*

    (c) 2015 Airic Lenz
    
    The 1st version of this code, dealing with core functionalities, 
    was heavily inspired by the OpenMoCo Engine by C.A. Church
    and is basically based on it. Thank you for your great work! 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



//////////////////////////////////////////////////////////////////////
//                                                                  //
// select one state and test (values can be "true" or "false":      //

#define CHECK_COM   true 

//                                                                  //
//                                                                  //
//////////////////////////////////////////////////////////////////////

 
 
  // count the number of pulses
unsigned long step_count = 0;                   

  // delta between min speed delay and max speed delay
unsigned long delay_delta;   

  // the variable that holds the factor of the max speed (
  // e.g. 0.4 of max speed)
float speed_fac;  

  // the ramp that is used (after corrections)
unsigned long used_motor_ramp_in;
unsigned long used_motor_ramp_out;

  // the steps we are going to travel
unsigned long motor_steps;

  // constinuous mode variables
float steps_per_milli;

  // the number of steps we need to to for the next continuous phase 
unsigned long steps_to_do;

  // counter fot all continuous steps done
unsigned long continuous_steps_done;




// ===================================================================================
boolean motor_is_in_motor_phase()           { return action_status & B00000010; }
boolean motor_is_in_post_phase()            { return action_status & B00000001; }

boolean motor_is_sleep_enabled()            { return isBit(motor_status, BIT_7); }

boolean motor_is_set_distance()             { return motor_steps > 0; } 

boolean motor_is_keep_powered()             { return motor_status & B00010000; } 
void motor_set_keep_powered_flag()          { motor_status |= B00010000; } 
void motor_delete_keep_powered_flag()       { bitClear(motor_status, 4); } 

boolean motor_is_key_start_set()            { return isBit(motor_key_status, BIT_0); } 
void motor_set_key_start_flag()             { setBit(motor_key_status, BIT_0); } 
void motor_delete_key_start_flag()          { deleteBit(motor_key_status, BIT_0); }

boolean motor_is_key_end_set()              { return isBit(motor_key_status, BIT_1); } 
void motor_set_key_end_flag()               { setBit(motor_key_status, BIT_1); } 
void motor_delete_key_end_flag()            { deleteBit(motor_key_status, BIT_1); } 

boolean motor_is_looping()                  { return isBit(motor_key_status, BIT_2); } 

boolean motor_is_program_starting_flag()    { return isBit(motor_status, BIT_1); }
void motor_set_program_starting_flag()      { setBit(motor_status, BIT_1); }
void motor_delete_program_starting_flag()   { deleteBit(motor_status, BIT_1); }



boolean motor_is_radial()                   { return (motor_calibration_index == 0); }


void motor_reset_continuous_steps()         { continuous_steps_done = 0; }



// ===================================================================================
// starts the motor phase and returns if the motor phase was started
// ===================================================================================
boolean motor_start_phase() {
 
  //motor_calculate_steps();
  
  // start the motor phase if we have motor_steps defined
  if (motor_steps > 0) {

    // open the slot for the motor    
    action_status |= B00000010;   
    
    action_start_time = millis();
    
    return true;
  }
  
  return false;
}


// ===================================================================================
void motor_stop_phase() {
 
  // close the time slot for the motor    
  bitClear(action_status, 1);   
  
}


// ===================================================================================
void motor_start_post_phase() {
  
  // if we want a motor post delay and
  // we are not in stop-motion mode, activate it now
  if (motor_post > 0) { 
      //&& (!(core_is_mode_stop_motion()))) {
    
    // set the corresponding flag
    action_status |= B00000001;  
    
    // remember the time we started
    action_start_time = millis();
  } 
  
}


// ===================================================================================
void motor_stop_post_phase() {
  
  // clear the motor post flag...
  bitClear(action_status, 0); // B11111110
            
}



// ===================================================================================
void motor_move_continuous() {

  now = millis();
     
  // time total since start * s_p_m = theoret. steps total
  unsigned long theo_steps = ((float) (now - program_start_time)) * steps_per_milli;
  
  // check this because with both values equal, the cuntion leads to wrong 
  // and freaking big continuous step numbers...
  if (theo_steps > continuous_steps_done) steps_to_do = (unsigned long) theo_steps - continuous_steps_done;
  else                                    steps_to_do = 0;

  
  // do all the steps we need to do
  for (uint16_t i=0; i < steps_to_do; i++) {
    
    // Move stepper one pulse
    digitalWrite(DEF_MOTOR_STEP_PIN, HIGH);
    delayMicroseconds(20);
    digitalWrite(DEF_MOTOR_STEP_PIN, LOW);
    delayMicroseconds(150);
    
    continuous_steps_done++;    
  } 
 
  // add the steps to the home counter
  motor_add_steps_to_home_counter(steps_to_do);
  

}

// ===================================================================================
// sets the stepping size for the stepper driver
void motor_setStepSize()
{
  // set the stepping
  byte bits = motor_step_sizes[motor_step_sizes_index].pinStates;
                  
  digitalWrite(DEF_MOTOR_MS1_PIN, isBit(bits, BIT_0));
  digitalWrite(DEF_MOTOR_MS2_PIN, isBit(bits, BIT_1));  
}


// ===================================================================================
// returns the real calibration values based on motor type and step-size
float motor_getCalibrationValue() {
  
  // get the actual step size (1, 2, 4)
  switch (motor_step_sizes_index) {
   
    case 0:   return motor_calibrations[motor_calibration_index].stepsPerCm;          // 1/8 step
    case 1:   return motor_calibrations[motor_calibration_index].stepsPerCm / 2.0;    // 1/4 step
    case 2:   return motor_calibrations[motor_calibration_index].stepsPerCm / 4.0;    // 1/2 step
        
  };
  
}



// returns false if the motor was stopped by the user or on another error
// ===================================================================================
boolean motor_video_move(boolean check_for_errors, 
                         byte ramp_type, 
                         byte ramp_in_percent, 
                         byte ramp_out_percent) {
  
  step_count = 0;
  unsigned long step_delay;
  
  // set the new max speed
  
  float fac = ((float) (100 - motor_video_speed)) * 0.01;
  
  // linear behaviour is not very good - thus we choose a non-linear bahaviour:
  //fac *= fac; // linear behaviour needed for synchonizing chained devices
  
  motor_step_delay_min = ((float) DEF_MOTOR_MAX_DELAY_STANDARD) * fac;
                        
  // (re) calculate some initial values needed for the ramping curve:
  // ramping was not cropped: speed factor = 1
  speed_fac = 1;
    
  // multiply the new speed delta with our factor
  delay_delta = speed_fac * (float) (motor_step_delay_max - motor_step_delay_min);
  
  // re calculate the motor steps we need to do
  motor_steps = ((float) motor_video_dist) * motor_getCalibrationValue(); 
  
 
  // set the ramp values
  used_motor_ramp_in = ((float) motor_steps * (float) ramp_in_percent) / 100.0;
  used_motor_ramp_out = ((float) motor_steps * (float) ramp_out_percent) / 100.0;

  
  // avoid divisons by zero in the next step
  if (used_motor_ramp_in == 0) used_motor_ramp_in = 1;
  if (used_motor_ramp_out == 0) used_motor_ramp_out = 1;
  
  
  
  // wake up the motor
  digitalWrite(DEF_MOTOR_SLEEP_PIN, HIGH);
      
  // wait a little time to let the motor settle and break  
  core_delay(DEF_MOTOR_BREAK_DELAY);    
      
  // while there are still steps to do...
  while (step_count < motor_steps) {

    // K E Y   I N P U T   C H E C K    
    // copy the volantile buffer for less access to it
    key_pressed = vol_key_pressed;
    
    // check if a key was pressed
    if (key_pressed != B0) {
      
      // delete the key-pressed-buffer
      vol_key_pressed = B0;
        
      // if we stopped the program, leave this function with false
      if (!motor_do_key_handling(key_pressed)) {
        
        // before leaving, add the done steps to the home-distance
        motor_add_steps_to_home_counter(step_count);
        
        return false;
      }
    }
    
    // C H E C K   F O R   C O M   P A C K E T
    if (CHECK_COM) com.executeCommunication();
    if (com_is_stop_flag()) return false;
    
    
    // B A C K L I G H T   C H E C K
    // if backlight is on, check if it needs to be turned off
    // after several time of no user action
    if ( 
        ((interaction_time + (unsigned long) backlight_wait * 1000UL) < millis()) &&
          ui_is_backlight_flag()  
       ) {
      
      ui_disable_backlight();
    }
    
    
    // determine the ramp delay for the current step
    step_delay = motor_calculate_ramp_delay(ramp_type);
       
    // Move stepper one pulse using the delay just calculated
    digitalWrite(DEF_MOTOR_STEP_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(DEF_MOTOR_STEP_PIN, LOW);
    if (step_delay > 0) {
      
      // delayMicroseconds only takes unsigned int
      // plus there is an upper precission limit of 16383 
      // (we still use the upper limit of 20k):
      if (step_delay > 20000) delay (step_delay / 1000UL);
      else                    delayMicroseconds(step_delay);  
    }
    
    step_count ++;
      
  }
  
  // add everything to the home variables
  motor_add_steps_to_home_counter(step_count);
    
  // wait a little time to let the motor settle and break  
  core_delay (DEF_MOTOR_BREAK_DELAY);
  
  // bring the motor back to sleep 
  if (!motor_is_keep_powered()) {
    digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
  }
  
  return true;
  
}




// returns false if the motor was stopped by the user or on another error
// ===================================================================================
boolean motor_move(byte ramp_type) {
  
  step_count = 0;
  
  if (!core_is_program_running()) {
    com_delete_stop_flag();
  }
    
  // wake up the motor
  digitalWrite(DEF_MOTOR_SLEEP_PIN, HIGH);
      
  // wait a little time to let the motor settle and break  
  core_delay(DEF_MOTOR_BREAK_DELAY);    
      
  // while there are still steps to do...
  while (step_count < motor_steps) {
    
    // copy the volantile buffer for less access to it
    key_pressed = vol_key_pressed;
    
    
    // check if a key was pressed
    if (key_pressed != B0) {
      
      // delete the key-pressed-buffer
      vol_key_pressed = B0;
        
      // if we stopped the program, leave this function with false
      if (!motor_do_key_handling(key_pressed)) {
        
        // before leaving, add the done steps to the home-distance
        motor_add_steps_to_home_counter(step_count);
        
        return false;
      }
    }
    
        
    // C H E C K   F O R   C O M   P A C K E T
    if (CHECK_COM) com.executeCommunication();
    if (com_is_stop_flag()) {
      return false;
    }
    
    // determine the ramp delay for the current step
    unsigned long step_delay = motor_calculate_ramp_delay(ramp_type);

        
    // Move stepper one pulse using the delay just calculated
    digitalWrite(DEF_MOTOR_STEP_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(DEF_MOTOR_STEP_PIN, LOW);
    if (step_delay > 0) {
      
      // delayMicroseconds only takes unsigned int
      // plus there is an upper precission limit of 16383 
      // (we still use the upper limit of 20k):
      if (step_delay > 20000) delay (step_delay / 1000UL);
      else                    delayMicroseconds(step_delay);  
    }
    
    step_count ++;
      
  }
  
  // add everything to the home variables
  motor_add_steps_to_home_counter(step_count);
  
  
  // wait a little time to let the motor settle and break  
  core_delay (DEF_MOTOR_BREAK_DELAY);
  
  
  // bring the motor back to sleep if desired
  if ((motor_is_sleep_enabled()) && 
      (!motor_is_keep_powered())) {
        
    digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
  }
  
  
  return true;
  
}


// ===================================================================================
unsigned long motor_calculate_ramp_delay(byte ramp_type) {
  
  unsigned long res;
  float         x_scale;
  
  // accelleration phase
  if ((long) step_count < (long) used_motor_ramp_in) {
     
    // x scale represents the position in the current speed-phase between 0 (beginning) and 1 (end)
    x_scale = (float) step_count / (float) used_motor_ramp_in;

  }     
    
  // reached deceleration phase  
  else if ((long) step_count > ((long) motor_steps - (long) used_motor_ramp_out)) {
    
    // x scale represents the position in the current speed-phase between 0 (beginning) and 1 (end)
    x_scale = (float) (motor_steps - step_count) / (float) used_motor_ramp_out;

  }
  
  // reached max speed phase  
  else  {
    
    // wait a little bit because here should be a calculation which requires some time 
    // (each calc requires a different amount); 
    switch(ramp_type) {
      case DEF_RAMPING_LINEAR:    delayMicroseconds(45); break;
      case DEF_RAMPING_ATANGENS:  delayMicroseconds(350); break; 
      case DEF_RAMPING_SINUS:     delayMicroseconds(200); break;
    }                
    
    // now return the result
    return motor_step_delay_max - delay_delta; 
  }
  
  
  // calculate the actual delay
  switch(ramp_type) {
    case DEF_RAMPING_LINEAR:    res = x_scale * delay_delta;                                        // y = x
                                break;
    case DEF_RAMPING_ATANGENS:  res = (atan(15.0 * x_scale) / 1.51) * delay_delta;                  // y = atan(x * 15) / 1.51
                                break;
    case DEF_RAMPING_SINUS:     res = (((cos(PI * (x_scale - 1.0))) / 2.0) + 0.5) * delay_delta;    // y = (cos(Pi(x - 1)) / 2) + 0.5
                                break;
  }  


  return motor_step_delay_max - res;
  
}


// ===================================================================================
void motor_jog(byte key, byte ramp_type) {
  
  // wake the motor up
  digitalWrite(DEF_MOTOR_SLEEP_PIN, HIGH);
  
  // set the motor speed for jogging
  float speedRange = (motor_step_delay_max >> 2) - motor_step_delay_min;
  speedRange *= ((float) motor_speed_jog / 100.0);
  motor_step_delay_min  = (motor_step_delay_max >> 2 ) - speedRange;
    
  // wait a little time to let the motor settle and break  
  //core_delay(DEF_MOTOR_BREAK_DELAY);  
  
  // set the ramping values
  used_motor_ramp_in = motor_ramp_jog;
  used_motor_ramp_out = motor_ramp_jog;
  
  // calculate the ramp delta from min speed to max speed
  motor_calculate_ramp_delay_delta(true, motor_ramp_jog);
  
  // our variable to count the steps we do
  step_count = 0;
  motor_steps = DEF_LONG_MAX;
  
  // store the old direction
  boolean old_dir = isBit(motor_status, BIT_3);
  
  
  // Set the stepper direction - use an inverted direction for
  // the case the motor is a radial motor (type = axis)
  if (motor_is_radial()) {
    motor_set_direction(key == KEY_RIGHT);
  } else {
    motor_set_direction(key == KEY_LEFT);
  }
    
  // the switch var
  boolean key_still_down = true;
  
    
  // the stepping loop
  while (key_still_down) {
        
    com.executeCommunication();
        
    if (key == KEY_LEFT) key_still_down = keys_read_key_left();
    if (key == KEY_RIGHT) key_still_down = keys_read_key_right();
    if (com_get_received_key() != KEY_NONE) key_still_down = true;

    // determine the ramp delay for the current step
    unsigned long step_delay = motor_calculate_ramp_delay(ramp_type);
    
    // Move stepper one pulse using the delay just calculated
    digitalWrite(DEF_MOTOR_STEP_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(DEF_MOTOR_STEP_PIN, LOW);
    if (step_delay > 0) {
      
      // delayMicroseconds only takes unsigned int
      // plus there is an upper precission limit of 16383 
      // (we still use the upper limit of 20k):
      if (step_delay > 20000) delay (step_delay / 1000UL);
      else                    delayMicroseconds(step_delay);  
    }
    
    step_count++;
    
  } // end: while key is down 
  
  
  
  // --- do out-ramping ---
  
  used_motor_ramp_in = step_count;
  
  // determine the amount of steps we need to ramp out
  if (step_count <= motor_ramp_jog) motor_steps = step_count << 1; // * 2
  else                              motor_steps = step_count + used_motor_ramp_out;
  
  while (step_count < motor_steps) {
    
    // determine the ramp delay for the current step
    unsigned long step_delay = motor_calculate_ramp_delay(ramp_type);
    
    // Move stepper one pulse using the delay just calculated
    digitalWrite(DEF_MOTOR_STEP_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(DEF_MOTOR_STEP_PIN, LOW);
    if (step_delay > 0) {
      
      // delayMicroseconds only takes unsigned int
      // plus there is an upper precission limit of 16383 
      // (we still use the upper limit of 20k):
      if (step_delay > 20000) delay (step_delay / 1000UL);
      else                    delayMicroseconds(step_delay);  
    }
    
    step_count++;
    
  }
  
  
  // add everything to the home variables
  motor_add_steps_to_home_counter(step_count);
  
  // set old direction
  motor_set_direction(old_dir);
  
  // set the original motor speed
  motor_step_delay_max  = DEF_MOTOR_MAX_DELAY_STANDARD; 
  motor_step_delay_min  = DEF_MOTOR_MIN_DELAY_STANDARD;  
    
  // wait a little time to let the motor settle and break  
  //core_delay(DEF_MOTOR_BREAK_DELAY);  
  
  // bring the motor back to sleep 
  if (!motor_is_keep_powered()) {
    digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
  }
  
}





// ===================================================================================
void motor_prepare_values(boolean recalculate_steps, boolean create_ramp) {
  
  // calculate the motor steps if required
  if (recalculate_steps) motor_calculate_steps();
  
  unsigned long used_motor_ramp;

  
  if (create_ramp) {
    
    // caclulate the used motor ramp (ramping percent times half of the distance to be moved
    used_motor_ramp = ((float) motor_ramp_percent / 100.0) * ((float)(motor_steps >> 1));
     
  } 
  
  // if we are not supposed to create the ramp distance from parameters,
  // check the default value for validity
  else {
        
    // if ramp is longer than half of the total phase - shorten it (/2):
    if (motor_ramp > (motor_steps >> 1)) {
      used_motor_ramp = motor_steps >> 1;
    }  else {
      used_motor_ramp = motor_ramp;
    }
   
  }
  
    
  used_motor_ramp_in = used_motor_ramp;
  used_motor_ramp_out = used_motor_ramp;
  
    
  // avoid divisons by zero in the next step
  if (used_motor_ramp == 0) used_motor_ramp = 1;

  // calculate the ramp delta from min speed to max speed
  motor_calculate_ramp_delay_delta(true, used_motor_ramp);
  
  // Set the stepper direction 
  motor_set_direction(isBit(motor_status, BIT_3));
         
  // calculate the amount of steps per millisecond we have to 
  // do in continuous mode
  steps_per_milli = (float) motor_steps / (float) cycle_length; 
  
  continuous_steps_done = 0; 
  
}



// ===================================================================================
void motor_calculate_ramp_delay_delta(boolean amplify, unsigned int used_motorRamp) {
  
  // calculate some initial values needed for the ramping curve:
  // if the distance to be moved is shorter than the default ramp, 
  // get the factor how much it is shorter (0 >= n <= 1).
  // Use another ramp for SM moves and manual jogging here:
  if (ui_is_cursor_in_move_section() && 
      !motor_is_program_starting_flag()) {
    speed_fac = 1; 
    
  } else if (motor_steps < motor_ramp) {
    speed_fac = (float) motor_steps / (float) motor_ramp;
    
    // and slightly amplify it for better performance with the arc-tan curve
    if (amplify) {
      speed_fac = sqrt(sqrt(speed_fac));
    }
    
  } else {
    speed_fac = 1;
  }
    
  // multiply the new speed delta with our factor
  delay_delta = speed_fac * (float) (motor_step_delay_max - motor_step_delay_min);

}




// ===================================================================================
void motor_calculate_steps() {
 
  // if in continuous:
  if (core_is_mode_timelapse())
  {
  
    if (core_is_style_continuous()) motor_convert_speed_to_distance();
    
  } 
  
  
  // update the cycle distance
  core_calculate_cycle_distance();
    
  // calculate how many steps we are going to travel during one cycle
  motor_steps = motor_getCalibrationValue() * motor_cycle_distance; 
  
  
  
}


// ===================================================================================
void motor_convert_speed_to_distance() {
  // distance per cycle = speed per second * cycle length
  motor_cycle_distance = motor_speed * ((float) cycle_length / 1000.0);  
}

// ===================================================================================
void motor_convert_distance_to_speed() {
  // speed per second = distance per cycle / cycle length
  motor_speed = motor_cycle_distance / ((float) cycle_length / 1000.0);
}


// ===================================================================================
void motor_add_steps_to_home_counter(long step_count) {
  
  // add the steps to the home variables
  // depending on the motor direction
  if (isBit(motor_status, BIT_3)) {
    motor_home_steps -= step_count;
  } else {
    motor_home_steps += step_count;
  }  
  
}


// ===================================================================================
boolean motor_move_to_position(long targetPos) {
  
  // calculate the distance ro the target position
  long steps_to_target = motor_home_steps - targetPos;      
  
  // set the step values for the motor 
  motor_steps = abs(steps_to_target);
  
   
  boolean result;
  boolean direction_old;
   
  // remeber the old direction
  direction_old = isBit(motor_status, BIT_3);;
     
  // set the correct direction to move back (home steps can be negative)
  if (steps_to_target > 0) {
    setBit(motor_status, BIT_3);   
  } else {
    deleteBit(motor_status, BIT_3); 
  }
  
  // set the default ramping
  motor_ramp = DEF_MOTOR_RAMP_STANDARD;
   
  // prepare
  motor_prepare_values(false, false);
    
  
  // move
  if (motor_move(DEF_RAMPING_ATANGENS)) {
    result = true;
  } else {
    result = false;  
  }
   
  // set the motor to sleep
  if (!motor_is_keep_powered()) {
    digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
  }      
  
  // set the old direction
  motor_set_direction(direction_old);
  
  return result;  
}



// ======================================================================================
// returns true if no key was pressed and false if the motor phase should be stopped by
// a SELECT-key press
// ======================================================================================
boolean motor_do_key_handling(byte key) {
  
  // if backlight is allready on, check which key was pressed
  if (ui_is_backlight_flag()) {
   
    // SELECT key pressed?
    if (key & B10000000) {
              
      motor_stop_phase();
      
      // set the repaint flag
      ui_set_repaint_flag();
      ui_set_full_repaint_flag();
      
      // leave the motor functions...
      return false;
    
    }
    
  }
  
  // enable the backlight 
  ui_trigger_backlight();
  
  return true;

}


// =====================================================================================
void motor_set_direction(boolean dir) {
  
  if (dir) {
    setBit(motor_status, BIT_3); 
    digitalWrite(DEF_MOTOR_DIR_PIN, HIGH);    
  
  } else {
    
    deleteBit(motor_status, BIT_3); 
    digitalWrite(DEF_MOTOR_DIR_PIN, LOW);  
  
  }
  
}


// ======================================================================================
void motor_continuous_check() {
  
  if (isBit(motor_status, BIT_0)) {
    
    deleteBit(motor_status, BIT_0);
    
    // and remember the start time
    program_start_time = millis();
  }
  
  // if the programm is running and we are in contiuous mode
  if (
      (system_mode == DEF_MODE_TIMELAPSE) && 
      (system_move_style == DEF_MOVE_STYLE_CONTINUOUS) &&
      core_is_program_running() && 
      !core_is_start_phase()
     ) { 
    
    // do some continuous steps
    motor_move_continuous(); 
  }
} 

