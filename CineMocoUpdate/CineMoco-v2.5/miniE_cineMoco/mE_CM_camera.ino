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


unsigned long camera_exposure;



// ===================================================================================
boolean camera_is_in_focus_phase()        { return action_status & B01000000; }
boolean camera_is_in_exposure_phase()     { return action_status & B00010000; }
boolean camera_is_in_post_phase()         { return action_status & B00000100; }


boolean camera_is_HDR_enabled()           { return exposure_status & B10000000; }
boolean camera_is_bulb_ramping_enabled()  { return exposure_status & B01000000; }

boolean camera_is_exposing()              { return exposure_status & B00100000; }
void camera_set_is_exposing()             { exposure_status |= B00100000; } 
void camera_delete_is_exposing()          { bitClear(exposure_status, 5); } // B11011111; 

boolean camera_is_in_HDR_delay()          { return exposure_status & B00010000; }
void camera_set_is_in_HDR_delay()         { exposure_status |= B00010000; } 
void camera_delete_is_in_HDR_delay()      { bitClear(exposure_status, 4); } // B11101111; 




// ===================================================================================
void camera_start_focus_phase() {
  
  // enpower the focus pin
  digitalWrite(DEF_CORE_FOCUS_PIN, HIGH);
  
  // remember the starting time
  action_start_time = millis(); 
  
  // set the "i am focussing" flag to on
  action_status |= B01000000;   
  
}


// ===================================================================================
// this function does not close the focus line because it will be handled in the 
// function camera_shoot - called directly after this methode
void camera_stop_focus_phase() {
 
  // toggle the "i am focussing" flag to off B01000000
  bitClear(action_status, 6);
 
}



// ===================================================================================
void camera_start_exposure_phase() {
  
  // set the "i am in exposure phase" flag to on
  action_status |= B00010000;   
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  action_start_time = millis();
  
}



// ===================================================================================
void camera_stop_exposure_phase() {
  
  // just in case 
  if (camera_is_exposing()) camera_detrigger();
    
  // do some continuous steps    
  motor_continuous_check();
    
  // toggle the "i am in exposure phase" flag to off B00010000
  bitClear(action_status, 4);
  
  // increment the shoot counter
  camera_shoot_count++;
  
  // do some continuous steps    
  motor_continuous_check();
  
}




// ===================================================================================
boolean camera_start_post_phase() {
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // set the past exposure flag
  if (camera_post > 0) {
    
    action_status |= B00000100;  
  
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
  
    action_start_time = millis();
  
    return true;
    
  // otherwise return false
  } else {
    
    return false;
    
  }
}

// ===================================================================================
void camera_stop_post_phase() {
 
 // clear camera post delay bit
 bitClear(action_status, 2); 
  
}


// ===================================================================================
// blocking function for test shots
void camera_test_shoot(long exposure) {
  
  camera_trigger();
  
  core_delay(exposure);
  
  camera_detrigger();
    
  // toggle the "i am exposing" flag to off B00010000
  //bitClear(action_status, 4);
}


// ===================================================================================
boolean camera_check_exposure_state() {
  
  // camera is not exposing right now
  if (!camera_is_exposing() && 
      !camera_is_in_HDR_delay()) {
    
    // do some continuous steps    
    motor_continuous_check();

        
    // if bulb ramping is enabled
    if (camera_is_bulb_ramping_enabled()) {
      
      // calculate the exposure difference between the single shots
      float bulp_delta = ((float) HDR_exposure_last - (float) HDR_exposure_first) / (float) (camera_shot_max - 1);
      
      // do some continuous steps    
      motor_continuous_check();
      
      // set the camera exposure for the current shot
      camera_exposure = HDR_exposure_first + ((float) camera_shoot_count * bulp_delta);
      
      // do some continuous steps    
      motor_continuous_check();
      
    } else {
      
      camera_exposure = camera_exp_time; 
     
    } 
    
    
    // if HDR is enabled
    if (camera_is_HDR_enabled()) {
      
      // set the shot count to the first shot
      HDR_current_shot++;
      
      // calculate the minimum and maximum exposure values for this specific shot
      float HDR_min_exposure = (float) camera_exposure / HDR_bracket_factor;
      //unsigned int HDR_max_exposure = camera_exposure * HDR_bracket_factor;
      
      // calculate the actual exposure value for the current shot
      camera_exposure = HDR_min_exposure * pow(HDR_bracket_step_factor, HDR_current_shot - 1);

    }
    
    // start the actual exposure
    camera_trigger();
 
  } // camera is not exposing 
  
  
  // do some continuous steps    
  motor_continuous_check();
  
  
  // is exposing right now or in delay between single HDR shots?
  if (camera_is_exposing() ||
      camera_is_in_HDR_delay()) {
    
    // do some continuous steps    
    motor_continuous_check();
        
    // did we exposed the time we wanted to expose?
    if (camera_is_exposing() &&
        ((action_start_time + camera_exposure) <= millis())) {
    
      // do some continuous steps    
      motor_continuous_check();  
          
      camera_detrigger();
      
      // do some continuous steps    
      motor_continuous_check();
      
      // check if we are done with the current frame:
      if (!camera_is_HDR_enabled()) {
        
        // do some continuous steps    
        motor_continuous_check();
        
        ui_set_paint_shoot_count_flag();
        //ui_set_repaint_header_flag(); 

        return true;
      } 
    
      // we have HDR enabled  
      else {
        
        // if we did all the shots we wanted
        if (HDR_current_shot == HDR_shot_number) {
          
          HDR_current_shot = 0;
          
          //ui_is_paint_shoot_count_flag();          
          //ui_set_repaint_header_flag(); 
          
          return true;
          
        } else {
        
          // wait a little while between the HDR shots - 
          // otherwise the camera wont recognioze that there are more than one shots
          camera_set_is_in_HDR_delay();
          
          // remember the starting time
          action_start_time = millis(); 
        
        }
      }
    }  
    
    
    // do some continuous steps    
    motor_continuous_check();
    
    
    // is the HDR delay over?
    if (camera_is_in_HDR_delay() &&
        ((action_start_time + (unsigned long) camera_post) <= millis())) {
      
      camera_delete_is_in_HDR_delay(); 
        
    }

  } 
  
  
  // do some continuous steps    
  motor_continuous_check();
  
  
  return false;
}




// ===================================================================================
void camera_trigger() {
  
  // set the "is exposing" flag
  camera_set_is_exposing(); 
  
  // close or open the focus line while shooting (relevant for shootimg behavior of different camera brands)
  if (camera_types[camera_type_index].focusHighWithShutter) {
    digitalWrite(DEF_CORE_FOCUS_PIN, HIGH);
  } else {
    digitalWrite(DEF_CORE_FOCUS_PIN, LOW);
  }
  
  // enpower the camera pin
  digitalWrite(DEF_CORE_CAMERA_PIN, HIGH);
  
  // remember the starting time
  action_start_time = millis(); 
  
}



// ===================================================================================
void camera_detrigger() {

  digitalWrite(DEF_CORE_FOCUS_PIN, LOW);
  digitalWrite(DEF_CORE_CAMERA_PIN, LOW);
  
  // delete the "is exposing" flag
  camera_delete_is_exposing() ;
   
}



// ===================================================================================
void camera_prepare_HDR_values() {
  
  if (camera_is_HDR_enabled()) {
    
    // reset the internal HDR shot counter
    HDR_current_shot = 0;
    
    // calculate the actual bracketing value
    float bracket = (float) HDR_bracket * 1.0 / 3.0;
        
    // calculate the exposure time factor (one side) out of the bracket
    HDR_bracket_factor = pow(2, bracket); 
  
    // the step factor between each HDR shot
    // (3 shots and +/2 means 2 brackets between each shot --> 2 brackets = exposure factor 4)
    HDR_bracket_step_factor = pow(2, (bracket * 2.0) / (float) (HDR_shot_number - 1));
  
  }
  
}

