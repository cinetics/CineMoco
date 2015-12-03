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


// the variables to periodically check if settings were changed remotely 
#define   SAVE_CHECK_DELAY  5000
uint32_t  com_save_time_check;


// when browsing the available motors - the current position in the motor list
// (salve 0 = master; salve 1...n = slaves array)
byte  com_slave_pos = 0;
byte  com_slave_motor_pos = 0;

byte com_key = KEY_NONE;


// the communication-status byte
// B7 = sync signal received      B1000 0000
// B6 = program stop flag         B0100 0000
// B5 = manual override start     B0010 0000
// B4 = start signal              B0001 0000
// B3 = acknowledge received      B0000 1000
// B2 = 
// B1 = 
// B0 = settings were changed     B0000 0001
byte com_status = B00000000;




// an array to store some ui values for every slave
// B7 = start point is set        B1000 0000
// B6 = end point is set
// B5 = keep pwrd is enabled
// B4 = 
// B3 = 
// B2 = 
// B1 = 
// B0 = slave is ready             B0000 0001
byte com_slave_ui_data[MOCOM_MAX_SLAVES];


// remote settings store
float com_motor_video_dist;
unsigned long com_camera_exp_time;
byte com_motor_video_speed;
byte com_motor_video_ramp_in;
byte com_motor_video_ramp_out;
boolean com_motor_looping;
boolean com_motor_powered;

byte com_motor_calibration_index;            
byte com_run_distance;
byte com_motor_direction;
byte com_motor_ramp_percent;



// ===================================================================================

void    com_set_sync_flag()                      { setBit(com_status, BIT_7); }
boolean com_is_sync_flag()                       { return isBit(com_status, BIT_7); }
void    com_delete_sync_flag()                   { deleteBit(com_status, BIT_7); } 

void    com_set_start_flag()                     { setBit(com_status, BIT_4); }
boolean com_is_start_flag()                      { return isBit(com_status, BIT_4); }
void    com_delete_start_flag()                  { deleteBit(com_status, BIT_4); } 

void    com_set_stop_flag()                      { setBit(com_status, BIT_6); }
boolean com_is_stop_flag()                       { return isBit(com_status, BIT_6); }
void    com_delete_stop_flag()                   { deleteBit(com_status, BIT_6); } 

void    com_set_manual_override_start_flag()     { setBit(com_status, BIT_5); }
boolean com_is_manual_override_start_flag()      { return isBit(com_status, BIT_5);}
void    com_delete_manual_override_start_flag()  { deleteBit(com_status, BIT_5); }





// ===================================================================================
// this function initialized the communication
// ===================================================================================
void com_init() {

  com.init(57600);  
  //com.init(38400);  
        
  com.registerFunction_CommunicationReceived(com_handle_events);  
  com.registerFunction_TimeCritical(motor_continuous_check);
  com.setID(com_id);

}


// ===================================================================================
// process the communication
// ==================================================================================
void com_process() {
  
  // do some the communication stuff
  com.executeCommunication();
  
  // is the start - flag is set:
  if (com_is_start_flag()) core_start_program(); 
  
  // check if we need to save any settings
  if ((com_save_time_check + SAVE_CHECK_DELAY) < millis()) {
    
    com_check_autosave_settings();
    com_save_time_check = millis();
    com.executeCommunication();
  }
  
}



// ===================================================================================
// checks if the com-internal "save settings" flag is set...
// ===================================================================================
void com_check_autosave_settings() {
  
  // check if we need to save changed settings to the EEPROM
  if (isBit(com_status, BIT_0) && 
      ui_is_settings_autosave()) {
    
    // save the settings
    eeprom_write_config();
    
    // delete the com-internal "save settings" flag
    deleteBit(com_status, BIT_0);

  }  
  
}


// ===================================================================================
// this function handles the communication events
// ===================================================================================
void com_handle_events() {
    
  byte command  = com.getLastReceivedCommand();
  byte senderId = com.getLastSenderID();
  
  
  // ---------------------
  // start the engine
  if ((command == MOCOM_COMMAND_START) &&
      (com.isMaster() == false)) {
    
    // set the sync received flag
    com_set_sync_flag();
    //core_delete_stop_motion_wait();

    return;
  }
  
  // ---------------------
  // camera shoot
  if ((command == MOCOM_COMMAND_CAMERA_SHOOT) &&
      (com.isMaster() == false)) {
    
    // set the sync received flag
    com_set_sync_flag();
    //core_delete_stop_motion_wait();
    return;
  }
  
  
  // ---------------------
  // initialize the start of the engine
  if ((command == MOCOM_COMMAND_PREPARE) &&
      (com.isMaster() == false)) {
        
    if (core_is_program_running()) core_stop_program(false, false); 
    com_set_start_flag();
    return;
  }
  
  
  // ---------------------
  // we received an Acknowledge from the master
  if ((command == MOCOM_COMMAND_ACK) &&
      (com.isMaster() == false)) {
        
    // set the bit that we received an acknowldge    
    setBit(com_status, BIT_3);
    return;
  }
  
  
  // ---------------------
  // stop the engine
  if ((command == MOCOM_COMMAND_STOP) &&
      (com.isMaster() == false)) {
    
    com_set_stop_flag();
    core_stop_program(false, true);
  }
    
    
  // ---------------------
  // motor info was requested by the master
  if (command == MOCOM_COMMAND_GET_MOTOR_DATA) {
    
    com.sendClearMotorData();
    com.addMotor(MOTOR_TYPE_LINEAR);
    com.sendDone();
    
    // delete the probaly already set keyframes;
    motor_delete_key_start_flag();
    motor_delete_key_end_flag();
    
    // send a message to the screen that we registerd in teh chain
    ui_init_registerd_in_chain_message();
    ui_set_repaint_flag();
    ui_set_full_repaint_flag();
  }
  
  
  // ---------------------
  if ((command == MOCOM_COMMAND_KEY) && 
      (com.isMaster() == false)) {
    
    // get the key value that was sent
    com_key = com.getDataByte(0);  
        
    // go to the main screen
    if (ui_go_to_main_screen()) {
      ui_set_repaint_flag();
      ui_set_full_repaint_flag();
      ui_paint_screen(com_key);
    }

  }
  
  
  // ---------------------
  if ((command == MOCOM_COMMAND_KEY_RELEASE) &&
      (com.isMaster() == false)) {
    // delete the key value
    com_key = KEY_NONE;  
  }
  
  
  // ---------------------
  if ((command == MOCOM_COMMAND_DONE) && 
       com.isMaster()) {
    
    // send the acknowledge to the slave
    com.sendCommand(senderId, MOCOM_COMMAND_ACK);  
        
    // deselect the prossilby selected slave
    com.deselectSlave();
            
    // loop all slaves we have registered
    while (com.selectNextSlave()) {
      
      // is the ID of the slave the ID we are looking for?
      if (senderId == com.getSlaveID()) {
        
        setBit(com_slave_ui_data[com.getSelectedSlaveIndex()], BIT_0);
        return;
      }
      
    }
        
  }
  
  
  // ---------------------
  // we received a code command  
  // ---------------------
  else if (command == MOCOM_COMMAND_CODE) {
    
    byte code = com.getDataByte(0); 
      
    switch (code) {
      
      // do a repaint
      case 10 :    {
                    ui_set_repaint_flag();
                    ui_set_full_repaint_flag();
                    break;
                   }
      
      
      // set start keyframe
      case 132 :  {
                    core_set_start_keyframe();
                                        
                    // send the new video distance to the master (needed for ramping limit)
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 230, motor_video_dist); 
                    break;
                  }
                                  
      // set end keyframe
      case 133 :  {
                    core_set_end_keyframe();
                                        
                    // send the new video distance to the master  (needed for ramping limit)
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 230, motor_video_dist); 
                    break;
                  }
      
      // goto start keyframe
      case 134 :  {
                    core_move_motor_to_start_pos(true);
                    break;
                  }
      
      // goto end keyframe
      case 150 :  {
                    core_move_motor_to_end_pos();
                    break;
                  }
        
      
    } // switch (code)
    
  } // command = code
  
  
  
  // ---------------------
  // we were asked for data
  // ---------------------
  else if (command == MOCOM_COMMAND_GET_DATA) {
    
    byte code = com.getDataByte(0);   

    switch (code) {
      
      // direction
      case 23 :   {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 23, (byte) isBit(motor_status, BIT_3));
                    break;
                  }
                  
      // run distance
      case 30 :   {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 30, (float) run_dist);
                    break;
                  }
                  
      // camera exposure time
      case 36 :   {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 36, (long) camera_exp_time);
                    break;
                  }
      
      // motor calibration (motor type)
      case 45 :   {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 45, motor_calibration_index);
                    break;
                  }            
                  
      // ramp percent
      case 56 :   {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 56, (byte) motor_ramp_percent);
                    break;
                  }
                  
      // video speed
      case 124 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 124, motor_video_speed);
                    break;
                  }
                  
      // video ramp in
      case 125 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 125, (byte) motor_video_ramp_in);
                    break;
                  }
      
      // video ramp out
      case 126 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 126, (byte) motor_video_ramp_out);
                    break;
                  }
                  
      // video distance
      case 127 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 126, (float) motor_video_dist);
                    break;
                  }
                  
      // keep motor powered
      case 149 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 149, (byte) motor_is_keep_powered());
                    break;
                  }
                  
      // video bouncing
      case 156 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 156, motor_is_looping());
                    break;
                  }
                                    
      // video moce distance
      case 230 :  {
                    com.sendCommand(MOCOM_MASTER_ID, MOCOM_COMMAND_DATA, 230, motor_video_dist);
                    break;
                  }
    
    } // swtich (code)
    
  } // command = get data
  
  
  
  
  // ---------------------
  // we received data
  // ---------------------
  else if (command == MOCOM_COMMAND_DATA) {
    
    byte code = com.getDataByte(0);   
        
    switch (code) {
      
      // move style
      case 7 :    {
                    // set the new move style
                    system_move_style = com.getDataByte(1); 
                                       
                    // do the things needed after a move style change
                    core_move_style_change();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
      
      
      // direction
      case 23 :   {
                    boolean dir = (boolean) com.getDataByte(1); 
                    motor_set_direction(dir);
                                        
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
      
      
      
      // move style
      case 25 :   {
                    // set the new move style
                    system_mode = com.getDataByte(1); 
                                       
                    // do the things needed after a move style change
                    core_move_style_change();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // run distance
      case 30 :   {
                    
                    run_dist = com.getDataFloat(1);
                    
                    // calculate the distance to move for one cycle; 
                    core_calculate_cycle_distance();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // run record time
      case 32 :   {
                    run_record_time = (unsigned long) com.getDataLong(1);
                    // recalculate the main run-values
                    core_calculate_interval();
                    core_calculate_cycle_distance();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                                        
                    break;
                  }
                  
      // run record time
      case 33 :   {
                    run_play_time = (unsigned long) com.getDataLong(1);
                    // recalculate the main run-values
                    core_calculate_shot_number();
                    core_calculate_interval();
                    core_calculate_cycle_distance();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // run record time
      case 34 :   {
                    run_fps = com.getDataByte(1);
                    // recalculate the main run-values
                    core_calculate_shot_number();
                    core_calculate_interval();
                    core_calculate_cycle_distance();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // camera exposure time
      case 36 :   {
                    camera_exp_time = (unsigned long) com.getDataLong(1);
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                                    
      // setup style
      case 54 :   {
                    ui_setup_style = com.getDataByte(1); 
                    
                    // do the things that need to be done after a setup-style change
                    core_caclulate_setup_style_changes();
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // ramp percent
      case 56 :   {
                    motor_ramp_percent = (unsigned byte) com.getDataByte(1); 
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                                    
      // video speed
      case 124 :   {
                    motor_video_speed = com.getDataByte(1);
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // video ramp-in
      case 125 :   {
                    motor_video_ramp_in = com.getDataByte(1);
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
      
      // video ramp-out
      case 126 :   {
                    motor_video_ramp_out = com.getDataByte(1);
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // video total distance
      case 127 :   {
                    
                    motor_video_dist = com.getDataFloat(1);
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // keep motor powered
      case 149 :   {
                    boolean keepPowered = (boolean) com.getDataByte(1);
                    
                    if (keepPowered) {
                      
                      motor_set_keep_powered_flag();
                      // enable the motor (motor wakeup)
                      digitalWrite(DEF_MOTOR_SLEEP_PIN, HIGH);
                    
                    } else {
                      
                      motor_delete_keep_powered_flag();
                      // enable the motor (motor wakeup)
                      digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
                    
                    }
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
      // bouncing
      case 156 :   {
                    boolean bouncing = (boolean) com.getDataByte(1);
                    
                    if (bouncing) {
                      setBit(motor_key_status, BIT_2);
                    } else {
                      deleteBit(motor_key_status, BIT_2); 
                    }
                    
                    // set the com internal "settings were changed" flag
                    setBit(com_status, BIT_0);
                    
                    break;
                  }
                  
                  
      // motor video distance
      case 230 :   {
                    
                    motor_video_dist = com.getDataFloat(1);
                    break;
                  }
        
                  
      
    }
    
    // repaint the screen 
    ui_set_repaint_flag();
    ui_set_full_repaint_flag();
    
  }  
    
  

}






// ===================================================================================
// sends a done to the master until an acknowledge command is received
// ===================================================================================
void com_send_done_with_ack() {
    
  // create a random delay for avoiding sending at the same interval
  int randDelay = 0;
  
  // the time variable to remember when we sent the "done" command
  uint32_t timeSent = 0;
  
  // loop until we jump out of this function
  while (1) {
    
    // do we need to send a new mail?
    if ((timeSent + randDelay) < millis()) {
      
      // send a new done command
      com.sendDone();
      
      // remember the tine
      timeSent = millis();
      // generate a new random delay       
      randDelay = random(500);  
    }
    
    // process the communication
    // --> gets processes in the com_handle_events()
    com.executeCommunication();    
    
    // did we receive the acknowledge from the master?
    if (isBit(com_status, BIT_3)) {
      
      deleteBit(com_status, BIT_3);
      return;   
    } 
    
    // leave on key press
    if (keys_get_key_pressed() == KEY_SELECT) {
      return;  
    }
    
  } // end: eternal loop
   
}



// ===================================================================================
// this function returns if a send-key command was received
// ===================================================================================
byte com_get_received_key() {
    return com_key;
}

// ===================================================================================
// this function sends the pressed key to the slave
// ===================================================================================
void com_send_key(byte key, boolean all) {
  
  if (all) {
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_KEY, key);
  } else {
    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_KEY, key);
  }
      
}

// ===================================================================================
// this function sends the key-release to the slave
// ===================================================================================
void com_send_key_release(boolean all) {
  
  if (all) {
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_KEY_RELEASE);    
  } else {
    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_KEY_RELEASE);    
  }  
   
}

 


// ===================================================================================
// this function sets the start points 
// ===================================================================================
void com_set_start_keyframe(boolean all) {
  
  if (all) {
    
    for (int i=0; i < com.getSlaveCount(); i++) {
      
      // set the flag for this slaves start pos
      com_slave_ui_data[i] |= B10000000;
    }
    
    // send the command to set the start keyframe
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_CODE, (byte) 132);
    
  } else {
    // set the flag for this slaves start pos
    com_slave_ui_data[com_slave_pos - 1] |= B10000000;
    
    // send the command to set the start keyframe
    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_CODE, (byte) 132);
  }
 
}


// ===================================================================================
// this function sets the end points 
// ===================================================================================
void com_set_end_keyframe(boolean all) {
  
  if (all) {
    
    for (int i=0; i < com.getSlaveCount(); i++) {
      // set the flag for this slaves end pos
      com_slave_ui_data[i] |= B01000000;
    }
    
    // send the command to set the end keyframe
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_CODE, (byte) 133);
    
  } else {
    // set the flag for this slaves end pos
    com_slave_ui_data[com_slave_pos - 1] |= B01000000;
    
    // send the command to set the end keyframe
    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_CODE, (byte) 133);
  }
 
}


// ===================================================================================
// this function sends a code to the curretly selected or to all slaves
// ===================================================================================
void com_send_command_code(byte code, boolean all) {
  
  if (all) {
    
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_CODE, (byte) code);
    
  } else {

    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_CODE, (byte) code);
  }
 
}


// ===================================================================================
// this function sends a code plus a data-byte to the curretly selected,
// or to all slaves
// ===================================================================================
void com_send_command_code(byte code, byte data, boolean all) {
  
  if (all) {
    
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_CODE, (byte) code, (byte) data);
    
  } else {

    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_CODE, (byte) code, (byte) data);
  }
 
}


// ===================================================================================
// this function sends a code plus a data-byte to the curretly selected,
// or to all slaves
// ===================================================================================
void com_send_command_data(byte code, byte data, boolean all) {
  
  if (all) {
    
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA, (byte) code, (byte) data);
    
  } else {

    com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, (byte) code, (byte) data);
  }
 
}



// ===================================================================================
// this function gets if the clients startpoint was set or not
// ===================================================================================
boolean com_is_start_keyframe_set(boolean all) {
  
  if (com_slave_pos == 0) {
    return motor_is_key_start_set();
  } else {
    
    if (all) {
      
      for (int i=0; i < com.getSlaveCount(); i++) {
        if (!(com_slave_ui_data[i] & B10000000)) {
          return false;
        }
      }
      
      if (!motor_is_key_start_set()) {
        return false;
      }
      
      return true;
      
    } else {
      return com_slave_ui_data[com_slave_pos - 1] & B10000000; 
    }
    
        
  }
}



// ===================================================================================
// this function gets if the clients endpoint was set or not
// ===================================================================================
boolean com_is_end_keyframe_set(boolean all) {
  
  if (com_slave_pos == 0) {
    return motor_is_key_end_set();
  } else {
    
    if (all) {
      
      for (int i=0; i < com.getSlaveCount(); i++) {
        if (!(com_slave_ui_data[i] & B01000000)) {
          return false;
        }
      }
      
      if (!motor_is_key_end_set()) {
        return false;
      }
           
      return true;
      
    } else {
      return com_slave_ui_data[com_slave_pos - 1] & B01000000; 
    }
        
  }
}


// ===================================================================================
// sends the repaint-flag to one or all slaves
// ===================================================================================
void com_send_repaint_flag(boolean all) {
  
  if ((com.getSlaveCount() > 0) &&
      (com.isMaster())) {
  
    // send the code for "repaint"
    com_send_command_code(10, all);
 
  }
  
}




// ===================================================================================
// sends the current system mode to one or all slaves
// ===================================================================================
void com_send_system_mode(boolean all) {
    
  if ((com.getSlaveCount() > 0) &&
      (com.isMaster())) {
  
    // send the code for "recored style" to all clients
    com_send_command_data(25, system_mode, all);
    
    // send the code for "move style" to all clients
    com_send_command_data(7, system_move_style, all);
                        
    // set the setup style to the same as we have now
    com_send_command_data(54, ui_setup_style, all);  
  }
  
}





// ===================================================================================
// receive the settings-data from the client
// ===================================================================================
void com_receive_settings() {
  
  if (ui_setup_style == DEF_SETUP_STYLE_KEYFRAMES) {
    
    switch (system_mode) {
      
      case DEF_MODE_TIMELAPSE : {
                            
                            // request the exposure value from the currently
                            // selected slave
                            
                            camera_exp_time = (unsigned long) com.requestDataLong(36);
                            
                            
                            break;
                          }
                          
      case DEF_MODE_VIDEO : {
                                                        
                            // request the video run data from the currently
                            // selected slave
                            motor_video_speed =    com.requestDataByte(124);
                            motor_video_ramp_in =  com.requestDataByte(125);
                            motor_video_ramp_out = com.requestDataByte(126);
                            com_motor_powered =    (boolean) com.requestDataByte(149);
                            boolean bouncing =     (boolean) com.requestDataByte(156);
                                                                                    
                            if (bouncing) {
                              setBit(motor_key_status, BIT_2);
                            } else {
                              deleteBit(motor_key_status, BIT_2);
                            }
                            
                            motor_video_dist =     com.requestDataFloat(230);
                            
                                                                                            
                            break;
                          }
      
    }
    
  } // end: keyframes
    
  else if (ui_setup_style == DEF_SETUP_STYLE_RUN) {
  
    switch (system_mode) {
      
      case DEF_MODE_TIMELAPSE : {
        
                            boolean dir             = (boolean) com.requestDataByte(23);
                            run_dist                = com.requestDataFloat(30);
                            camera_exp_time         = (unsigned long) com.requestDataLong(36);
                            motor_calibration_index = com.requestDataByte(45);
                            motor_ramp_percent      = (unsigned byte) com.requestDataByte(56);
                            motor_set_direction(dir);
                                                        
                            break;
                          }
                          
      case DEF_MODE_VIDEO : {
                            
                            boolean dir             = (boolean) com.requestDataByte(23);  
                            motor_calibration_index = com.requestDataByte(45);
                            motor_video_speed       = com.requestDataByte(124);
                            motor_video_ramp_in     = com.requestDataByte(125);
                            motor_video_ramp_out    = com.requestDataByte(126);
                            motor_video_dist        = com.requestDataFloat(127);
                            motor_set_direction(dir);
                                      
                            break;
                          }
    }
    
  }
  
}



// ===================================================================================
// sends the main setp-data if in an SMS mode
// ===================================================================================
void com_send_sms_run_data() {
  
  if ((com.isMaster()) &&
      (com.getSlaveCount() > 0) &&
    //(ui_setup_style == DEF_SETUP_STYLE_KEYFRAMES) &&
      (system_mode == DEF_MODE_TIMELAPSE)) {    
        
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA,  32, (long) run_record_time);
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA,  33, (long) run_play_time);
    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA,  34, run_fps);
          
  }

}



// ===================================================================================
// buffer the master's setting when going to edit a slave's settings
// ===================================================================================
void com_save_master_settings() {
  
  com_camera_exp_time         = camera_exp_time;
  com_run_distance            = run_dist;
  
  com_motor_calibration_index = motor_calibration_index;            
  com_motor_video_dist        = motor_video_dist;
  com_motor_video_speed       = motor_video_speed;
  com_motor_video_ramp_in     = motor_video_ramp_in;
  com_motor_video_ramp_out    = motor_video_ramp_out;
  com_motor_looping           = motor_is_looping();
  com_motor_ramp_percent      = motor_ramp_percent;
  com_motor_direction         = isBit(motor_status, BIT_3);
  
  
}

// ===================================================================================
// restores the original values when editing the master'settings again
// ===================================================================================
void com_restore_master_settings() {
  
  camera_exp_time         = com_camera_exp_time;
  run_dist                = com_run_distance;
  
  motor_calibration_index = com_motor_calibration_index;            
  motor_video_dist        = com_motor_video_dist;
  motor_video_speed       = com_motor_video_speed;
  motor_video_ramp_in     = com_motor_video_ramp_in;
  motor_video_ramp_out    = com_motor_video_ramp_out;
  motor_ramp_percent      = com_motor_ramp_percent;
  motor_set_direction(com_motor_direction);
  
  if (com_motor_looping) {
    setBit(motor_key_status, BIT_2);
  } else {
    deleteBit(motor_key_status, BIT_2);
  }
  
  
    
}


// ===================================================================================
// clears the ui-data array
// ===================================================================================
void com_clear_slave_ui_data() {
    
  for (int i=0; i<MOCOM_MAX_SLAVES; i++) {
    com_slave_ui_data[i] = 0;
  }
  
}


