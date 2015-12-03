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


// needed for the bouncing mode - this stores
// the original motor direction so that it can be reset
// after the program stops
boolean core_original_direction;



  
// ===================================================================================
boolean core_is_start_immediately()       { return isBit(action_status, BIT_5); }  
boolean core_is_program_running()         { return isBit(action_status, BIT_3); }    

boolean core_is_start_phase()             { return isBit(action_status, BIT_7); }

boolean core_is_style_SMS()               { return (system_move_style == DEF_MOVE_STYLE_SMS); }
boolean core_is_style_continuous()        { return (system_move_style == DEF_MOVE_STYLE_CONTINUOUS); }

boolean core_is_mode_timelapse()          { return (system_mode == DEF_MODE_TIMELAPSE); }
boolean core_is_mode_video()              { return (system_mode == DEF_MODE_VIDEO); }



// ===================================================================================
boolean core_start_program() {
  
  // set the flag that indicates that the start phase is active
  setBit(action_status, BIT_7);
  
  // Set the new direction 
  core_original_direction = isBit(motor_status, BIT_3);
  
  // delete the start flag that might be set
  com_delete_start_flag();
  
  // delete the com stop flag that might be set
  com_delete_stop_flag();
  
  // delete the com sync flag that might be set
  com_delete_sync_flag();
  
    
  
  // are all keyframes set? (if in keyframes setup mode)
  if (ui_is_setup_style_keyframes()) {
     
    // check the slaves if in chaining mode 
    boolean keyFrameError = false;
    
    if ( com.isMaster() &&
        (com.getSlaveCount() > 0) ) {
      
      for (int i=0; i < com.getSlaveCount(); i++) {
        if (!(com_slave_ui_data[i] & B10000000)) { keyFrameError = true; break; }
        if (!(com_slave_ui_data[i] & B01000000)) { keyFrameError = true; break; }   
      }
    }
 
    if (!motor_is_key_start_set() || 
        !motor_is_key_end_set()   ||
        keyFrameError) {
    
      ui_init_motor_key_not_all_points_set_message();
      
      // remove the start-phase flag
      deleteBit(action_status, BIT_7);
  
      return false;  
    }      
  } // end: setup style keyframes

    
  // show start message & delay for one second
  ui_init_engine_start_message();
  ui_print_message(); 
 

  // do some daisy chaining stuff
  if (com.isMaster()) {
    
    // set the start immediately flag 
    action_status |= B00100000;   
    
    // select the master first if we had a slave selected
    if (com_slave_pos != 0) {
      
        com_slave_pos = 0;
        com.deselectSlave();
        com_restore_master_settings();
    }
    
    // send the current system mode to all clients
    com_send_system_mode(true);
    
    // send the setup data (record time, playback time, fps) 
    com_send_sms_run_data();
    
    // prepare all slaves for the program
    core_prepare_slaves();
            
    // Move to start position without a message to the user 
    core_move_motor_to_start_pos(false);
        
        
  } // end: we are a master 
  
  
  // we are a slave
  else {
    
    boolean oldDir = isBit(motor_status, BIT_3);  
    
    // Move to start position without a message to the user 
    core_move_motor_to_start_pos(false);
    
    motor_set_direction(oldDir);
      
    // send a done command and wait for acknowledge of the master
    com_send_done_with_ack();
        
  }
  
  
  // set the focus pin high for some time to allow the camera to wake up
  digitalWrite(DEF_CORE_FOCUS_PIN, HIGH);
  core_delay(150);
  digitalWrite(DEF_CORE_FOCUS_PIN, LOW);
      
  
  
  // clear the shoot counter
  camera_shoot_count = 0; 
  
  // prepare the HDR values
  camera_prepare_HDR_values(); 
  
  // set a special flag to do the motor
  // preparation for the program and not in the
  // menu and manual move context
  motor_set_program_starting_flag();  
  
  // re-calculate the keyframe basics,
  // also for setting the correct direction
  if (ui_is_setup_style_keyframes()) {
    if (core_is_mode_video()) core_calc_keyframe_video_values();
    else                      core_calc_keyframe_timelapse_values();
  }

  // doo all the motor preparation...  
  if (core_is_mode_video()) {
    // prepare everything for the motor
    motor_prepare_values(true, false);

  } else {
    
    // prepare everything for the motor
    motor_prepare_values(true, true);
    
    if (system_move_style == DEF_MOVE_STYLE_CONTINUOUS) {
      // set the flag for re-setting the start time 
      // when the cont-function is called the first time
      setBit(motor_status, BIT_0);
    }
    
  } 
      
  // all motor values are done so we can remove the 
  // special flag to do the motor preparation for the program  
  motor_delete_program_starting_flag();
    
  // start the motor if wanted:
  if (motor_is_set_distance()) {
    // wake up the motor
    digitalWrite(DEF_MOTOR_SLEEP_PIN, HIGH);
  } 
  
  
  // if we are the master, do the start delay
  if (com.isMaster()) {
    
    core_delay(DEF_SYSTEM_START_DELAY);
    
    // send the actual start command
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_START);
  } 
  
  // if we are a slave, wait for the start signal
  else { 
    
    byte pKey = KEY_NONE;
    
    // wait until we receive the initial sync signal...
    // (only as long as no key is pressed)
    while ((!com_is_sync_flag()) && 
           (pKey == KEY_NONE)) {
      
      com.executeCommunication(); 
      pKey = keys_get_key_pressed();
    }
    
    
    // check if the program was started by a manual override
    if (pKey == KEY_SELECT) {
      com_set_manual_override_start_flag();
    }
            
  }
  
  // delete some flags that might cause trouble
  com_delete_sync_flag();
  //core_delete_stop_motion_wait();
  camera_delete_is_in_HDR_delay();
  camera_delete_is_exposing();
    
  
  // turn on the programm-running flag 
  action_status |= B00001000;   
    
  // fully repaint the user interface (home screen)
  ui_go_to_main_screen();
  ui_set_repaint_flag();
  ui_set_full_repaint_flag();
  ui_do_screen();
  
  
  // and remember the start time
  program_start_time = millis();
  
  // remove the start-phase flag
  deleteBit(action_status, BIT_7);
    
  return true;  
}




// ===================================================================================
void core_stop_program(boolean all, boolean message) {
  
  // if we are asked to stop all programs of all slaves, do it
  if (com.isMaster() && all) {
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_STOP);
  }
  
  // turn off the programm-running flag and all other flags too.
  action_status = B0;
  
  // if we want a "program done" message
  if (message) {
    
    // eneable the message painting
    ui_set_repaint_flag();  
    ui_set_full_repaint_flag();
    
    // go the the program-done screen if master
    if (com.isMaster()) {
      ui_set_program_done_screen();  
    }
    
  }
  
  
  // and just in case turn off the "real" camera exposure
  digitalWrite(DEF_CORE_CAMERA_PIN, LOW);
  
  // and just in case turn off the "real" camera focus
  digitalWrite(DEF_CORE_FOCUS_PIN, LOW);

  // set the motor-step to sleep
  digitalWrite(DEF_MOTOR_STEP_PIN, LOW);

  // wait a little time to let the motor settle and break  
  delay (DEF_MOTOR_BREAK_DELAY);
  
  // delete all possible flags and go back to the main screen
  com_delete_stop_flag();
  com_delete_sync_flag();
  com_delete_manual_override_start_flag();
  ui_delete_message_on_screen_flag();
  ui_delete_message_in_queue_flag();
    
    
  // restore the original motor direction
  motor_set_direction(core_original_direction);
      
  // set the repaint flags to force a full repaint of the ui
  ui_set_repaint_flag();
  ui_set_full_repaint_flag(); 
  
  // do the screen stuff
  ui_do_screen();
  
  // enable the backlight if not already enabled
  ui_trigger_backlight();
    
}

// ===================================================================================
// returns true if the next cycle needs to start
// ===================================================================================
boolean core_is_next_cycle() {
  
  return (program_start_time + (cycle_length * camera_shoot_count)) <= millis();
  
}




// ===================================================================================
// prepares all registered slaves for the program start
// ===================================================================================
void core_prepare_slaves() {
    
  // if we have no slaves, leave immediately
  if (com.getSlaveCount() == 0) return;
    
  // our main done flag
  boolean done = false;
   
  // temp variables for storing the received data
  byte command, id;
    
  // set all variables in our slave status array to false
  for (int i=0; i<com.getSlaveCount(); i++) {
    deleteBit(com_slave_ui_data[i], BIT_0);
  }
  
  // send the prepare command to all slaves
  com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_PREPARE); 
  
    
  // loop until we are done
  while (!done) {
    
    // check if we received something from the slaves
    // --> gets processes in the com_handle_events()
    com.executeCommunication();
        
    // set the main-loop-flag to true
    done = true;
    
    // loop the whole array and check if we are ready
    // (reset the main flag to false if not)
    for (int i=0; i<com.getSlaveCount(); i++) {
      
      byte res = isBit(com_slave_ui_data[i], BIT_0);
           
      // if the bit is not set
      if (res == 0) {
        done = false;
      }
      
    }
    
    // leave on key press
    if (keys_get_key_pressed() == KEY_SELECT) {
      return;  
    }
    
  } // end: main loop
    
}



// ===================================================================================
void core_check_if_timelapse_is_done() {
  
  // how much time is elapsed since the program start?
  uint32_t elapsed_time = millis() - program_start_time;
  
  // is this longer than the theroretical program run time?
  if (elapsed_time >= (camera_shot_max * cycle_length) - 300) {
    
    motor_continuous_check();
    
    // if we are in bouncing mode, reset all major variables so that
    // the program starts again but in reverse direction:
    if (motor_is_looping()) {
    
      // reset the camera shoot count to 1
      camera_shoot_count = 1;
      
      motor_continuous_check();
      
      // invert the motor direction
      boolean new_dir = !isBit(motor_status, BIT_3);
      motor_set_direction(new_dir);
     
      // reset the continuos motor steps done
      motor_reset_continuous_steps();      
     
      // calculate the new perfect program start time
      program_start_time = program_start_time + (cycle_length * camera_shot_max);
            
      motor_continuous_check();
                          
    } // end: in bouncing mode
    
    else {
      
      // no bouncing? then stop the program
      core_stop_program(com.isMaster(), false);
      
    }
    
  }
 
}




// ===================================================================================
// This function does all the thing needed after the move style was changed
// ===================================================================================
void core_move_style_change() {

  // transfer the motor speed/distance values:
  // now in continuous mode
  if (core_is_mode_timelapse() && core_is_style_continuous()) {
    
    if (ui_is_setup_style_keyframes()) core_calc_keyframe_timelapse_values();
    else                               motor_convert_distance_to_speed();
  } 
    
  // now in a S-M-S mode
  else if (core_is_mode_timelapse() && core_is_style_SMS()) { //|| core_is_mode_stop_motion()) {
    
    if (ui_is_setup_style_keyframes()) core_calc_keyframe_timelapse_values();     
    else                               motor_convert_speed_to_distance();
  
  }
    
  // now in video mode
  else if (core_is_mode_video()) {
      
    //if (ui_is_setup_style_cycle()) {
    ui_set_setup_style(DEF_SETUP_STYLE_RUN);  
    //}
      
    if (ui_is_setup_style_keyframes()) core_calc_keyframe_video_values();       
    else                               motor_convert_speed_to_distance();
    
  }  
  
}



// ===================================================================================
// calculates the new values for the new setup style (from the old ones..)
// ===================================================================================
void core_caclulate_setup_style_changes() {
    
  
  // if continuous, call this to get the settings from continuous to SMS
  if (core_is_mode_timelapse() && core_is_style_continuous()) {
    motor_convert_speed_to_distance();
  }
  
  // now in run based
  if (ui_is_setup_style_run()) {

    // calculate the distance to move for one cycle
    core_calculate_cycle_distance();
    
    // recalculate shot #
    core_calculate_shot_number();
    
    // recalculate the cycle length (interval)
    core_calculate_interval();
  }
      
  // if continuous, call this to send the settings from SMS to continuous
  if (core_is_mode_timelapse() && core_is_style_continuous()) {
    motor_convert_distance_to_speed();
  }
  
  // if we are in keyframes mode now
  if (ui_is_setup_style_keyframes()) {
    
    if (core_is_mode_video()) {
      
      core_calc_keyframe_video_values();
    
    } else {
    
      core_calc_keyframe_timelapse_values();
      
    }
  
  }
  
}


// ===================================================================================
void core_calculate_shot_number() {
  
  // recalculate shot #
  camera_shot_max = run_play_time * (unsigned long) run_fps;
  
}

// ===================================================================================
void core_calculate_interval() {

  // recalculate the cycle length (interval)
  cycle_length = ((float) run_record_time / (float) camera_shot_max) * 1000;  
  
  //cycle_length = round (((float) run_record_time / (float) (camera_shot_max - 1)) * 10.0);  
  
  
  // check if the exposure value is longer than the current cycle length
  if (camera_exp_time > cycle_length) {
    camera_exp_time = cycle_length;
  }    
  
}

// ===================================================================================
void core_calculate_cycle_distance() {
  
  // refresh the shot number
  core_calculate_shot_number();
  
  // calculate the actual motor-cycle distance
  motor_cycle_distance = (float) run_dist / (float) camera_shot_max;
  
  // if continuous, call this to send the settings from SMS to continuous
  if (core_is_mode_timelapse() && core_is_style_continuous()) {
    motor_convert_distance_to_speed();
  }
  
}





// ===================================================================================
// calculates the basic value from the keyframes data
// ===================================================================================
float core_calculate_keyframe_basics() {
  
  long dist = motor_key_start - motor_key_end;
    
  // Set the new direction 
  if (dist < 0) {
    motor_set_direction(false);
  } else {
    motor_set_direction(true);
  }
  
  float cal = motor_getCalibrationValue();
  
  return ((float)dist / cal);
  
}


// ===================================================================================
// calculates and transfers the keyframes to "real" data (for the video mode)
// ===================================================================================
void core_calc_keyframe_video_values() {
  
  float dist = core_calculate_keyframe_basics();  
  
  motor_video_dist = abs(dist);  
  
}



// ===================================================================================
// calculates and transfers the keyframes to "real" data (for all timelapse-modes)
// ===================================================================================
void core_calc_keyframe_timelapse_values() {
  
  float dist = core_calculate_keyframe_basics();  
  
  // set the distance
  run_dist = abs(dist);
  
  // calculate the distance to move for one cycle; 
  core_calculate_cycle_distance();
    
  // transfer the distance into continuous mode too
  motor_convert_distance_to_speed();
  
}




// ===================================================================================
// this function sets the start-keyframe to where the motor stands right now
// ===================================================================================
void core_set_start_keyframe() {
 
  // set start keyframe to the current position   
  motor_key_start = motor_home_steps;
  motor_set_key_start_flag();
                  
  // calculate and check the needed values
  if (core_is_mode_video()) core_calc_keyframe_video_values();
  else                      core_calc_keyframe_timelapse_values();
 
                                      
  // show the keyframe is set message
  ui_init_motor_key_set_message(); 
  ui_set_repaint_flag(); 
  ui_set_full_repaint_flag(); 
  
 
}

// ===================================================================================
// this function sets the end-keyframe to where the motor stands right now
// ===================================================================================
void core_set_end_keyframe() {
  
  // set end keyframe to the current position   
  motor_key_end = motor_home_steps;
  motor_set_key_end_flag();
                 
  // calculate and check the needed values
  if (core_is_mode_video()) core_calc_keyframe_video_values();
  else                      core_calc_keyframe_timelapse_values();
                  
  // show the keyframe is set message
  ui_init_motor_key_set_message(); 
  ui_set_repaint_flag();
  ui_set_full_repaint_flag(); 

}



// ===================================================================================
// this function moves the motor to the start position
// ===================================================================================
void core_move_motor_to_start_pos(boolean showMessage) {
  
  // in keyframes mode?
  if (ui_setup_style == DEF_SETUP_STYLE_KEYFRAMES) {
  
    // is a start point defined?
    if (!motor_is_key_start_set()) { 
      ui_init_motor_key_not_set_message();     
      return;  
    }
         
    // are we supposed to show the user message?
    if (showMessage) {  
      ui_init_motor_key_move_to_message();
      ui_print_message();
    }
                    
    // move motor to start keyframe with error check  
    if (motor_move_to_position(motor_key_start)) {
      
      // init the "key reached " message and print it
      if (showMessage){
        ui_init_motor_key_reached_message();
      }
      
      // moving home can take some time so simulate 
      // an empty user action to keep 
      // the system awake for further actions...
      interaction_time = millis();
                      
    } else if (showMessage) {
      
      // end the motor home message  
      ui_end_message();  
          
    }    
  
  } // end: keyframe mode
  
}


// ===================================================================================
// this function moves the motor to the end position
// ===================================================================================
void core_move_motor_to_end_pos() {

  // is a start point defined?
  if (!motor_is_key_end_set()) { 
    ui_init_motor_key_not_set_message();     
    return;
  }
                  
  ui_init_motor_key_move_to_message();
  ui_print_message();
                  
  // move motor to home with error check  
  if (motor_move_to_position(motor_key_end)) {
  
    // init the "home reached " message and print it
    ui_init_motor_key_reached_message();
    //print_message(); 
    
    // moving home can take some time so simulate 
    // an empty user action to keep 
    // the system awake for further actions...
    interaction_time = millis();
    
  } else {
    
    // end the motor home message  
    ui_end_message();  
  }       

}






// ===================================================================================
void core_delay(unsigned int milliseconds) {
  
  unsigned long startTime = millis();

  while ((startTime + milliseconds) > millis()) {
    
     // if the programm is running and we are in contiuous mode
     motor_continuous_check(); 
     
     // do the communication stuff
     com.executeCommunication();
  } 
    
}


// ===================================================================================
void core_check_sleep() {
  
  if (ui_is_system_sleep()) {
  
    if (!core_is_program_running()) {
    
      if ((interaction_time + DEF_SYSTEM_SLEEP_DELAY) < millis()) {
      
        ui_disable_backlight();
        GLCD.ClearScreen();
        core_sleep_now();  
      
      }  
      
    }
    
  }
  
}

// ===================================================================================
void core_sleep_now() {
  
  // needed delay to let all other function end
  core_delay(100);
  
  // sleep mode is set here
  set_sleep_mode(SLEEP_MODE_IDLE);   

  // enables the sleep bit in the mcucr register
  // so sleep is possible. just a safety pin 
  sleep_enable();                    
  
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_timer3_disable();
  power_timer4_disable();
  power_timer5_disable();
  power_twi_disable();
  
  // Serial 2 needs to stay awake 
  // to allow Serial communication during sleep
  power_usart0_disable();
  power_usart1_disable();  
  power_usart3_disable();  
    
  // here the device is actually put to sleep!!
  sleep_mode();            
 
  // z z z z z z z z z z z z z z Z Z Z Z Z Z Z Z Z Z 
  // -----------------------------------------------
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP        
  
  // first thing after waking from sleep:  
  sleep_disable();         
  
  power_all_enable();
  
  if (keys_key_pressed()) {
        
    // set the repaint flag
    ui_set_repaint_flag();
    ui_set_full_repaint_flag();  
  }
    
}



// ======================================================================================
// Function to restart the Arduino
// ======================================================================================
//reset_arduino() function:
void(* reset_arduino) (void) = 0; //declare reset function @ address 0






