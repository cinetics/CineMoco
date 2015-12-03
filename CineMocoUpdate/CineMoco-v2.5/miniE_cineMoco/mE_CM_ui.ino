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


  // the max number of lines that can be displayed on one menu screen
#define menu_screen_line_height    10   // height of each menu line in px 
#define menu_max_screen_lines      5    // amount of line that can be displayed at once
#define header_height              10   // height of header in px




  const byte menu_relation_count = 64; 


byte menu_content_relations[menu_relation_count][2] = { 
    
      {  11,   0 }, {  11,  66 }, {  11,  30 }, {  11,  36 }, {  11,  32 }, {  11,  33 }, {  11,  34 }, {  11,   7 }, {  11,  23 }, {  11,  56 },                              // status screen advanced (timelapse - run based)
      {  12,   0 }, {  12,  66 }, {  12, 132 }, {  12, 133 }, {  12, 134 }, {  12, 150 }, {  12,  36 }, {  12,  32 }, {  12,  33 }, {  12,  34 }, {  12,   7 }, {  12,  56 },  // status screen advanced (timelapse - keyframe based) 
      {  15,   0 }, {  15,  66 }, {  15, 127 }, {  15, 124 }, {  15,  23 }, {  15, 125 }, {  15, 126 },                                                                        // status screen advanced (video mode - run based)
      {  16,   0 }, {  16,  66 }, {  16, 132 }, {  16, 133 }, {  16, 134 }, {  16, 150 }, {  16, 124 }, {  16, 125 }, {  16, 126 },                                            // status screen advanced (video mode - keyframe based)
      
      {  20,  25 }, {  20,  54 }, {  20, 156 }, {  20,  59 }, {  20,  58 }, {  20,  29 },                                                                                      // settings advanced GENeral 
      {  21,  40 }, {  21, 148 }, {  21, 102 }, {  21, 106 }, {  21, 171 }, {  21,  57 },                                                                                      // settings advanced CONtroller
  /*{  22,  46 }, {  22,  47 },*/ {  22,  45 }, {  22,  42 }, {  22,  24 }, {  22, 149 }, {  22,  22 },                                                                        // settings advanced MoToR
      {  23,  26 }, {  23,  27 }, {  23,  43 }, {  23,  48 }, {  23, 108 }, {  23,  49 }, {  23,  50 }, {  23,  51 }, {  23,  52 }                                             // settings advanced CAM
};
      





  // main position in our UI.
  // we start with position 0 (status screen).
byte main_pos = 0;

  // latest main position in our UI.
  // This is needed for optimized action screen painting
byte main_pos_old = 255;

  // variable to detect program running changes
  // This is needed for optimized screen painting
boolean program_running_old = false;

  // position of the cursor on the screen. this is needed to paint verticall scrolling
byte menu_cursor_pos = 0;

  // position in the menu 
byte menu_pos = 0;

  // count of the current menu/text lines
byte menu_length;

  // this variable determines if we are on the right side of an menu, editing a value
//boolean menu_editing = false;

  // variable to toggle set / set all (used for daisy chaingin)
//boolean menu_set_all = false;

  // helping variable to determine if the menu style changed
boolean menu_style_changed = false;


  // status screen, timed-program and clock change memory
unsigned int camera_shoot_count_old = 0;
unsigned int cycle_length_old = 0;
byte action_status_old = B0;
byte battery_level_old = 1023;
byte power_status_old = 255;


  // variable to remember when a key was pressed
unsigned long key_time;

  // the pressed key - not the global key-pressed buffer but only one key from that buffer
byte pressed_key;




// ======================================================================================

//boolean ui_is_setup_style_cycle()        { return (ui_setup_style == DEF_SETUP_STYLE_CYCLE); }
boolean ui_is_setup_style_run()          { return (ui_setup_style == DEF_SETUP_STYLE_RUN); }
boolean ui_is_setup_style_keyframes()    { return (ui_setup_style == DEF_SETUP_STYLE_KEYFRAMES); }
void ui_set_setup_style(byte style)      { ui_setup_style = style; }
       

boolean ui_is_backlight_flag()           { return ui_status & B10000000; }
void ui_set_backlight_flag()             { ui_status |= B10000000; }
void ui_delete_backlight_flag()          { bitClear(ui_status, 7); }  // B01111111

boolean ui_is_repaint_flag()             { return ui_status & B01000000; }
void ui_set_repaint_flag()               { ui_status |= B01000000; }
void ui_delete_repaint_flag()            { bitClear(ui_status, 6); }  // B10111111

boolean ui_is_save_settings_flag()       { return ui_status & B00100000; }
void ui_set_save_settings_flag()         { ui_status |= B00100000; }
void ui_delete_save_settings_flag()      { bitClear(ui_status, 5); }  // B11011111

boolean ui_is_settings_changed_flag()    { return ui_status & B00010000; }
void ui_set_settings_changed_flag()      { ui_status |= B00010000; }
void ui_delete_settings_changed_flag()   { bitClear(ui_status, 4); } // B11101111

//boolean ui_is_repaint_header_flag()      { return ui_status & B00001000; }
//void ui_set_repaint_header_flag()        { ui_status |= B00001000; }
//void ui_delete_repaint_header_flag()     { bitClear(ui_status, 3); }  // B11110111

boolean ui_is_long_key_press()           { return ui_status & B00000100; }
void ui_set_long_key_press()             { ui_status |= B00000100; }
void ui_delete_long_key_press()          { bitClear(ui_status, 2); }  // B11111011

boolean ui_is_message_on_screen()        { return ui_status & B00000010; }
void ui_set_message_on_screen_flag()     { ui_status |= B00000010; }
void ui_delete_message_on_screen_flag()  { bitClear(ui_status, 1); } // B11111101

boolean ui_is_message_in_queue()         { return ui_status & B00000001; }
void ui_set_message_in_queue_flag()      { ui_status |= B00000001; }
void ui_delete_message_in_queue_flag()   { bitClear(ui_status, 0); } // B11111110


boolean ui_is_settings_autosave()        { return ui_settings & BIT_6; }
void ui_set_settings_autosave()          { ui_settings |= BIT_6; }
void ui_delete_settings_autosave()       { deleteBit(ui_settings, BIT_6); } // B10111111

boolean ui_is_system_sleep()             { return ui_settings & B00100000; }

boolean ui_is_cursor_in_move_section()   { return ((menu_pos == 0) && (main_pos == 0)); } 


boolean ui_is_full_repaint_flag()        { return isBit(ui_status, BIT_8); }
void ui_set_full_repaint_flag()          { setBit(ui_status, BIT_8); }
void ui_delete_full_repaint_flag()       { deleteBit(ui_status, BIT_8); }

boolean ui_is_program_done_screen()      { return isBit(ui_status, BIT_10); }
void ui_set_program_done_screen()        { setBit(ui_status, BIT_10); }
void ui_delete_program_done_screen()     { deleteBit(ui_status, BIT_10); }

boolean ui_is_paint_shoot_count_flag()   { return isBit(ui_status, BIT_11); }
void ui_set_paint_shoot_count_flag()     { setBit(ui_status, BIT_11); }
void ui_delete_paint_shoot_count_flag()  { deleteBit(ui_status, BIT_11); }

boolean ui_is_paint_battery_flag()       { return isBit(ui_status, BIT_12); }
void ui_set_paint_battery_flag()         { setBit(ui_status, BIT_12); }
void ui_delete_paint_battery_flag()      { deleteBit(ui_status, BIT_12); }


 


// ======================================================================================
// this function is called from the main loop and handles key inputs, backlight and
// initiates all further screen action that needs to be done.
// ======================================================================================
void ui_do_screen() {

  now = millis();
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
      
  // get the pressed button (either from the keys or via daisy chaining)
  pressed_key = keys_get_key_pressed();  
  if (pressed_key == KEY_NONE) {
    pressed_key = com_get_received_key();
  }

  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

  // is a message displed and 
  // should message time exceed?
  if ((ui_is_message_on_screen()) &&
      (message_duration != 255)) {
        
    // is message time exceeded? 
    if ( message_start_time + ((unsigned long) message_duration * 1000UL) <= now ) {

      ui_end_message();    
    } 
     
  } 


  // if the programm is running and we are in contiuous mode
  motor_continuous_check();


  // we have a pressed key!
  if (pressed_key != KEY_NONE) {
    
    // reset the user action time 
    interaction_time = now;  
    key_time = now;    
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // is backlight off? 
    // only do something if the backlight was allready on.
    // first press then when backlight is off
    // is supposed to turn on the light and nothing else.
    if (ui_is_backlight_flag()) {
      
      // if the programm is running and we are in contiuous mode
      motor_continuous_check();
      
      // set the repaint flag 
      ui_set_repaint_flag(); 
      
      // if the programm is running and we are in contiuous mode
      motor_continuous_check();
        
      // do something - corresponding to the pressed key 
      ui_key_action(pressed_key);
      
      // if the programm is running and we are in contiuous mode
      motor_continuous_check();
            
    } 
    else { // turn backlight on again on any key press

      // delete the key press to avoid any action out of turning on the BL
      pressed_key = KEY_NONE;   
      
      // if the programm is running and we are in contiuous mode
      motor_continuous_check();
        
      // turn backlight on
      ui_trigger_backlight();
      
      // do a full repaint
      ui_set_full_repaint_flag();
      
    }

  } // end keypress


  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

  // status update needed? do this only if we are not planning
  // to repaint the screen (in this case it will be repainted anyway) 
  // and no message is on the screen. 
  if ((main_pos == 0) &&
     (!ui_is_repaint_flag()) && 
     (!ui_is_message_on_screen())) {
    
    ui_check_value_stati();
      
  }

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

  // if we need to paint a message
  if (ui_is_message_in_queue()) {
    
    // delete the repaint flag B01000000
    ui_delete_repaint_flag();
    
    // paint the message
    ui_print_message();
    
  }

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

  // do we need to repaint? 
  if (ui_is_repaint_flag()) {

    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // delete the repaint flag B01000000
    ui_delete_repaint_flag();
    
    // paint the screen
    ui_paint_screen(pressed_key);
   
  } 
  
    
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();


  // if backlight is on, check if it needs to be turned off
  // after some time of inactivity
  if (ui_is_backlight_flag()) {

    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // did we wait our prefered time since last key action?  
    if ((key_time + (unsigned long) backlight_wait * 1000UL) < now) {

      // if the programm is running and we are in contiuous mode
      motor_continuous_check();
        
      ui_disable_backlight();
    } 
  }
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
    
  // need the settings to be saved?
  if (ui_is_save_settings_flag()) {
    
    eeprom_write_config();
    ui_delete_save_settings_flag();  
  }
  
} 



// ======================================================================================
// Here we react on a key press and move through the UI tree
// ======================================================================================
void ui_key_action(int key) {
  
  
  // =============
  // KEY SELECT
  if (key == KEY_SELECT) {
    
    // message on screen?
    if (ui_is_message_on_screen()) {
 
      ui_end_message();  

    }
    
    // we are on the "program done" screen
    else if (ui_is_program_done_screen()) {
    
      // delete the "program done screen" flag
      ui_delete_program_done_screen();  
      
      // do a full repaint
      ui_set_repaint_flag();
      ui_set_full_repaint_flag();
            
    }
  
    // no message on screen and not "program done" screen
    else {
      
      // program running?
      if (core_is_program_running()) {
        
        core_stop_program(com.isMaster(), false);
        
        // do a full repaint
        ui_set_repaint_flag();
        ui_set_full_repaint_flag();
        
      } else {
        
        if (main_pos == 0) {
          
          if (!core_is_program_running()) {
            core_start_program();
          }
          
        } else {
        
          // go back to main screen
          ui_go_to_main_screen();
          
          ui_check_autosave();
          
        }
      
      }   
      
    }
    
  }
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // only accept menu UI input when engine is stopped
  // and ww are not on the done - screen
  if (!core_is_program_running() &&
      !ui_is_program_done_screen()) {
    
    // =============
    // KEY MENU
    if (key == KEY_MENU) {
        
      // remember our old screen position
      main_pos_old = main_pos;
      
      main_pos++;          
      if (main_pos == 5) main_pos = 1;
        
      menu_pos = 0;
      menu_cursor_pos = 0;
      //menu_editing = false;
      
    }
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // =============
    // KEY UP
    if (key == KEY_UP) {
  
      // if we are not editing a value or on a message screen
      //if ((!menu_editing) || 
      //    (ui_is_message_on_screen())) {
      
      if (menu_pos > 0)  menu_pos--; 
      if (menu_cursor_pos > 0)  menu_cursor_pos--; 
      
      // check if we need to save the settings      
      ui_check_autosave();
      
    }
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // =============
    // KEY DOWN
    if (key == KEY_DOWN) {
  
      // if we are not editing a value or on a message screen
      //if ((!menu_editing) || 
      //    (ui_is_message_on_screen())) {
      
      
      
      if (menu_pos < menu_length - 1)  menu_pos++; 
    
      // if a message is on the screen, we have a different scrolling behavior.
      // In this case there is no cursor on the screen that is used for scrolling but
      // the full screen is scrolled on every up or down event:
      
      // message on screen?
      if (ui_is_message_on_screen()) {
        if (menu_cursor_pos < (menu_length - menu_max_screen_lines - 1)) 
        
          menu_cursor_pos++; 
          
          //menu_set_all = false;
          ui_check_autosave();
      }
      // no message on the screen - cursor based scrolling
      else {  
        if ((menu_cursor_pos < (menu_max_screen_lines - 1)) && 
            (menu_cursor_pos < menu_length - 1)) 
      
          menu_cursor_pos++; 
          
          //menu_set_all = false;
          ui_check_autosave();
       
      } 
       
    }
      
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();  
      
    
    // =============
    // KEY LEFT  
    if (key == KEY_LEFT) {
      
      // are we right now moving the motor?
      if (ui_is_cursor_in_move_section()) {
        
        ui_paint_move_section(key);
        
        // do we have a client selected and are we a master?
        
        
        if (com.isMaster() &&
            (com_slave_pos > 0)) {
          
          boolean all = com_slave_pos == 255;
              
          // send the key value to the slave/s
          com_send_key(key, all);
          
          // if all motors should move, then move the master as well
          if (all) {
            motor_jog(key, DEF_RAMPING_ATANGENS);   
          }
          
          // wait until the key was released
          while(keys_read_key_left());
          
          //send the key release command
          com_send_key_release(all);
                    
        } else {
          motor_jog(key, DEF_RAMPING_ATANGENS);   
        }
        
        
        /*
        if ((com_slave_pos != 0) && 
             com.isMaster()) {
               
          // send the key value to the slave
          com_send_key(key);
          
          // wait until the key was released
          while(keys_read_key_left());
          
          //send the key release command
          com_send_key_release();
          
        } else {
          motor_jog(key, DEF_RAMPING_ATANGENS); 
        }
        */
        
        

        ui_paint_move_section(KEY_NONE);
        ui_delete_repaint_flag();
                      
      } // end: in move section
      
      else {
                        
        // message on screen and not in stop-motion wait?
        if (ui_is_message_on_screen()) {
            //&& (!core_is_in_stop_motion_wait)) {
              
          ui_end_message();
        } 
      
      }
    
    }  
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    // =============
    // KEY RIGTH  
    if (key == KEY_RIGHT) {
    
      // are we right now moving the motor?
      if (ui_is_cursor_in_move_section()) {
        
        ui_paint_move_section(key);
        
        
        
        if (com.isMaster() &&
            (com_slave_pos > 0)) {
          
          boolean all = (com_slave_pos == 255);
              
          // send the key value to the slave/s
          com_send_key(key, all);
          
          // if all motors should move, then move the master as well
          if (all) {
            motor_jog(key, DEF_RAMPING_ATANGENS);   
          }
          
          // wait until the key was released
          while(keys_read_key_right());
          
          //send the key release command
          com_send_key_release(all);
                    
        } else {
          motor_jog(key, DEF_RAMPING_ATANGENS);   
        }
        
        
        /*
        // do we have a client selected and are we a master?
        if ((com_slave_pos != 0) && 
             com.isMaster()) {
               
          // send the key value to the slave
          com_send_key(key);
          
          // wait until the key was released
          while(keys_read_key_right());
          
          //send the key release command
          com_send_key_release();
          
        } else {
          motor_jog(key, DEF_RAMPING_ATANGENS);
        }
        */
        
        
        ui_paint_move_section(KEY_NONE);
        ui_delete_repaint_flag();
        
      } // end: in move section
      
      else {
                        
        // message on screen and not in stop-motion wait?
        if (ui_is_message_on_screen()) { 
           // && (!core_is_in_stop_motion_wait)) {
              
          ui_end_message();
        } 
      
      }
      
    }
    
  } // engine is stopped   
  

}



// ======================================================================================
// paints the screen
// ======================================================================================
void ui_paint_screen(byte pressed_key) {
  
  // -----------------------------------
  // message on screen?
  if (ui_is_message_on_screen()) {

    ui_print_message();
    
  } 
  
  // -----------------------------------
  // paint the dashboard (program is running)
  else if (core_is_program_running()) {
    
    // full repaint?
    if (ui_is_full_repaint_flag()) {
      
      // paint the "program running" dashboard
      ui_paint_dashboard();
        
    } else {
      
      // paint the shoot count
      if (ui_is_paint_shoot_count_flag()) {
        ui_delete_paint_shoot_count_flag();
        ui_paint_shoot_count(true);   
      }
      
      // paint the battery
      if (ui_is_paint_battery_flag()) {
        ui_delete_paint_battery_flag();
        ui_paint_power_state(100, 37, true);
      }
      
      
      
    }
        
  }
  
  // -----------------------------------
  // paint the message that the program is done
  else if (ui_is_program_done_screen()) {
    
    // paint the "program is done" screen
    ui_paint_done_screen();  
    
  }
  
    
  // regular painting
  else {
          
    // load the menu strings for the current UI position to our lines array
    ui_load_menu_strings();
      
    // paint first line on the main screen only when not in setup-style-keyframes mode
    // if so we will paint the move system at that position
    boolean paint_first_line = !(main_pos == 0);
          
    // paint the menu (value names)
    boolean first_line_on_screen = ui_paint_menu(pressed_key, paint_first_line);
    
    // paint the move part if needed
    if (first_line_on_screen && (!paint_first_line)) {
      ui_paint_move_section(pressed_key);      
    }
    
    // paint the scroll bar if more than 5 menu lines
    ui_paint_scrollbar(menu_length > 5, false);
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
          
    // status screen
    if (main_pos == 0) {
      
      // if the programm is running and we are in contiuous mode
      motor_continuous_check();
      
      // only paint the header if we had a screen change or 
      // the program state changed or
      // we want a full repaint
      if ((main_pos_old != main_pos) ||
          (core_is_program_running() != program_running_old) ||  
          (ui_is_full_repaint_flag())) {
            
        main_pos_old = main_pos;
        program_running_old = core_is_program_running();
      
        // paint advanced header extras if not in video mode where we don't need them
        if (core_is_mode_video()) {
          ui_paint_status_header_base();
          ui_paint_video_hint();
        } else {
          ui_paint_advanced_status_header(); 
        }
      }
    
    }
    // settings screen
    else {
      
      // only paint the header if we had a screen change or 
      // the program state chenged or
      // we want a full repaint
      if ((main_pos_old != main_pos) ||
          (core_is_program_running() != program_running_old) ||  
          (ui_is_full_repaint_flag())) {
            
        main_pos_old = main_pos;
        program_running_old = core_is_program_running();
        
        ui_paint_advanced_settings_header();    
      }
      
    }
       
  }
  
  // delete the full repaint bit that might be set
  ui_delete_full_repaint_flag();
  
}




// ===================================================================================
// This function gathers all strings for the current menu screen
// ===================================================================================
void ui_load_menu_strings() {
  
    // the bijective ui item id
  byte current_menu_code = ui_encode_ui_pos(main_pos);
     
    // reset the menu length
  menu_length = 0;
  
    // flag for hiding a menu line
  boolean hideLine;
  
    // a temp offset to allow different values for different system states
  int entry_shift;
  
  for (int i=0; i<menu_relation_count; i++) {
    
    entry_shift = 0;
    hideLine = false;
    
    // if we found an entry that is on the current screen
    if (menu_content_relations[i][0] == current_menu_code) {
      
      // S0-M21, S10-M21
      if (menu_content_relations[i][1] == 21) {
        // in continuous mode? 
        if (core_is_mode_timelapse() && core_is_style_continuous()) {
          entry_shift += 1; 
        }
      }
     
      // S23-M32
      if ((menu_content_relations[i][0] == 23) &&
          (menu_content_relations[i][1] == 32)) {
        
        if (!ui_is_setup_style_run()) entry_shift += 5;  
      }
      
      
      // M66      device / motor
      if (
          (
            (menu_content_relations[i][0] == 11) ||
            (menu_content_relations[i][0] == 12) ||
            (menu_content_relations[i][0] == 15) ||
            (menu_content_relations[i][0] == 16) 
          ) 
          && 
          (
            (menu_content_relations[i][1] == 66)
          ) 
          &&
          (
            (!com.isMaster()) ||
            ((com.isMaster()) && (com.getSlaveCount() == 0))
          )
         ) { 
      
        hideLine = true;     
      }
        
      
      /*
      // Non-valid menu entities for "Selected Device: All"
      if (
          (
            (menu_content_relations[i][0] == 11) ||  // Timelapse Run
            (menu_content_relations[i][0] == 12)     // Timelapse Feyframes
          ) 
          && 
          (
            (menu_content_relations[i][1] == 23) ||  // Direction
            (menu_content_relations[i][1] == 30) ||  // Distance
            (menu_content_relations[i][1] == 36) ||  // Exposure time
            (menu_content_relations[i][1] == 56)     // Motor ramp percentage
          ) 
          &&
          (
            (com.isMaster())          && 
            (com.getSlaveCount() > 0) &&
            (com_slave_pos == 255)        // selected device: All
          )
         ) { 
      
        hideLine = true;     
      }
      */
      
            
      
      // S12-M106
      if ((menu_content_relations[i][0] == 20) &&
          (menu_content_relations[i][1] == 106)) {
        
        if (!com.isMaster()) hideLine = true;
      }
           
      
      
      // S20-M106
      if ((menu_content_relations[i][0] == 20) &&
          (menu_content_relations[i][1] == 106)) {
        
        if (!com.isMaster()) hideLine = true;
      }
      
      
      // add the new line (possibly remapped) to the menu array
      if (!hideLine) {
        
        // store the corresponding string to the lines array
        ui_get_string(menu_content_relations[i][1] + entry_shift, menu_length);
        
        // store the line_code
        line_codes[menu_length] = menu_content_relations[i][1] + entry_shift;
         
        // increment the menu length
        menu_length++;
      }
    }  
    
  } 
  
}



// ======================================================================================
// generates the bijective ui item id
// ======================================================================================
byte ui_encode_ui_pos(byte current_pos) {
     
  switch (current_pos) {
    case 0:   
              if (core_is_mode_video()) {  // VIDEO MODE
                                
                // cycle-, run- or keyframe-based menu?
                //if      (ui_is_setup_style_cycle())     return 15; // status - cycle based (advanced)
                if (ui_is_setup_style_run())       return 15; // status - run based (advanced)
                else if (ui_is_setup_style_keyframes()) return 16; // status - keyframe based (advanced)
                
              } else {                     // TIMELAPSE MODE (SMS, STOP-MOTION, CONTINUOUS)
          
                // cycle-, run- or keyframe-based menu?
                //if      (ui_is_setup_style_cycle())     return 10; // status - cycle based (advanced)
                if (ui_is_setup_style_run())       return 11; // status - run based (advanced)
                else if (ui_is_setup_style_keyframes()) return 12; // status - keyframe based (advanced)
              
              }       
             
              break;  
    
    case 1:   return 20; // settings GEN 
    case 2:   return 21; // settings CON 
    case 3:   return 22; // settings MOT 
    case 4:   return 23; // settings EXP 
  }
      
}




// ======================================================================================
// Checks if any status has changed
// ======================================================================================
void ui_check_value_stati() {
  
  /*
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // ---------------------------
  // program state
  if ((action_status_old & B00001000) != (core_is_program_running())) {
    action_status_old = action_status;
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
  
    // set the repaint flag and the - only status update paint flag
    ui_set_repaint_flag();
    //ui_set_repaint_header_flag();
  }
  */
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // ---------------------------
  // shoot count
  if (camera_shoot_count_old != camera_shoot_count) {
    camera_shoot_count_old = camera_shoot_count; 
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();
    
    ui_set_repaint_flag();
    ui_set_paint_shoot_count_flag();
    //ui_paint_shoot_count(false);
       
  }
  

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // ---------------------------
  // battery level
  byte val = power_get_battery_level();
  if ((battery_level_old != val) ||
      (power_status_old != power_status))  {
    
    // if the programm is running and we are in contiuous mode
    motor_continuous_check();    
        
    battery_level_old = val;
    power_status_old = power_status;

    // set the repaint flag and the - only status update paint flag
    ui_set_repaint_flag();
    //ui_set_repaint_header_flag();
  }
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
}


// ======================================================================================
// this function resets the backlight ( this function is supposed to be called from
// outside the UI to switch on or reset backlight on some events)
// ======================================================================================
void ui_trigger_backlight() {

  // turn backlight on
  ui_set_backlight_level();

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

  // reset the backlight counter
  interaction_time = millis();  
  key_time = interaction_time;
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
  
  // set the backlight bit 
  ui_set_backlight_flag();

  // if the programm is running and we are in contiuous mode
  motor_continuous_check();

}

// ======================================================================================
// this function turns the backlight off
// ======================================================================================
void ui_disable_backlight() {
  
  // turn off the backlight   
  digitalWrite(DEF_UI_LCD_BACKLIGHT_PIN, LOW);
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
      
  // set the backlight bit to false B10000000
  ui_delete_backlight_flag();
  
}



// ======================================================================================
void ui_set_backlight_level() {
  
  // set the backlight
  if (backlight_level == 100) {
    digitalWrite(DEF_UI_LCD_BACKLIGHT_PIN, HIGH); 
  } 
  else {
    analogWrite(DEF_UI_LCD_BACKLIGHT_PIN, (backlight_level * 255 / 100));
  }
  
  
}



// ======================================================================================  
void ui_check_autosave() {

  // were the settings changed?
  if (ui_is_settings_changed_flag()) {
    
    // only do setting-saving if we are not in 
    // daisy-chaining while editing the settings of
    // a client and settings should be automatically be saved
    if ((com_slave_pos == 0) &&
        (ui_is_settings_autosave())) {
          
      // set the flag that the settings need to be saved.
      // this is not done here due to performance reasons...
      ui_set_save_settings_flag();
    }

    // delete the settings changed flag
    ui_delete_settings_changed_flag();

  }
}



// ===================================================================================
// generates a message:
// titleLine:  String-code for the title line
// startLine:  String-code for the 1st content line
// endLine:    String-code for the last content line
// duration:   duration of the message in seconds
// ===================================================================================
void ui_generate_message(byte titleLine, byte startLine, byte endLine, byte duration) {
  
  menu_length = 0;
  menu_pos = 0;
  menu_cursor_pos = 0;  
  
  line_codes[0] = titleLine; // title

  // fill the array  
  for (int i=startLine; i<=endLine; i++) {
    line_codes[1 + menu_length++] = i; // content ...
  }
  
  // set the message-flag and the message-to-be-painted-flag
  ui_set_message_on_screen_flag();
  ui_set_message_in_queue_flag();
    
  // set the message duration 
  message_duration = duration;
  
  // trigger the baklight
  ui_trigger_backlight();
  
  // remember the time we started;
  message_start_time = millis();
    
}



// ===================================================================================
// Function for ending a displayed message
// ===================================================================================
void ui_end_message() {
          
  // delete the message flag 
  ui_delete_message_on_screen_flag();
    
  // set the repaint flag 
  ui_set_repaint_flag(); 
  ui_set_full_repaint_flag(); 

  menu_pos = 0;
  menu_cursor_pos = 0;
  //menu_editing = false;
  
}


// ===================================================================================
// Function for getting a string from the flash and storing it in our line array
// ===================================================================================
void ui_get_string(byte buf_number, byte line_number) {

  // Using the string table in program memory requires the use of special functions to 
  // retrieve the data. The strcpy_P function copies a string from program space to a 
  // string in RAM. 

  // Necessary casts and dereferencing 
  strcpy_P(lines[line_number], (char*) pgm_read_word(&(string_table[buf_number]))); 

}

// ===================================================================================
boolean ui_go_to_main_screen() {
  
  if ((main_pos != 0) ||  
      (menu_pos != 0) ||
      (menu_cursor_pos != 0) 
      ){
   
    main_pos_old = main_pos;    
    main_pos = 0;  
    menu_pos = 0;
    menu_cursor_pos = 0;   
  
    return true;  
  }

  return false;
  
}




