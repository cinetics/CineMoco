 /*
    
    (c) 2015 Airic Lenz

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

char noll[] = "0";
char leer[] = " ";
char dot[]  = ".";


// ======================================================================================
// paints the base header
// ======================================================================================
void ui_paint_base_header() {
 
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  GLCD.FillRect( 0, 0, GLCD.Width, header_height, BLACK);
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
    
  GLCD.FillRect( 0, header_height + 1, GLCD.Width, 1, WHITE);
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  GLCD.SetFontColor( WHITE);
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
}


// ======================================================================================
// paints the video string in the header
// ======================================================================================
void ui_paint_video_hint() {

  GLCD.SetFontColor( WHITE);
  GLCD.CursorToXY(83, 2);
  
  ui_get_string(128, 0);
  GLCD.print(lines[0]);  
  
  
}

// ======================================================================================
// paints the dashboard screen (when the program is running) 
// ======================================================================================
void ui_paint_dashboard() {
  
  // clear the screen
  GLCD.FillRect( 0, 0, GLCD.Width, GLCD.Height, WHITE);
  
  GLCD.SetFontColor( BLACK);
  
  GLCD.CursorToXY(22, 3);
  GLCD.print(F("IN PROGRESS..."));  

  // only paint the shoot count if not in video mode  
  if (system_mode != DEF_MODE_VIDEO) {
    GLCD.CursorToXY(22, 21);
    GLCD.print(F("Shoot"));  
    ui_paint_shoot_count(true);
    
    GLCD.CursorToXY(22, 33);
    GLCD.print(F("Battery"));
    ui_paint_power_state(100, 32, true);  
    
  } else {
    
    GLCD.CursorToXY(22, 21);
    GLCD.print(F("Battery"));
    ui_paint_power_state(100, 20, true);  
  }
    
    
  GLCD.CursorToXY(13, 54);
  GLCD.print(F("PRESS PLAY TO END"));  
  
  
}

// ======================================================================================
// paints the "program done" screen 
// ======================================================================================
void ui_paint_done_screen() {

  // clear the screen
  //GLCD.FillRect( 0, 0, GLCD.Width, GLCD.Height, WHITE);
  
  GLCD.SetFontColor( BLACK);
  
  // just overwrite this part:
  GLCD.CursorToXY(22, 3);
  GLCD.print(F("   COMPLETE   "));  
  
  // only paint the final shoot count if not in video mode  
  if (system_mode != DEF_MODE_VIDEO) {
    ui_paint_shoot_count(true);
  }
  
}


// ======================================================================================
// paints the shoot count
// ======================================================================================
void ui_paint_shoot_count(boolean live) {
 
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // if no shot limit or limit bigger than 999:
  if ((camera_shot_max == 0) || (camera_shot_max > 999)) {
    
    if (live) {
      
      GLCD.SetFontColor(BLACK);
      GLCD.CursorToXY(73, 21);
      
    } else {
      
      GLCD.SetFontColor(WHITE);
      GLCD.CursorToXY(80, 2);
      
    }
        
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // avoid numbers bigger than the shoot count when bouncing
    if (camera_shoot_count > camera_shot_max) {
      camera_shoot_count -= camera_shot_max;  
    }
    
    // paint just the shot count in this style [XXXX]
    if (camera_shoot_count < 10)        GLCD.print(F("[   "));
    else if (camera_shoot_count < 100)  GLCD.print(F("[  "));
    else if (camera_shoot_count < 1000) GLCD.print(F("[ "));
    else                                GLCD.print(F("["));
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    GLCD.print(camera_shoot_count, DEC); 
    GLCD.print("]");  
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
  } else {
    
    if (live) {
      GLCD.CursorToXY(55, 21);
    } else {
      GLCD.CursorToXY(64, 2);
    }
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // paint the shot count in this style: [XXX/YYY]
    if (camera_shoot_count < 10)       GLCD.print(F("[  "));  
    else if (camera_shoot_count < 100) GLCD.print(F("[ "));
    else                               GLCD.print(F("[")); 
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    GLCD.print(camera_shoot_count, DEC);   
   
    // if the programm is running and we are in contiuous mode
    motor_continuous_check(); 
   
    if (camera_shot_max < 10)       GLCD.print(F("/  "));  
    else if (camera_shot_max < 100) GLCD.print(F("/ "));
    else                            GLCD.print(F("/")); 
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    GLCD.print(camera_shot_max, DEC);   
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    GLCD.print(F("]"));
    
  }
  
}



// ======================================================================================
// paints the base menu header
// ======================================================================================
void ui_paint_status_header_base() {
  
  ui_paint_base_header();
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // paint the power status
  ui_paint_power_state(120, 1, false);
}




// ======================================================================================
// paints the "advanced menu" header
// ======================================================================================
void ui_paint_advanced_status_header() {

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // paint the basic menu header
  ui_paint_status_header_base();
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

  // add additional information:
  
  // generate the string with the cycle length
  char value_str[8];
  dtostrf( (float) cycle_length / 1000.0, 3, 1, value_str);

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  ui_get_string(68, 0);  // "
  strcat(value_str, lines[0]);
  
  // paint the cycle length
  GLCD.CursorToXY(2, 2);
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  GLCD.print(value_str);
 
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  ui_paint_shoot_count(false);
    
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
 
}



// ======================================================================================
// paints the advanced settings header
// ======================================================================================
void ui_paint_advanced_settings_header() {
  
  ui_paint_base_header();
  byte pos = main_pos - 1;
  
  // paint the tab we are on:
  GLCD.FillRect( (32*pos) + 1, 1, 29, header_height - 2, WHITE);
  GLCD.SetDot( (32*pos) + 1,  1, BLACK);
  GLCD.SetDot( (32*pos) + 30, 1, BLACK);
  
  // write all 4 tab titles
  for (int i=0; i<4; i++) {
    
    ui_get_string( i+16, 0); // GEN, CON, MOT, EXP
    
    GLCD.CursorToXY((32*i) + 7 + (i/2), 2);
    
    // check which color is needed for the text
    if ((main_pos - 1) == i) {
      GLCD.SetFontColor(BLACK);  
    } else {
      GLCD.SetFontColor(WHITE);  
    }
    
    GLCD.print(lines[0]);
  }
  
  
}


// ======================================================================================
// paints the menu entries
// ======================================================================================
// result = true when the first line is on screen
//
boolean ui_paint_menu(byte pressed_key, boolean paint_first_line) {
    
  GLCD.SetFontColor(BLACK);
     
  // calculate scrolling pos   
  byte menu_offset = menu_pos - menu_cursor_pos;
  
  // return value that shows if the first line is on screen
  boolean first_line_on_screen = (menu_offset == 0);
    
  for (int i=0; i<menu_max_screen_lines; i++) {
       
    // check if the this is the first line and
    // if it should be painted
    if (!((i == 0) && (!paint_first_line) && (first_line_on_screen))) {
    
       // selected line?
      if ((menu_pos == i + menu_offset) && 
          (!core_is_program_running())) {
      
        // paint the selection bar
        GLCD.FillRect( 0,  14 + (i * menu_screen_line_height) - 1, 
                       GLCD.Width - 5, 9, BLACK);
        
        // paint the text line
        GLCD.SetFontColor(WHITE);
        GLCD.CursorToXY(2, 14 + (i * menu_screen_line_height));
        GLCD.print(lines[i + menu_offset]);   
        
        // paint the data value
        ui_print_value(line_codes[i + menu_offset], 14 + (i*menu_screen_line_height), true, pressed_key);
        GLCD.SetFontColor(BLACK);     
         
       
      } 
      // regular menu line
      else if (i < menu_length) {
              
        // clear the selection bar
        GLCD.FillRect( 0,  14 + (i * menu_screen_line_height) - 1, 
                       GLCD.Width - 5, 9, WHITE);
        
        
        
        // paint the text line
        GLCD.CursorToXY(2, 14 + (i * menu_screen_line_height));
        GLCD.print(lines[i + menu_offset]); 
      
        // paint the data value
        ui_print_value(line_codes[i + menu_offset], 14 + (i*menu_screen_line_height), false, pressed_key);  
            
      } // no menu lines anymore - clear anyway to delete possible former entries at that position
      else {
      
        // clear the selection bar
        GLCD.FillRect( 0,  14 + (i * menu_screen_line_height) - 1, GLCD.Width - 5, 9, WHITE);
      
      }
    
    }
        
  }
    
  return first_line_on_screen;  
    
}


// ======================================================================================
// paints the scrollbar if required
// ======================================================================================
void ui_paint_scrollbar(boolean paint, boolean full_screen_scrolling) {
  
  // clear the scrollbar area
  GLCD.FillRect( GLCD.Width - 3 , header_height + 1, 3, GLCD.Height, WHITE);
  
  if (paint) {
  
    byte first_visible_line; 
    
    // do we scroll with a cursor on the screen or just the full screen?
    if (full_screen_scrolling)   first_visible_line = menu_cursor_pos;
    else                         first_visible_line = menu_pos - menu_cursor_pos;

    byte bar_max_len = GLCD.Height - header_height - 6;
    byte bar_length;
   
    if (menu_length > menu_max_screen_lines) {
      // calculate the need length of the scroll bar
      bar_length = (float)menu_max_screen_lines / (float)menu_length * (float)bar_max_len;    
      
    } else {
      bar_length = bar_max_len;    
    }

    byte bar_start;
   
    // calculate where the bar starts with the current position in the list
    if (full_screen_scrolling) bar_start = (float)bar_max_len * ((float)first_visible_line / (float)(menu_length - 2));
    else                       bar_start = (float)bar_max_len * ((float)first_visible_line / (float)(menu_length - 1));
    
    // paint the scrollbar    
    GLCD.FillRect( GLCD.Width - 2 , header_height + 3 + bar_start, 
                   1,  bar_length, 
                   BLACK);
    
    
  }
    
}


// ======================================================================================
// this function paints the move section ( <-- move --> )
// ======================================================================================
void ui_paint_move_section(byte key) {
  
  ui_get_string(129, 0); // move
  ui_get_string(130, 1); // <--
  ui_get_string(131, 2); // -->
  

  // ----------------- key - LEFT
  if ((menu_pos == 0) && 
      (key == KEY_LEFT) &&
      (!core_is_program_running())) {
        
    // paint the selected move button
    GLCD.FillRect( 0,  13, 30,  8, BLACK);  
    GLCD.SetFontColor(WHITE);
    
  } else {
    
    // paint the selected move button
    GLCD.FillRect( 0,  13, 30,  8, WHITE);  
    GLCD.SetFontColor(BLACK);
    
  }
  
  GLCD.CursorToXY(10, 14);
  GLCD.print(lines[1]);

  
  
  // ----------------- are we on the first line? (make the main item selected or unselected)
  if ((menu_pos == 0) && 
      (key != KEY_LEFT) && 
      (key != KEY_RIGHT) &&
      (!core_is_program_running())) {
        
    // paint the selected move button
    GLCD.FillRect( 31,  13, 61,  8, BLACK);  
    GLCD.SetFontColor(WHITE);
    
  } else {
    
    // paint the selected move button
    GLCD.FillRect( 31,  13, 61,  8, WHITE);  
    GLCD.SetFontColor(BLACK);
    
  }
    
  // paint the center text line
  byte text_width = GLCD.StringWidth(lines[0]);
  GLCD.CursorToXY(((GLCD.Width - 2) / 2) - (text_width / 2), 14);
  
  GLCD.print(lines[0]);
    
  
  
  
  
  
  // ----------------- key - RIGHT
  if ((menu_pos == 0) && 
      (key == KEY_RIGHT) &&
      (!core_is_program_running())) {
  
    // paint the selected move button
    GLCD.FillRect( 93,  13, 30,  8, BLACK);  
    GLCD.SetFontColor(WHITE);
    
  } else {
    
    // paint the selected move button
    GLCD.FillRect( 93,  13, 30,  8, WHITE);  
    GLCD.SetFontColor(BLACK);
    
  }
  
  
  GLCD.CursorToXY(104, 14);
  GLCD.print(lines[2]);
  
  
  
  // ----------------- Separation line
  
  // paint a separation line between this section and the rest below (clear this area)
  GLCD.DrawHLine(0, 12 + menu_screen_line_height, GLCD.Width - 3, WHITE);
  
  
}      




// ======================================================================================
// this function paints the values (identified by the linecode)
// ======================================================================================
void ui_print_value(byte line_code, byte yPos, boolean editing, byte key) {
 
  char value_str[11];
  char temp[11];
  

  
  switch (line_code) {
    
    // Move Style
    case 7:   {
                if ((editing) && 
                    ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                  ui_change_value_numeric_ubyte(&system_move_style, 1, 0, DEF_MOVE_STYLE_COUNT - 1, true);
                      
                  if (com.isMaster());
                    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA, 7, system_move_style);
                  
                  // do the needed changes
                  core_move_style_change();   
                      
                  // receive the settings that can be changed remotely
                  com_receive_settings();
                                                  
                }
                             
                                
                switch (system_move_style) {
                  
                  case DEF_MOVE_STYLE_CONTINUOUS:   ui_get_string( 8, 0); // Continuous
                                                    break;       
                  case DEF_MOVE_STYLE_SMS:          ui_get_string( 9, 0); // S-M-S
                                                    break;
                }
      
                strcpy(value_str, lines[0]);
                break;
              } 
    
    
    /*
    // interval
    case 20:  { 
                if (editing) {
                  
                  byte stepSize;
                  
                  if (key == KEY_RIGHT) {
                    
                    if (cycle_length < 100000) stepSize = 100;
                    else                       stepSize = 1000;
                      
                  } else if (key == KEY_LEFT) {
                    
                    if (cycle_length > 100000) stepSize = 1000;
                    else                       stepSize = 100;
                    
                  }
                  
                  ui_change_value_numeric_ulong(&cycle_length, stepSize, 2, 0);
                  
                }

                // determine the full number
                unsigned int num = (unsigned int) (cycle_length / 1000.0);
                itoa(num, value_str, 10); 
                
                if (num < 10000) {
                  
                  // determine the digits
                  unsigned int digits = cycle_length - (num * 1000);
                  
                  itoa(digits, temp, 10);  
                
                  strcat(value_str, dot);
                  strcat(value_str, temp);
                }
                
                ui_get_string(68, 0);
                strcat(value_str, lines[0]);
                break;
              }
    */
    
    // distance
    case 21:  {  
                
                unsigned long temp_dist;
                unsigned long fac;
                
                // initialize the factor variable with valid values
                if      (motor_cycle_distance < 1)    fac = 100; 
                else if (motor_cycle_distance < 100)  fac = 10;
                else                                  fac = 1;
                
                  
                if (editing) { 
                    
                  if (key == KEY_RIGHT) {
                    
                    if      (motor_cycle_distance < 1)    fac = 100; 
                    else if (motor_cycle_distance < 100)  fac = 10;
                    else                                  fac = 1;
                      
                  } else if (key == KEY_LEFT) {
                    
                    if      (motor_cycle_distance > 100)  fac = 1; 
                    else if (motor_cycle_distance > 1)    fac = 10;
                    else                                  fac = 100;
                  
                  }
                
                }
                              
                temp_dist = (motor_cycle_distance * fac) + 0.1; // + 0.1 to avoid 0.9999999
                
                if (editing) { 
                  ui_change_value_numeric_ulong(&temp_dist, 1, 0, 0);
                  motor_cycle_distance = (float) temp_dist / (float) fac;
                }  
                
                // add the full numbers to the string
                itoa((temp_dist / fac), value_str, 10);
                
                // add the comma
                strcat(value_str, dot);
                
                // calucate the digits after the comma as a full number
                unsigned long comma = temp_dist - (fac * (unsigned long) (temp_dist / fac));
                
                // add a leading zero if needed
                if ((comma < 10) && (fac > 10)) strcat(value_str, noll);
                                  
                // convert these number to a temp string
                itoa(comma, temp, 10);
                
                // add this temp string to the result string
                strcat(value_str, temp);
                
                // add the unit
                ui_get_string( 63, 0); // cm
                strcat(value_str, lines[0]);
                
                break; 
              }
              
    // speed
    case 22:  {  
                
                if ((editing) && 
                    ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                  ui_change_value_numeric_ubyte(&motor_step_sizes_index, 1, 0, MOTOR_STEP_SIZE_COUNT - 1, true);
                  
                  // set the new value
                  motor_setStepSize();
                                
                }
                                
                strcpy(value_str, motor_step_sizes[motor_step_sizes_index].name);
                break; 
                
              }          
                  
    // direction
    case 23:  {  
                
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                    
                    // invalid and noneditable menu line
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                
                } else {
                    
                  if ((editing) && 
                      ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                    ui_change_value_boolean_bit(&motor_status, 3); 
                      
                    // if we are master and there is a selected slave
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                      
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 23, (byte) isBit(motor_status, BIT_3));
                    } 
                    
                  }
                  
                  if (isBit(motor_status, BIT_3)) {
                    ui_get_string( 2, 0); // CW    
                  } else {
                    ui_get_string( 3, 0); // CCW   
                  }
                  strcpy(value_str, lines[0]);
                }
                
                break;  
              }
              
    // motor sleep
    case 24:  {  
                if (editing && ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  toggleBit(motor_status, BIT_7);
                }
                
                if (isBit(motor_status, BIT_7)) {
                  ui_get_string( 4, 0); // enabled   
                } else {
                  ui_get_string( 5, 0); // disabled
                }
                strcpy(value_str, lines[0]);
                
                break;  
              }
              
    // record style
    case 25:  {  
                      
                byte old_system_mode = system_mode;
                
                if (editing && ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                    
                  ui_change_value_numeric_ubyte(&system_mode, 1, 0, DEF_MODE_COUNT - 1, true); 
                  
                   // do the needed changes
                  core_move_style_change();   
                  
                  // send the new mode to all slaves
                  if (com.isMaster()) {
                    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA, 25, system_mode);
                  }
                                      
                  // send the global run - setup settings (record time, fps, playback time)
                  // if in a timelapse mode
                  com_send_sms_run_data(); 
                                      
                  // in case there is already a slave selected
                  if ((com_slave_pos != 0) && 
                       com.isMaster()) {
                    
                    // receive the settings that can be changed remotely
                    com_receive_settings();
                  }
                  
                  // send a repaint after the new modes were sent
                  com_send_repaint_flag(true);
                  
                } // editing
                
      
                switch (system_mode) {
                  
                  case DEF_MODE_TIMELAPSE:   ui_get_string( 10, 0); // timelapse
                                             break;       
                  case DEF_MODE_VIDEO:       ui_get_string( 53, 0); // video
                                             break;
                }
      
                strcpy(value_str, lines[0]);
                break;
                  
              }
                        
            
              
    // camera type
    case 26:  {  
                
                if (editing) ui_change_value_numeric_ubyte(&camera_type_index, 1, 0, CAMERA_TYPE_COUNT - 1, true);
                
                // get the name of this value
                strcpy(value_str, camera_types[camera_type_index].name); 
                
                break;  
              }              
              
    // test shot
    case 27:  {  
                if ((editing) && (key == KEY_RIGHT)) {
                  camera_test_shoot(150);
                }
                
                ui_get_string( 13, 0); // fire   
                strcpy(value_str, lines[0]); 
                
                break;  
              }           
   
    
    // information
    case 29:  {  
                if ((editing) && (key == KEY_RIGHT)) {
                  ui_init_info_message();
                }
                
                ui_get_string( 13, 0); // ->    
                strcpy(value_str, lines[0]);
                
                break;  
              }          
    
    // total distance
    case 30:  {  
                  if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                  } else {
               
                    
                    if ((editing) && 
                      ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                    
                    float stepSize = 1;
                                      
                    ui_change_value_numeric_float(&run_dist, stepSize, 0, 0);    
                    
                    // cut off unwanted granularity (we want it only as detailed as step size)
                    run_dist = ((unsigned long) (run_dist / stepSize)) * stepSize;
                         
                    // if we are master and there is a selected slave
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                    
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 30, run_dist);
                    }     
                         
                    // calculate the distance to move for one cycle; 
                    core_calculate_cycle_distance();
        
                  }
                                
                  // store the float also as long (cut everything behind the comma)
                  int cutter = run_dist;
                  itoa(cutter, value_str, 10);
                  
                  if (motor_is_radial()) {
                    ui_get_string( 20, 0); // °
                  } else {
                    ui_get_string( 63, 0); // cm
                  }
                  
                  strcat(value_str, lines[0]);
                }
                
                
                break; 

              }
    
    // record time
    case 32:  {  
                if ((editing) && 
                    ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                
                  ui_change_value_numeric_ulong(&run_record_time, 10, 10, 0);  
                  
                  // recalculate the cycle length (interval)
                  core_calculate_interval();
                  
                  // calculate the distance to move for one cycle; 
                  core_calculate_cycle_distance();
                  
                  // do a full repaint
                  setBit(ui_status, BIT_8);
                  
                  // sebd the new settings to the slave (in case this is needed)
                  com_send_sms_run_data();
                  
                }
                
                
                int  hours = run_record_time / 3600L;
                byte minutes = (run_record_time - (hours * 3600L)) / 60L;
                byte seconds = run_record_time - (hours * 3600L) - (minutes * 60L);

                
                ui_get_string( 39, 0); // :
                
                
                itoa(hours, temp, 10);                      // convert hours to temp string 
                if (hours < 10) {                           // add leading zero if needed... 
                  strcpy(value_str, noll);  
                  strcat(value_str, temp);                  // and after the zero add hours from temp
                }   
                else {           
                  strcpy(value_str, temp);                  // ...if not, just add the hours
                } 
                
                strcat(value_str, lines[0]);                // add :              
                
                if (minutes < 10) strcat(value_str, noll);  // add leading zero if needed 
                itoa(minutes, temp, 10);                    // convert minutes to temp string 
                strcat(value_str, temp);                    // add minutes from temp  
                strcat(value_str, lines[0]);                // add :   
                
                if (seconds < 10) strcat(value_str, noll);  // add leading zero if needed 
                itoa(seconds, temp, 10);                    // convert seconds to temp string 
                strcat(value_str, temp);                    // add seconds from temp  
                
                // add "
                ui_get_string(68, 0);  // "
                strcat(value_str, lines[0]);                // add " 
                
                break; 

              }
              
    // playback time
    case 33:  {  
                if ((editing) && 
                    ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                  ui_change_value_numeric_ulong(&run_play_time, 1, 1, 3599); // min = 1 sec; max = 59 min 59 sec  

                  // recalculate shot #
                  core_calculate_shot_number();
                  
                  // recalculate the cycle length
                  core_calculate_interval();
                  
                  // calculate the distance to move for one cycle; 
                  core_calculate_cycle_distance();
                  
                  // do a full repaint
                  setBit(ui_status, BIT_8);
                  
                  // sebd the new settings to the slave (in case this is needed)
                  com_send_sms_run_data();
                
                }
                
                            
                int  hours = run_play_time / 3600L;
                byte minutes = (run_play_time - (hours * 3600L)) / 60L;
                byte seconds = run_play_time - (hours * 3600L) - (minutes * 60L);
                
                
                ui_get_string( 39, 0); // :
                
                itoa(minutes, temp, 10);                    // convert minutes to temp string 
                
                if (minutes < 10) {                         // add leading zero if needed 
                  strcpy(value_str, noll);  
                  strcat(value_str, temp);                 
                } else {
                  strcpy(value_str, temp);       
                }   
                                
                strcat(value_str, lines[0]);                // add :   
                
                if (seconds < 10) strcat(value_str, noll);  // add leading zero if needed 
                itoa(seconds, temp, 10);                    // convert seconds to temp string 
                strcat(value_str, temp);                    // add seconds from temp  
                
                // add "
                ui_get_string(68, 0);  // "
                strcat(value_str, lines[0]);                // add " 
                
                
                break; 

              }          
    
    
    
    // FPS
    case 34:  {  
                if ((editing) && 
                    ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                  // loop the values with step-size 1 from 1 to 60
                  ui_change_value_numeric_ubyte(&run_fps, 1, 1, 60, true);
                  
                  // recalculate shot #
                  core_calculate_shot_number();
                  
                  // recalculate the cycle length (interval)
                  core_calculate_interval();
                  
                  // calculate the distance to move for one cycle; 
                  core_calculate_cycle_distance();
                  
                  // do a full repaint
                  setBit(ui_status, BIT_8);
                  
                  // sebd the new settings to the slave (in case this is needed)
                  com_send_sms_run_data();

                }
                
                
                // convert it to a string
                itoa(run_fps, value_str, 10);
                ui_get_string( 31, 0); // fps
                // add the unit
                strcat(value_str, lines[0]);
                
                break;  
              }
    
    
    // Start from Home
    case 35:  {  
                if (editing) ui_change_value_boolean_bit(&motor_status, 2);
                                
                if (isBit(motor_status, BIT_2)) {
                  ui_get_string( 4, 0); // enabled   
                } else {
                  ui_get_string( 5, 0); // disabled
                }
                
                strcpy(value_str, lines[0]);
                break;  
                
              }           
               
              
    // exposure time
    case 36:  {  
                
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                } else {
      
                  // if bulp ramping is disabled
                  if (!camera_is_bulb_ramping_enabled()) {
  
                    // change the exposure value
                    if ((editing) && 
                       ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                        
                      ui_change_exposure_value(&camera_exp_time, DEF_CAMERA_MIN_EXPOSURE, DEF_CAMERA_MAX_EXPOSURE);
                    
                      // in case there is already a slave selected, send the data
                      if ((com_slave_pos != 0) && 
                           com.isMaster()) {
                      
                        com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 36, (long) camera_exp_time);
                      }
                      
                    }
                    
                    // generate the value string
                    strcpy(value_str, ui_generate_exposure_value_string(&camera_exp_time));
                                        
                  } 
                  // bulp ramping is enabled
                  else {
                   
                    ui_get_string( 50, 0); // bulp ramp
                    strcpy(value_str, lines[0]);
                   
                  } 
                }
                
                
                break;  
              }
              
              
    // shot limit (shot #)
    case 37:  {  
                if (editing) {
                  
                  if (camera_is_bulb_ramping_enabled())  ui_change_value_numeric_ulong(&camera_shot_max, 1, 2, DEF_UINT_MAX);
                  else                                   ui_change_value_numeric_ulong(&camera_shot_max, 1, 0, DEF_UINT_MAX);
                  
                  camera_shoot_count = 0;
                }
                
                if (camera_shot_max > 0) {
                  itoa(camera_shot_max, value_str, 10);
                } else {
                  ui_get_string( 38, 0); // ms
                  strcpy(value_str, lines[0]);    
                }
                
                break;  
              }
             
    // backlight power
    case 40:  {  
                if (editing) ui_change_value_numeric_ubyte(&backlight_level, 1, 5, 100, false);  //change_backlight_level();
                
                // set the curretn backlight level
                ui_set_backlight_level();
                
                itoa(backlight_level, value_str, 10);
                ui_get_string( 62, 0); // %
                strcat(value_str, lines[0]);
                
                break;
              }
       
    // backlight wait
    case 41:  {  
                if (editing) ui_change_value_numeric_uint(&backlight_wait, 1, 3, DEF_UINT_MAX);     
                
                itoa(backlight_wait, value_str, 10);
                ui_get_string( 68, 0); // s
                strcat(value_str, lines[0]);
                
                break;    
              }
     
    // motor post
    case 42:  {  
            
                if ( (editing) && 
                  ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                  int stepSize;
                  
                  if (key == KEY_RIGHT) {
                    
                    if       (motor_post <  100)                           stepSize = 1;
                    else if ((motor_post >= 100)  && (motor_post < 1000))  stepSize = 10;
                    else if ((motor_post >= 1000) && (motor_post < 5000))  stepSize = 100;
                    else if ((motor_post >= 5000) && (motor_post < 10000)) stepSize = 500;
                    else                                                   stepSize = 1000;
                      
                  } else {
                    
                    if       (motor_post > 10000)                          stepSize = 1000;
                    else if ((motor_post > 5000) && (motor_post <= 10000)) stepSize = 500;
                    else if ((motor_post > 1000) && (motor_post <= 5000))  stepSize = 100;
                    else if ((motor_post > 100)  && (motor_post <= 1000))  stepSize = 10;
                    else                                                   stepSize = 1;
                  
                  }
                                    
                  ui_change_value_numeric_ulong(&motor_post, stepSize, 0, DEF_ULONG_MAX);    
                                    
                  // cut off unwanted granularity (we want it only as detailed as step size)
                  motor_post = (unsigned long) (motor_post / stepSize) * stepSize;
                }
                
                              
                if (motor_post < 1000) {
                  
                  itoa(motor_post, value_str, 10);
                  ui_get_string( 60, 0); // ms
                  strcat(value_str, lines[0]);
                                
                } else {
                                
                  // determine the full number
                  unsigned long num = motor_post / 1000;
                  itoa(num, value_str, 10); 
                
                  if (num < 100) {
                  
                    // determine the digits
                    unsigned int digits = (motor_post - (num * 1000)) / 100;
                  
                    itoa(digits, temp, 10);  
                
                    strcat(value_str, dot);
                    strcat(value_str, temp);
                  }
                
                  ui_get_string(68, 0); // "
                  strcat(value_str, lines[0]);
                  
                }
                
                break;
                
              }
              
    // camera post delay
    case 43:  {  
                if ( (editing) && 
                  ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                  
                  int stepSize;
                  
                  if (key == KEY_RIGHT) {
                    
                    if       (camera_post <  100)                             stepSize = 1;
                    else if ((camera_post >= 100)  && (camera_post < 1000))   stepSize = 10;
                    else if ((camera_post >= 1000) && (camera_post < 5000))   stepSize = 100;
                    else if ((camera_post >= 5000) && (camera_post < 10000))  stepSize = 500;
                    else                                                      stepSize = 1000;
                      
                  } else {
                    
                    if       (camera_post > 10000)                            stepSize = 1000;
                    else if ((camera_post > 5000) && (camera_post <= 10000))  stepSize = 500;
                    else if ((camera_post > 1000) && (camera_post <= 5000))   stepSize = 100;
                    else if ((camera_post > 100)  && (camera_post <= 1000))   stepSize = 10;
                    else                                                      stepSize = 1;
                  
                  }
                                    
                  ui_change_value_numeric_ulong(&camera_post, stepSize, 0, DEF_ULONG_MAX);    
                                    
                  // cut off unwanted granularity (we want it only as detailed as step size)
                  camera_post = (unsigned long) (camera_post / stepSize) * stepSize;
                }
                
                              
                if (camera_post < 1000) {
                  
                  itoa(camera_post, value_str, 10);
                  ui_get_string( 60, 0); // ms
                  strcat(value_str, lines[0]);
                                
                } else {
                                
                  // determine the full number
                  unsigned long num = camera_post / 1000;
                  itoa(num, value_str, 10); 
                
                  if (num < 100) {
                  
                    // determine the digits
                    unsigned int digits = (camera_post - (num * 1000)) / 100;
                  
                    itoa(digits, temp, 10);  
                
                    strcat(value_str, dot);
                    strcat(value_str, temp);
                  }
                
                  ui_get_string(68, 0); // "
                  strcat(value_str, lines[0]);
                  
                }
                
                break;
              }
    
    
    // motor calibration / type
    case 45:  { 
                // ui_change_value_numeric_ubyte(byte *value, byte stepSize, byte minimum, byte maximum, boolean loopValues)   
                  
                if (editing) ui_change_value_numeric_ubyte(&motor_calibration_index, 1, 0, MOTOR_CALIBRATION_COUNT - 1, true);
                                
                strcpy(value_str, motor_calibrations[motor_calibration_index].name);
                break;  
              }
    
    
    // go to motor home
    case 46:  {  
                if ((editing) && (key == KEY_RIGHT)) {
                      
                  // init the motor is moved to home meassage  and print it    
                  ui_init_motor_move_home_message();
                  ui_print_message();
                  
                  // move motor to home with error check  
                  if (motor_move_to_position(0)) {
                  
                    // init the "home reached " message and print it
                    ui_init_motor_home_reached_message();
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
                
                ui_get_string( 13, 0); // ->    
                strcpy(value_str, lines[0]);
                break;  
              } 
    
    // set motor home
    case 47:  {  
                if ((editing) && (key == KEY_RIGHT)) {
                    
                  // set home to the current position   
                  motor_home_steps = 0;
                    
                  // show the motor is set home message
                  ui_init_motor_set_home_message();
                    
                }
                
                ui_get_string( 13, 0); // ->    
                strcpy(value_str, lines[0]);
                break;  
              } 
              
    // HDR
    case 48:  {  
                if (editing) ui_change_value_boolean_bit(&exposure_status, 7);
                
                if (camera_is_HDR_enabled()) {
                  ui_get_string( 4, 0); // enabled   
                  
                  // if we are in continuous, go to SMS because 
                  // continuous is not allowed with HDR
                  if (core_is_mode_timelapse() && core_is_style_continuous()) {
                    system_move_style = DEF_MOVE_STYLE_SMS;
                  }
                  
                }
                else {
                  ui_get_string( 5, 0); // disabled
                }
                
                strcpy(value_str, lines[0]);
                break;  
                
              }           
    
     // HDR bracketing
    case 49:  {  
                
                // is HDR enebled?
                if (camera_is_HDR_enabled()) {
                  
                  if (editing) ui_change_value_numeric_ubyte(&HDR_bracket, 1, 1, 15, false);
                  
                  ui_get_string(109, 0);  // +/-
                  strcpy(value_str, lines[0]);
                  
                  // get the value in 1/3 steps...
                  byte main = HDR_bracket / 3;
                  byte rest = HDR_bracket % 3;
                  
                  // add the main number if not zero
                  if (main != 0) {
                    itoa(main, temp, 10); 
                    strcat(value_str, temp);
                  }  
                  
                  // addd the 1/3 step if not zero
                  if (rest != 0) {
                    strcat(value_str, leer);
                    
                    itoa(rest, temp, 10); 
                    strcat(value_str, temp);
                    strcat(value_str, "/3");
                  }  
                  
                }
                // HDR disabled 
                else {
                  // print grayed out
                  ui_get_string(55, 0); // -----
                  strcpy(value_str, lines[0]);
                }
                                
                break;  
                
              }           
    
    // HDR bulb ramping
    case 50:  {
                if (editing) { 
                  ui_change_value_boolean_bit(&exposure_status, 6);
                  
                  // we need a total-run-length when using bulp ramping....
                  // if there is no limit, set one
                  if ((camera_is_bulb_ramping_enabled()) && 
                      (camera_shot_max == 0)) {
                  
                    camera_shot_max = 250;
                        
                  }
                }
                                
                if (camera_is_bulb_ramping_enabled()) ui_get_string( 4, 0); // enabled   
                else                                  ui_get_string( 5, 0); // disabled
                
                strcpy(value_str, lines[0]);
                
                break;
              }
    
    // bulb ramping 1st exposure
    case 51:  {
      
                // is Bulb ramping disabled?
                if (camera_is_bulb_ramping_enabled()) {
                  
                  // change the exposure value
                  if (editing) ui_change_exposure_value(&HDR_exposure_first, DEF_CAMERA_MIN_EXPOSURE, DEF_CAMERA_MAX_EXPOSURE);
                    
                  // generate the value string
                  strcpy(value_str, ui_generate_exposure_value_string(&HDR_exposure_first));
                }  
                // Bulb ramping disabled
                else {
                  // print grayed out
                  ui_get_string(55, 0); // -----
                  strcpy(value_str, lines[0]);
                }
                
                break;
              }
              
    // bulb ramping Last exposure
    case 52:  {
                
                // is Bulb ramping disabled?
                if (camera_is_bulb_ramping_enabled()) {
                  
                  //if (editing) ui_change_value_numeric_uint(&HDR_exposure_last, 1, DEF_CAMERA_MIN_EXPOSURE, DEF_UINT_MAX);
                    
                  // change the exposure value
                  if (editing) ui_change_exposure_value(&HDR_exposure_last, DEF_CAMERA_MIN_EXPOSURE, DEF_CAMERA_MAX_EXPOSURE);
                    
                  // generate the value string
                  strcpy(value_str, ui_generate_exposure_value_string(&HDR_exposure_last));
                }
                // Bulb ramping disabled
                else {
                  // print grayed out
                  ui_get_string(55, 0); // -----
                  strcpy(value_str, lines[0]);
                }
                
                break;
              }
    
    
    // setup style
    case 54:  {  
                
                if ((editing) && 
                    ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                      
                  ui_change_value_numeric_ubyte(&ui_setup_style, 1, 0, DEF_SETUP_STYLE_COUNT - 1, true); 
                  
                  // calculate the new values for the new 
                  // setup type from the old ones...
                  core_caclulate_setup_style_changes();

                  // send the new mode to all slaves (if we have any)
                  if (com.isMaster()) {
                    com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_DATA, 54, ui_setup_style);
                  }
                  // send the new mode to all slaves (if we have any)
                  

                  
                  // send the global run - setup settings (record time, fps, playback time)
                  // if in a timelapse mode
                  com_send_sms_run_data();  
                                    
                  // in case there is already a slave selected
                  if ((com_slave_pos != 0) && 
                       com.isMaster()) {
                    com_receive_settings();
                  }
                  
                  // send a repaint after the new modes were sent
                  com_send_repaint_flag(true);
                  
                }
      
      
                switch (ui_setup_style) {
                  
                  //case DEF_SETUP_STYLE_CYCLE:     ui_get_string( 11, 0); // cycle based setup
                  //                                break;       
                  case DEF_SETUP_STYLE_RUN:       ui_get_string( 12, 0); // run based setup
                                                  break;
                  case DEF_SETUP_STYLE_KEYFRAMES: ui_get_string( 61, 0); // keyfgrame based setup
                                                  break;
                  
                }
      
                strcpy(value_str, lines[0]);
                break;
                  
              }
    
    
    // motor ramp percentage
    case 56:  {
      
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                } else {  
                  
                  if (editing) {
                    ui_change_value_numeric_ubyte(&motor_ramp_percent,  1, 0, 100, false);
                    
                    // in case there is already a slave selected, send the data
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                    
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 56, (byte) motor_ramp_percent);
                    }
                    
                  }
                                  
                  itoa(motor_ramp_percent, value_str, 10); 
                  ui_get_string( 62, 0); // %   
                  strcat(value_str, lines[0]);
                }
                
                break;
      
              }
    
    
    // reset settings
    case 57:  {  
                if ((editing) && (key == KEY_RIGHT)) {
                  
                  // show reset message
                  ui_init_reset_message();
                  ui_print_message();
                  
                  // change the save flag in the eeprom
                  eeprom_saved(false);  
                  
                  delay(2500);
   
                  // restart the system with empty EEPROM --> default vars 
                  reset_arduino();    
                    
                }
                
                ui_get_string( 13, 0); // ->    
                strcpy(value_str, lines[0]);
                break;  
              } 
    
    // motor ramp (jogging / move scrolling)
    case 58:  {  
                if (editing) {
                  
                  switch (motor_calibration_index) {
                  
                    // slider  
                    case 0: {  ui_change_value_numeric_ulong(&motor_ramp_jog,  80, 0, 320000);
                               break;
                    }   
                    // pan / tilt
                    case 1: {  ui_change_value_numeric_ulong(&motor_ramp_jog,  80, 0, 320000);
                               break;
                    } 
                    // S60 1.8°
                    case 2: {  ui_change_value_numeric_ulong(&motor_ramp_jog,  80, 0, 320000);
                               break;
                    } 
                    // M60 0.9°
                    case 3: {  ui_change_value_numeric_ulong(&motor_ramp_jog, 160, 0, 320000);
                               break;
                    } 
                    
                  }
                                    
                }
                
                
                switch (motor_calibration_index) {
                  case 0: { dtostrf((float) motor_ramp_jog / 1600.0, 4, 2, value_str);
                            break; }
                  case 1: { dtostrf((float) motor_ramp_jog / 1600.0, 4, 2, value_str);
                            break; }
                  case 2: { dtostrf((float) motor_ramp_jog / 1600.0, 4, 2, value_str);
                            break; }
                  case 3: { dtostrf((float) motor_ramp_jog / 3200.0, 4, 2, value_str);
                            break; }
                }
                
                ui_get_string( 65, 0); // rev                
                strcat(value_str, lines[0]);
                break;
                
              }          
    
    
    // motor speed for jogging
    case 59:  {
      
                if (editing) {
                  ui_change_value_numeric_ubyte(&motor_speed_jog,  1, 0, 100, false);
                }
                                
                itoa(motor_speed_jog, value_str, 10); 
                ui_get_string( 62, 0); // %   
                strcat(value_str, lines[0]);
                
                break;
      
              }
    
              
    // client / master motor selection
    case 66:  {  
                
                if (editing) {
                  
                  // do we have registered slave-devices?
                  if (com.getSlaveCount() > 0) {                  
                    

                    ///////////////////////////
                    ///////////////////////////                    
                    if (key == KEY_RIGHT) {
                      
                      // still selecting one of the slaves motors 
                      if (com_slave_motor_pos < com.getSlaveMotorCount() - 1) {
                        
                        com_slave_motor_pos++;
                                          
                      } else {
                        
                        // store the masters data
                        if (com_slave_pos == 0) {
                          // save our settings
                          com_save_master_settings();
                          // deselect all slaves
                          com.deselectSlave();
                        }
                        
                        
                        ///////////////////////////
                        // if we are standing on the last slave, then
                        // go to the "All" position which is 255
                        if (com_slave_pos == com.getSlaveCount()) {
                          
                          // go to the "All" position
                          com_slave_pos = 255;
                          // remove the selection
                          com.deselectSlave();
                          // send the current system mode to all slaves
                          com_send_system_mode(true);
                          // send a repaint after the new modes were sent
                          com_send_repaint_flag(true);
                          
                        }                        
                        
                        
                        // go to the master
                        else if (com_slave_pos == 255) {
                          
                          // select the master
                          com_slave_pos = 0;
                          // select the masters motor
                          com_slave_motor_pos = 0;
                          // restore the masters data
                          com_restore_master_settings();
                          
                        }
                        
                        
                        // select the next slave
                        else if (com.selectNextSlave()) {
                          
                          com_slave_pos++;
                          // send the current system mode to the slave
                          com_send_system_mode(false);
                          // receive the slaves settings
                          com_receive_settings();
                          // send a repaint after the new modes were sent
                          com_send_repaint_flag(true);
                          
                          // check if the client has motors:
                          if (com.getSlaveMotorCount() > 0) {
                            com_slave_motor_pos = 0; 
                          } else {
                            com_slave_motor_pos = 255;  
                          }
                          
                        } 
                        
                        
                        // no new slave could be selected, go back to 
                        // the master:
                        else {
                          
                          // select the master
                          com_slave_pos = 0;
                          // select the masters motor
                          com_slave_motor_pos = 0;
                          // restore the masters data
                          com_restore_master_settings();
                          
                        }
                        
                        
                      }        
                      
                    } // end: KEY RIGHT
              
              
                    ///////////////////////////
                    ///////////////////////////
                    if (key == KEY_LEFT) {
                     
                     // still selecting one of the slaves motors? 
                     if ((com_slave_motor_pos > 0) &&
                         (com_slave_motor_pos != 255)) {
                       
                       com_slave_motor_pos --;
                       
                     } else {
                       
                       
                       ///////////////////////////
                       // if we are standing on the master right now
                       if (com_slave_pos == 0) {
                         
                         // go to the "All" position
                         com_slave_pos = 255;
                         // save our settings
                         com_save_master_settings();
                         // remove the selection
                         com.deselectSlave();
                         // send the current system mode to all slaves
                         com_send_system_mode(true);
                         // send a repaint after the new modes were sent
                         com_send_repaint_flag(true);
                         
                       }  
                       
                       
                       // if we are standing on the "All" position 
                       // right now
                       else if (com_slave_pos == 255) {
                         
                         // go to the last slave
                         com_slave_pos = com.getSlaveCount();
                         // select the last slave
                         com.selectPreviousSlave();
                         // send the current system mode to the slave
                         com_send_system_mode(false);
                         // receive the slaves settings
                         com_receive_settings();
                         // send a repaint after the new modes were sent
                         com_send_repaint_flag(true);
                           
                         // select the last motor if the currently selected slave
                         if (com.getSlaveMotorCount() > 0) {
                           com_slave_motor_pos = com.getSlaveMotorCount() - 1; 
                         } else {
                           com_slave_motor_pos = 255;  
                         }
                         
                       } 
                       
                                              
                       // go to the previous slave
                       else if (com.selectPreviousSlave()) {
                         
                         com_slave_pos--;
                         // send the current system mode to the slave
                         com_send_system_mode(false);
                         // receive the slaves settings
                         com_receive_settings();
                         // send a repaint after the new modes were sent
                         com_send_repaint_flag(true);
                           
                         // select the last motor if the currently selected slave
                         if (com.getSlaveMotorCount() > 0) {
                           com_slave_motor_pos = com.getSlaveMotorCount() - 1; 
                         } else {
                           com_slave_motor_pos = 255;  
                         }
                           
                       } 
                       
                       
                       // no other valid option:
                       // select the master
                       else {
                         
                         // select the master
                         com_slave_pos = 0;
                         // select the masters motor
                         com_slave_motor_pos = 0;
                         // restore the masters data
                         com_restore_master_settings();
                         
                       }
                                             
                       
                       
                      } // not selecting a motor 
                                           
                    } // End: KEY LEFT
                    
                    
                  } else {
                    
                    com_slave_pos = 0;
                    com_slave_motor_pos = 0;
                    
                  }
                }
                
                
                // print the current state to the menu
                
                
                // MASTER
                if  (com_slave_pos == 0) {
                  
                  ui_get_string( 103, 0); // Master 
                  strcpy(value_str, lines[0]); 
                
                // ALL
                } else if (com_slave_pos == 255) {
                  
                  ui_get_string( 28, 0); // All 
                  strcpy(value_str, lines[0]); 
                                    
                } else {
                                    
                  ui_get_string( 21, 0); // #  
                  strcpy(value_str, lines[0]);
                  itoa(com.getSlaveID(), temp, 10); 
                  strcat(value_str, temp);
                                                      
                }
                
                break;  
              }           
    
              
    // communication chaning id
    case 102: {  
                if (editing) {
                  if ((key == KEY_RIGHT) || (key == KEY_LEFT)) {
                    
                    ui_change_value_numeric_ubyte(&com_id, 1, 0, MOCOM_MAX_SLAVES - 1, true);
                    
                    // set the new status
                    com.setID(com_id);  
                  }
                }
                                     
                
                // print the status to the screen
                if (com.isMaster()) {
                  ui_get_string( 103, 0); // master  
                  strcpy(value_str, lines[0]);
                } else {
                  ui_get_string( 104, 0); // device
                  itoa(com.getID(), temp, 10);        
                  strcat(value_str, temp); 
                }
                
                break;  
              }
  
    // update chaining info        
    case 106: { 
                if ((editing) && (key == KEY_RIGHT)) {
                  
                  // print a message on teh screen
                  ui_init_update_chain_info_message();
                  ui_print_message();
                  
                  // reset the menu selection
                  com_slave_motor_pos = 0;
                  com_slave_pos = 0;
                  
                  // clear the ui-data stored locally;
                  com_clear_slave_ui_data();
                  
                  // deselecct the currently selected slave
                  com.deselectSlave();
                  
                  // registering the slaves
                  com.registerSlaves();
                  
                  // send the new mode to all slaves (if we have any)
                  com_send_system_mode(true);
                  
                  // send the current timelapse setup (if in a timelapse mode)
                  com_send_sms_run_data();
                                                        
                  // end the motor home message  
                  ui_end_message();  

                }
                
                ui_get_string(13, 0); // ->
                strcpy(value_str, lines[0]);    
                break;  

              }


    // # of HDR shots           
    case 108: {   
                
                // is HDR enebled?
                if (camera_is_HDR_enabled()) {
                  
                  // change the value
                  if (editing) ui_change_value_numeric_ubyte(&HDR_shot_number, 1, 2, 7, false);
                  
                  // print the actual value
                  itoa(HDR_shot_number, value_str, 10);  
                } 
                
                // HDR disabled
                else {
                  // print grayed out
                  ui_get_string(55, 0); // -----
                  strcpy(value_str, lines[0]);
                }
                
                break;  
              }
    
     
    // show ecternal power supply           
    case 123: {   
                if (editing && 
                    ((key == KEY_RIGHT) ||
                     (key == KEY_LEFT))) {
                  
                  // toggle the flag     
                  battery_show_external_supply = !battery_show_external_supply;
                    
                  // set the settings were changed flag
                  ui_set_settings_changed_flag();    
                }
                
                    
                if (battery_show_external_supply) { 
                  ui_get_string( 4, 0); // enabled    
                } else {
                  ui_get_string( 5, 0); // disabled   
                }
                
                strcpy(value_str, lines[0]);
                break;  
              }
               
    // video speed           
    case 124: { 
                
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                } else {
      
                  if (editing) {
                    ui_change_value_numeric_ubyte(&motor_video_speed, 1, 1, 100, false);
                  
                    // in case there is already a slave selected, send the data
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                    
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 124, (byte) motor_video_speed);
                    }
                    
                  }
                  
                  // add the number to the reult string
                  itoa(motor_video_speed, value_str, 10);  
                  
                  // add %
                  ui_get_string( 62, 0); // enabled    
                  strcat(value_str, lines[0]);  
                  
                }
                
                break;  
              }
              
    // video ramp in           
    case 125: {   
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                } else {
                  
                  if (editing) {
                                      
                    ui_change_value_numeric_ubyte(&motor_video_ramp_in, 1, 0, 100, false);
                                      
                    if ((motor_video_ramp_in + motor_video_ramp_out) > 100) {
                      motor_video_ramp_out = 100 - motor_video_ramp_in;  
                      
                      //ui_set_repaint_flag();
                    }
                    
                    // in case there is already a slave selected, send the data
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                    
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 125, (byte) motor_video_ramp_in);
                    }
                    
                  }
                                
                  // store the float also as long (cut everything behind the comma)
                  itoa(motor_video_ramp_in, value_str, 10);
                  
                  // add cm
                  ui_get_string( 62, 0); // %
                  strcat(value_str, lines[0]);
                  
                }
                
                break; 
                                
              }
               
               
    // video ramp out           
    case 126: {   
                
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                } else {
                  
                  if (editing) {
                                      
                    ui_change_value_numeric_ubyte(&motor_video_ramp_out, 1, 0, 100, false);
                    
                    if ((motor_video_ramp_in + motor_video_ramp_out) > 100) {
                      motor_video_ramp_in = 100 - motor_video_ramp_out; 
                      
                      //ui_set_repaint_flag();
                    }
                    
                    
                    // in case there is already a slave selected, send the data
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                    
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 126, (byte) motor_video_ramp_out);
                    }
                    
                  }
                                
                  // store the float also as long (cut everything behind the comma)
                  itoa(motor_video_ramp_out, value_str, 10);
                  
                  // add cm
                  ui_get_string( 62, 0); // %
                  strcat(value_str, lines[0]);
                
                }
                
                break; 

              }

    // video total distance
    case 127: {
                if (
                      (com.isMaster())          && 
                      (com.getSlaveCount() > 0) &&
                      (com_slave_pos == 255)        // selected device: All
                     ) {
                  
                    // invalid and noneditable menu line   
                    ui_get_string( 55, 0); // ----
                    strcpy(value_str, lines[0]);
                    
                } else {
                  
                  if ((editing) && 
                      ((key == KEY_RIGHT) || (key == KEY_LEFT))) {
                    
                    unsigned long stepSize = 1;
                    
                    /*
                    if (key == KEY_RIGHT) {
                      
                      if       (motor_video_dist <  100)                                 stepSize = 1;
                      else if ((motor_video_dist >= 100) && (motor_video_dist < 1000))   stepSize = 10;
                      else                                                               stepSize = 100;
                        
                    } else if (key == KEY_LEFT) {
                      
                      if       (motor_video_dist >  1100)                                stepSize = 100;
                      else if ((motor_video_dist >= 110) && (motor_video_dist < 1100))   stepSize = 10;
                      else                                                               stepSize = 1;
                    
                    }
                    */                  
                    
                    // unsigned long minimum = motor_video_ramp_in + motor_video_ramp_out;
                    unsigned long minimum = 1; 
                             
                    // cut off unwanted granularity                  
                    motor_video_dist = ((unsigned long) (motor_video_dist / (float) stepSize)) * stepSize;     
                    
                    ui_change_value_numeric_float(&motor_video_dist, stepSize, minimum, 0);    
                    
                    // in case there is already a slave selected, send the data
                    if ((com_slave_pos != 0) && 
                         com.isMaster()) {
                    
                      com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 127, motor_video_dist);
                    }
                    
                  }
                                
                  // add the number 
                  itoa(motor_video_dist, value_str, 10);
                  
                  if (motor_is_radial()) {
                    ui_get_string( 20, 0); // °
                  } else {
                    ui_get_string( 63, 0); // cm
                  }
                  
                  strcat(value_str, lines[0]);
                
                }
                
                break; 

              }
    
    // keyframes - set start point           
    case 132: {
      
                if ((editing) && (key == KEY_RIGHT)) {
                    
                  // do we have slaves?
                  if (com.getSlaveCount() > 0) {
                    
                    // master (our device) is currently selected
                    if (com_slave_pos == 0) {
                      core_set_start_keyframe();    
                    }
                    else if (com_slave_pos == 255) {
                      core_set_start_keyframe();
                      com_set_start_keyframe(true);
                    } 
                    else {
                      com_set_start_keyframe(false);    
                    }
                    
                  } else {
                    // set the start-keyframe
                    core_set_start_keyframe();  
                  }
                                    
                } // editing
                
                
                
                // create the output string
                if (com.getSlaveCount() > 0) {
                  if (com_is_start_keyframe_set(com_slave_pos == 255)) ui_get_string( 143, 0); // set [v]!                                    
                  else                                                 ui_get_string(  13, 0); // set [ ]! 
                } else {
                  if (motor_is_key_start_set())                        ui_get_string( 143, 0); // set [v]!    
                  else                                                 ui_get_string(  13, 0); // set [ ]! 
                }
                
                
                strcpy(value_str, lines[0]);
                break;  
              }
              
    // keyframes - set end point           
    case 133: {
                if ((editing) && (key == KEY_RIGHT)) {
                    
                  // do we have slaves?
                  if (com.getSlaveCount() > 0) {
                    
                    // master (our device) is currently selected
                    if (com_slave_pos == 0) {
                      core_set_end_keyframe();    
                    } 
                    else if (com_slave_pos == 255) {
                      core_set_end_keyframe();  
                      com_set_end_keyframe(true);  
                    } 
                    else {
                      com_set_end_keyframe(false);  
                    }
                     
                    
                  } else {
                    // set the start-keyframe
                    core_set_end_keyframe();  
                  }
                  
                } // editing
                
                
                
                // create the output string
                if (com.getSlaveCount() > 0) {
                  if (com_is_end_keyframe_set(com_slave_pos == 255)) ui_get_string( 143, 0); // set [v]!                                    
                  else                                               ui_get_string(  13, 0); // set [ ]! 
                } else {
                  if (motor_is_key_end_set())                        ui_get_string( 143, 0); // set [v]!    
                  else                                               ui_get_string(  13, 0); // set [ ]! 
                }
                
                
                strcpy(value_str, lines[0]);
                break;  
                  
              }
              
    // keyframes - goto start           
    case 134: {
      
                if ((editing) && (key == KEY_RIGHT)) {
                  
                  // do we have slaves?
                  if (com.getSlaveCount() > 0) {
                      
                    // master (our device) is currently selected
                    if (com_slave_pos == 0) {
                      core_move_motor_to_start_pos(true);    
                    } 
                    else if (com_slave_pos == 255) {
                      com_send_command_code(134, true);  
                      core_move_motor_to_start_pos(true);
                    }
                    else {
                      com_send_command_code(134, false);  
                    }
                  
                  } else {
                    // set the start-keyframe
                    core_move_motor_to_start_pos(true);  
                  }     
                  
                } // editing
                                
                ui_get_string(  13, 0);  // go!    
                strcpy(value_str, lines[0]);
                break;  
                                 
              }
              
              
    // syst sleep          
    case 148: {
                
                if (editing) ui_change_value_boolean_bit(&ui_settings, 5);
                
                if (ui_is_system_sleep()) ui_get_string(4, 0);
                else                      ui_get_string(5, 0); 
                                
                strcpy(value_str, lines[0]);
                
                break;          
      
              }
              
    // keep motor powered
    case 149: {
                
                // we are editing the slave value
                if ((com.isMaster()) && 
                    (com.getSlaveCount() > 0) &&
                    (com_slave_pos != 0)) {
                  
                  if (editing &&
                      ((key == KEY_RIGHT) ||(key == KEY_LEFT))
                      ) {
                        
                    // toggle the state    
                    com_motor_powered = !com_motor_powered;
                  }
                      
                  // send the new status to the slave
                  com.sendCommand(com.getSlaveID(), MOCOM_COMMAND_DATA, 149, (byte) com_motor_powered);
                  
                  // get the UI strings                      
                  if (com_motor_powered) {
                    ui_get_string(4, 0);
                  } else {
                    ui_get_string(5, 0); 
                  }         
            
                } // end: edinting slavs values
                
                // we are editing the masters values
                else {
                  
                  if (editing) ui_change_value_boolean_bit(&motor_status, 4);
                
                  if (motor_is_keep_powered()) {
                    ui_get_string(4, 0);
                    
                    // enable the motor (motor wakeup)
                    digitalWrite(DEF_MOTOR_SLEEP_PIN, HIGH);
                  }
                  else {
                    ui_get_string(5, 0); 
                    
                    // disable the motor (motor sleep)
                    digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
                  }       
                  
                } // end: editing masters values
                
                
                strcpy(value_str, lines[0]);
                break;     
      
              }
              
              
    // keyframes - goto end
    case 150: {
      
                if ((editing) && (key == KEY_RIGHT)) {
                  
                  // do we have slaves?
                  if (com.getSlaveCount() > 0) {
                    
                  
                    
                    // master (our device) is currently selected
                    if (com_slave_pos == 0) {
                      core_move_motor_to_end_pos();    
                    } 
                    else if (com_slave_pos == 255) {
                      com_send_command_code(150, true);  
                      core_move_motor_to_end_pos();    
                    }
                    else {
                      com_send_command_code(150, false);  
                    }
                  
                  } else {
                    // set the start-keyframe
                    core_move_motor_to_end_pos();  
                  }     
                
                }
                
                
                // create the output string
                ui_get_string(  13, 0);  // go!    
                strcpy(value_str, lines[0]);
                break;  
                                 
              }
      
    
    // bouncing / loop
    case 156: {
                
                if (editing) ui_change_value_boolean_bit(&motor_key_status, 2);  // Bit 2
                              
                if (isBit(motor_key_status, BIT_2)) {
                  ui_get_string( 4, 0); // enabled    
                } else {
                  ui_get_string( 5, 0); // disabled   
                }
                
                strcpy(value_str, lines[0]);
                break; 
                        
              }
     
    // Dragonframe       
    case 171: { 
                if ((editing) && (key == KEY_RIGHT)) {
                  
                  // initialize the dragonframe module
                  // and send true for saving the reboot-flag     
                  dragonframe_init(true);
                  
                  // print the message for the dragonframe 
                  ui_init_dragonframe_message();
                  ui_print_message();
                  
                  // jump to the dragonframe module
                  dragonframe_process();
                  
                }
                
                ui_get_string( 13, 0); // ->    
                strcpy(value_str, lines[0]);
                break;   
       
       
     }
     
       
  } // switch
  
  
  byte text_width = GLCD.StringWidth(value_str);
  
  // paint the text line
  GLCD.CursorToXY(GLCD.Width - 5 - text_width, yPos);
   
  GLCD.print(value_str);
  
}





// ======================================================================================
// this function changes the exposure value and returns a string
// ======================================================================================
void ui_change_exposure_value(unsigned long *value, unsigned long minimum, unsigned long maximum) {

  byte exp_num;
  unsigned long stepSize;
  
  if (key == KEY_RIGHT) {
    if       (*value <  1000L)                         stepSize = 1;
    else if ((*value >= 1000L)  && (*value < 30000L))  stepSize = 500;
    else if ((*value >= 30000L) && (*value < 60000L))  stepSize = 1000;
    else if ((*value >= 60000L) && (*value < 600000L)) stepSize = 10000;
    else                                               stepSize = 60000;
  }
  else if (key == KEY_LEFT) {
    if       (*value >= 660000L)                       stepSize = 60000;
    else if ((*value >= 61000L) && (*value < 660000L)) stepSize = 10000;
    else if ((*value >= 31000L) && (*value < 61000L))  stepSize = 1000;
    else if ((*value >= 2500L)  && (*value < 31000L))  stepSize = 500;
    else                                               stepSize = 1;
    
  }
  
  if (*value <= 2000) {
        
    // ensure that the values meet the given exposure values
    // also remember which one it is (exp_num)
    for (int i = 0; i < exposure_value_count; i++) {
      
      if (*value <= exposure_values[i]) {
        
        *value = exposure_values[i];
        exp_num = i;    
        break;
      }
    }
  }
  
    
  // if key pressed
  if ((key == KEY_RIGHT) &&  
      ((*value < maximum) || (maximum == 0))
     ) {
    
    // do we need specific exposure values (when shorter than 1 sec)?    
    if (*value < 2000) {
      
      if (exp_num < exposure_value_count - 1) {
        *value = exposure_values[exp_num + 1];
      } else {
        *value = 2000;
      }  
           
      
    } else { // value >= 2000
             
      // no maximum
      if (maximum == 0) {
        
        *value += stepSize + (stepSize * key_count);   
        
      } 
      // maximum exists
      else {
      
        // would our next step exceed the maximum limit?           
        if ((maximum - *value) > (key_count * stepSize)) {
          *value += stepSize + ((float) stepSize * (float) key_count);
        } else {
          *value = maximum;  
        }
      }
      
      // cut the rest (only granularity as big as the stepSize)
      *value = ((unsigned long) *value / (unsigned long) stepSize) * (unsigned long) stepSize;
      
    }
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
    
    
  } else if ((key == KEY_LEFT) &&
             (*value > minimum)) {
    
    if (*value <= 2000) {

      if (exp_num > 0) {
        *value = exposure_values[exp_num - 1];
      }  

    } else { // value > 2000       
               
      // would our next step exceed the minimum limit?           
      if ((*value - minimum) > (key_count * stepSize)) {
        *value -= stepSize + ((float) stepSize * (float) key_count);
      } else {
        *value = minimum;  
      }
      
      // cut the rest (only granularity as big as the stepSize)
      *value = ((unsigned long) *value / (unsigned long) stepSize) * (unsigned long) stepSize;
    }
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
  }
  
  
  // check if the exposure value is longer than the current cycle length
  if (*value > cycle_length) {
    *value = cycle_length;
  }   
  
}





  

// ======================================================================================
// this function generates the value string for exposure values
// ======================================================================================
char* ui_generate_exposure_value_string(unsigned long *value) {
  
  char res_str[11];
  
  if (*value < 300) {
    
    byte exp_num;
    
    // determine which exposure value we have 
    for (int i = 0; i < exposure_value_count; i++) {
        
      if (*value <= exposure_values[i]) {
        exp_num = i;    
        break;
      }
    }    
    
    ui_get_string(115 + exp_num, 0);
    strcpy(res_str, lines[0]);
    
    
  } else { // >= 300
    
    char temp[11];
    
    if (*value < 60000L) {  // 0.0"
      
      // calculate the second value as well as the rest
      float fSecs = ((float) *value) / 1000.0;
      byte secs = fSecs;
      byte rest = (fSecs * 10.0) - (float) (secs * 10);
      
      // add the seconds to the string
      itoa(secs, res_str, 10);
      
      // add a dot
      strcat(res_str, dot);
      
      // add the rest
      itoa(rest, temp, 10);
      strcat(res_str, temp);
     
     
    } else {  // 00:00"
      
      unsigned long minutes = *value / 60000L;
      unsigned long seconds = (*value - (minutes * 60000L)) / 1000L;
      
      // convert the minutes to a temp string
      itoa(minutes, temp, 10);
      
      // add a leading zero if needed
      if (minutes < 10) {
        strcpy(res_str, noll);
        strcat(res_str, temp);
      }
      else strcpy(res_str, temp);
      
      // add :
      ui_get_string(39, 0);  // :
      strcat(res_str, lines[0]);
      
      // convert the seconds to a temp string
      itoa(seconds, temp, 10);
      
      // add leading zero if needed
      if (seconds < 10) strcat(res_str, noll);
      strcat(res_str, temp);
      
      
    }  
    
    // add "
    ui_get_string(68, 0); // "
    strcat(res_str, lines[0]);
    
  }
 
  return res_str;
  
}


// ======================================================================================
// this function changes a numeric byte value
// ======================================================================================
void ui_change_value_numeric_ubyte(byte *value, byte stepSize, byte minimum, byte maximum, boolean loopValues) {
 
  // if key pressed
  if ((key == KEY_RIGHT) &&  
      ((*value <= maximum) || (maximum == 0) || loopValues)
     ) {
    
    // no maximum
    if (maximum == 0) {
      
      *value += stepSize + (stepSize * key_count);   
      
    } 
    // maximum exists
    else {
    
      // would our next step exceed the maximum limit?           
      if ((maximum - *value) > (key_count * stepSize)) {
        
        *value += stepSize + ((float) stepSize * (float) key_count);
        
      } else if (loopValues) {
        *value = minimum; 
      }  else {
        *value = maximum; 
      }
    }
    
    // cut the rest (only granularity as big as the stepSize)
    *value = ((unsigned byte) *value / (unsigned byte) stepSize) * (unsigned byte) stepSize;
        
    // set the settings were changed flag
    ui_set_settings_changed_flag();
    
  } else if ((key == KEY_LEFT) &&
             ((*value >= minimum) || loopValues)) {
    
    // would our next step exceed the minimum limit?           
    if ((*value - minimum) > (key_count * stepSize)) {
      
      *value -= stepSize + ((float) stepSize * (float) key_count);
      
    } else if (loopValues) {
      *value = maximum;
    } else {
      *value = minimum;  
    }
    
    // cut the rest (only granularity as big as the stepSize)
    *value = ((unsigned byte) *value / (unsigned byte) stepSize) * (unsigned byte) stepSize;
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
  }
  
}


// ======================================================================================
// this function changes a numeric int value
// ======================================================================================
void ui_change_value_numeric_uint(unsigned int *value, int stepSize, unsigned int minimum, unsigned int maximum) {
 
  
  // if key pressed
  if ((key == KEY_RIGHT) &&  
      ((*value < maximum) || (maximum == 0))
     ) {
    
    // no maximum
    if (maximum == 0) {
      
      *value += stepSize + (stepSize * key_count);   
      
    } 
    // maximum exists
    else {
    
      // would our next step exceed the maximum limit?           
      if ((maximum - *value) > (key_count * stepSize)) {
        *value += stepSize + ((float) stepSize * (float) key_count);
      } else {
        *value = maximum;  
      }
    }
    
    // cut the rest (only granularity as big as the stepSize)
    *value = ((unsigned int) *value / (unsigned int) stepSize) * (unsigned int) stepSize;
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
    
  } else if ((key == KEY_LEFT) &&
             (*value > minimum)) {
    
    // would our next step exceed the minimum limit?           
    if ((*value - minimum) > (key_count * stepSize)) {
      *value -= stepSize + ((float) stepSize * (float) key_count);
    } else {
      *value = minimum;  
    }
    
    // cut the rest (only granularity as big as the stepSize)
    *value = ((unsigned int) *value / (unsigned int) stepSize) * (unsigned int) stepSize;
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
  }
  
}

// ======================================================================================
// this function changes a numeric long value
// ======================================================================================
void ui_change_value_numeric_ulong(unsigned long *value, long stepSize, unsigned long minimum, unsigned long maximum) {
 
  // if key pressed
  if ((key == KEY_RIGHT) &&  
      ((*value < maximum) || (maximum == 0))
     ) {
       
    // no maximum
    if (maximum == 0) {
      
      *value += stepSize + (stepSize * key_count);   
      
    } 
    // maximum exists
    else {
    
      // would our next step exceed the maximum limit?           
      if ((maximum - *value) > (key_count * stepSize)) {
        *value += stepSize + ((float) stepSize * (float) key_count);
      } else {
        *value = maximum;  
      }
    }
    
    // cut the rest (only granularity as big as the stepSize)
    *value = ((unsigned long) *value / (unsigned long) stepSize) * (unsigned long) stepSize;
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
    
  } else if ((key == KEY_LEFT) &&
             (*value > minimum)) {
               
    // would our next step exceed the minimum limit?           
    if ((*value - minimum) > (key_count * stepSize)) {
      *value -= stepSize + ((float) stepSize * (float) key_count);
    } else {
      *value = minimum;  
    }
    
    // cut the rest (only granularity as big as the stepSize)
    *value = ((unsigned long) *value / (unsigned long) stepSize) * (unsigned long) stepSize;
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
  }

}




// ======================================================================================
// this function changes a numeric float value
// ======================================================================================
void ui_change_value_numeric_float(float *value, float stepSize, float minimum, float maximum) {
  
  // if key pressed
  if ((key == KEY_RIGHT) &&  
      ((*value < maximum) || (maximum == 0))
     ) {
    
    // no maximum
    if (maximum == 0) {
      
      *value += stepSize + (stepSize * key_count);   
      
    } 
    // maximum exists
    else {
    
      // would our next step exceed the maximum limit?           
      if ((maximum - *value) > (key_count * stepSize)) {
        *value += stepSize + ((float) stepSize * (float) key_count);
      } else {
        *value = maximum;  
      }
    }
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
    
  } else if ((key == KEY_LEFT) &&
             (*value > minimum)) {
    
    // would our next step exceed the minimum limit?           
    if ((*value - minimum) > (key_count * stepSize)) {
      *value -= stepSize + ((float) stepSize * (float) key_count);
    } else {
      *value = minimum;  
    }
    
    // Avoid values just a little bit bigger than the minimum
    if (*value < (minimum + stepSize)) *value = minimum;
        
    // set the settings were changed flag
    ui_set_settings_changed_flag();
  }

}


// ======================================================================================
// this function changes a single bit in a given byte
// ======================================================================================
void ui_change_value_boolean_bit(byte *value, byte bit_num) {
  
  byte check_byte = 1 << bit_num;
  
  // if key pressed
  if ((key == KEY_RIGHT) || 
      (key == KEY_LEFT)) {
    
    // toggle the bit
    *value ^= check_byte;  
    
    // set the settings were changed flag
    ui_set_settings_changed_flag();
  }
  
}


// ======================================================================================
// this function paints the power status to the given position on the screen
// ======================================================================================
void ui_paint_power_state(byte x, byte y, boolean inverted) {
  
  // there seems to be a bug in the GLCD drawing routine that adds a 
  // border to the right and also to the bottom side of the picture)
  
  // external power supply plugged?
  if (power_is_external_supply_connected()) {
    
    if (inverted) {
      
      GLCD.DrawBitmap(plugIcon, x, y, BLACK);
      // bitmap is 5x8px but the actual icon 5x9px. We now paint the last line
      GLCD.DrawHLine(x, y+8, 4, WHITE);
      GLCD.SetDot(x+2, y+8, BLACK);
      
    } else {
      
      GLCD.DrawBitmap(plugIcon, x, y, WHITE);
      // bitmap is 5x8px but the actual icon 5x9px. We now paint the last line
      GLCD.DrawHLine(x, y+8, 4, BLACK);
      GLCD.SetDot(x+2, y+8, WHITE);
      
    }
    
    
  } else {
    
    byte level = power_get_battery_level();
    
    if (inverted) {
      
      GLCD.DrawBitmap(batteryIcon, x, y, BLACK);
      // bitmap is 5x8px but the actual icon 5x9px. We now paint the last line
     
      // fill the battery
      GLCD.FillRect( x+1, y + (8 - level), 
                     2, level, BLACK);
    
      // fix the picture
      GLCD.DrawLine(x, y+8, x+4, y+8, BLACK);
    
    } else {
      
      GLCD.DrawBitmap(batteryIcon, x, y, WHITE);
      // bitmap is 5x8px but the actual icon 5x9px. We now paint the last line
     
      // fill the battery
      GLCD.FillRect( x+1, y + (8 - level), 
                     2, level, WHITE);
    }
      
  }
  
  if (inverted) {
  
    // remove the right border line
    GLCD.DrawVLine(x+5, y, 9, WHITE);
  
  } else {
      
    // remove the right border line
    GLCD.DrawVLine(x+5, y, 9, BLACK);
  
  }
  
}






// ======================================================================================
// initializes the stop-motion message screen
// ======================================================================================
void ui_init_stop_motion_message() {
  
  ui_generate_message(165, 166, 170, 255);
}

// ======================================================================================
// initializes the general information screen
// ======================================================================================
void ui_init_info_message() {
  
  ui_generate_message(70, 71, 85, 255);
}

// ======================================================================================
// initializes the "motor-home is now set" message
// ======================================================================================
void ui_init_motor_set_home_message() {  

  ui_generate_message(91, 91, 92, 1);
}

// ======================================================================================
// initializes the "motor-home is reached" message
// ======================================================================================
void ui_init_motor_home_reached_message() {
  
  ui_generate_message(91, 96, 97, 1);  
}

// ======================================================================================
// initializes the "please while while motor is moved home" message
// ======================================================================================
void ui_init_motor_move_home_message() {

  ui_generate_message(91, 93, 95, 255);  
}

// ======================================================================================
// initializes the "system settings reset - now reboot" message
// ======================================================================================
void ui_init_reset_message() {

  ui_generate_message(98, 99, 101, 255);  
}


// ======================================================================================
// initializes the "engine starting..." message
// ======================================================================================
void ui_init_engine_start_message() {
  
  if (com.isMaster()) {
    ui_generate_message(112, 160, 160, 1);
  } else {
    ui_generate_message(112, 160, 164, 1);
  }
  
}

// ======================================================================================
// initializes the "keyframe is now set" message
// ======================================================================================
void ui_init_motor_key_set_message() {
  
  ui_generate_message(135, 136, 136, 1);
}

// ======================================================================================
// initializes the "Moving motor to start position" message
// ======================================================================================
void ui_init_motor_key_move_to_message() {
 
  ui_generate_message(135, 137, 139, 255);
}


// ======================================================================================
// initializes the "Updating chaing data" message
// ======================================================================================
void ui_init_update_chain_info_message() {
 
  ui_generate_message(105, 151, 153, 255);
}

// ======================================================================================
// initializes the "start position reached" message
// ======================================================================================
void ui_init_motor_key_reached_message() {
 
  ui_generate_message(135, 140, 141, 1);
}

// ======================================================================================
// initializes the "no start point was set" message
// ======================================================================================
void ui_init_motor_key_not_set_message() {
 
  ui_generate_message(135, 144, 145, 1);
}


// ======================================================================================
// initializes the "not all start points were set" message
// ======================================================================================
void ui_init_motor_key_not_all_points_set_message() {
 
  ui_generate_message(135, 146, 147, 1);
}


// ======================================================================================
// initializes the "Registered in the chain" message
// ======================================================================================
void ui_init_registerd_in_chain_message() {
 
  ui_generate_message(105, 158, 159, 1);
}


// ======================================================================================
// initializes the "..in Dragonframe Mode" message
// ======================================================================================
void ui_init_dragonframe_message() {
  ui_generate_message(85, 86, 89, 255);
}



// ===================================================================================
// Prints a message to the screen
// duration = 255 means message stays until left, right, menu or select is pressed
// ===================================================================================
void ui_print_message() {
  
  // paint the base header
  ui_paint_base_header();
  
  // paint the icon
  GLCD.DrawBitmap(messageIcon, 4, 1, WHITE);
    
  // bitmap is 12x8px but the actual icon 12x9px printed with som errors
  GLCD.DrawHLine(4, 9, 12, BLACK);
  GLCD.DrawVLine(16, 1, 8, BLACK);
  GLCD.SetDot(11, 9, WHITE);
  
  
  // print title name
  GLCD.CursorToXY(21, 2);
  ui_get_string(line_codes[0], 0);
  GLCD.print(lines[0]);
  
  // paint the content  
  GLCD.SetFontColor( BLACK );
  
  // paint the actual message
  for (int i=0; i<menu_max_screen_lines; i++) {
    
    // get the string for this line (+1 because the first line contains the title)
    ui_get_string(line_codes[i + menu_cursor_pos + 1], 0);
    
    // clear the area of the screen
    GLCD.FillRect( 0,  14 + (i*10) - 1, GLCD.Width, 9, WHITE);
    
    // only paint text if there is still content
    if (menu_length > i) {  
      // paint the text line
      GLCD.CursorToXY(2, 14 + (i*10));
      GLCD.print(lines[0]);   
    }
   
  }
  
  GLCD.FillRect( 0, header_height + 1, GLCD.Width, 1, WHITE);
  
  // paint the scroll bar if more than 5 menu lines
  ui_paint_scrollbar(menu_length > 5, true);
  
  // delete the "message to be painted" flag
  ui_delete_message_in_queue_flag();
  
}





