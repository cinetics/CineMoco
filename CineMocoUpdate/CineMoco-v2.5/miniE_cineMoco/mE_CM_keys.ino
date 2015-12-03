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


  // global variables to check if a specific key was pressed
// B7 = SELECT
// B6 = MENU
// B5 = UP
// B4 = DOWN
// B3 = LEFT
// B2 = RIGHT
// B1
// B0
volatile byte vol_key_pressed = 0;

  // values for long key presses
#define  key_initial_delay   500    // delay between keys on longnently pressed key
#define  key_rate            150    // the base key rate (delay) we start with when long pressing a key






byte key = KEY_NONE;
byte oldkey = KEY_NONE;

  // the dynamically calculated key rate (speed up during the long press)
unsigned long key_rate_dynamic = key_rate;
  // the key amount we send out allready during the long press
unsigned long key_count;
  // the time we started with a key press
unsigned long key_press_start;

// ======================================================================================
boolean keys_key_pressed()       { return isBit(vol_key_pressed, B11111100); } 
void keys_delete_key_buffer()    { vol_key_pressed = B0; }
boolean keys_is_select_pressed() { return isBit(vol_key_pressed, BIT_7); }


// ======================================================================================
byte keys_get_key_pressed() {
  
  // delete the global key var
  key = KEY_NONE;
  
  // copy the volantile buffer for less access to it
  key_pressed = vol_key_pressed;
  
  // delete the key-pressed-buffer if key was pressed
  if (keys_key_pressed()) vol_key_pressed = B0;
  
  // did we have a key event in the mean time?
  // SELECT
  if (key_pressed & B10000000) {
    
    bitClear(key_pressed, 7); // B01111111
    key = KEY_SELECT;
    return KEY_SELECT;
  
  } 
  
  // if the programm is running and we are in contiuous mode
  motor_continuous_check();
    
  //  if program is running
  if (action_status & B00001000) { 
      
      // if any key was pressed, trigger the backlight and return KEY_NONE
      if (key_pressed != B0) {
        
        // delete the pressed-key-buffer
        key_pressed = B0;
        
        // enable backlight
        ui_trigger_backlight();
        
        // set the value to the global key-var
        return KEY_NONE;
        
      }
  } 
  
  // program is stopped - perform the full check
  else {
  
    // MENU
    if (key_pressed & B01000000) {
      
      bitClear(key_pressed, 6); // B10111111
      return KEY_MENU;
      
    } 
    // UP
    else if ((keys_read_key_up()) ||
             (key_pressed & B00100000)) {
      
      bitClear(key_pressed, 5); // B11011111    
      key = KEY_UP;  
    }
    // DOWN
    else if ((keys_read_key_down()) ||
             (key_pressed & B00010000)) {
      
      bitClear(key_pressed, 4); // B11101111    
      key = KEY_DOWN;  
    }
    // LEFT
    else if ((keys_read_key_left()) ||
             (key_pressed & B00001000)) {
      
      bitClear(key_pressed, 3); // B11110111    
      key = KEY_LEFT;  
    }
    // RIGHT
    else if ((keys_read_key_right()) ||
             (key_pressed & B00000100)) {
      
      bitClear(key_pressed, 2); // B11111011    
      key = KEY_RIGHT;  
    }
    // ELSE
    else {
      key = KEY_NONE;  
    }


    
    // no key pressed
    if (key == KEY_NONE) {
      oldkey = KEY_NONE;
      return KEY_NONE;
    } 
      
      
      
    // single press detected
    if (key != oldkey) {                
      
      // delete the long key flag and the counter because we have a single press
      if (ui_is_long_key_press()) {
        
        ui_delete_long_key_press();
        key_count = 0;
        
        // reset the delays for long key presses
        key_rate_dynamic = key_rate;
      }
          
      oldkey = key;  
      
      if (key >=0) {
        return key;
      }
      
    }
    
    
    // long-key-press
    if (key == oldkey) {
      
      // if we have a long key press
      if (ui_is_long_key_press()) {
        
        // initial delay and rate delay over?
        if ( (key_press_start + key_initial_delay + (key_count * key_rate_dynamic)) < millis()) {
          
            key_count++;
                      
            // calculate the new key-rate-delay for speed up
            if (key_count > 2) {
              key_rate_dynamic = round(key_rate - (key_count / 3));
              
              // allow no zero values
              if (key_rate_dynamic < 0) {
               key_rate_dynamic = 0; 
              }
            }
            
            return key;
        }
        
      } else {
        // set the long key press flag and remember the time
        ui_set_long_key_press();
        key_press_start = millis();
            
      }
      
    }
    
  } 
  
  return KEY_NONE;
  
}

// ======================================================================================
// Interrupt function - executed when the interrupt is called
// ======================================================================================
void keys_select_key_event() {

  // set the SELECT bit
  vol_key_pressed |= B10000000;

}


// ======================================================================================
// Interrupt function - executed when the interrupt is called
// ======================================================================================
void keys_menu_key_event() {

  // set the MENU bit
  vol_key_pressed |= B01000000;

}

// ======================================================================================
// Interrupt function - executed when the interrupt is called
// ======================================================================================
void keys_up_key_event() {

  // set the UP bit
  vol_key_pressed |= B00100000;

}

// ======================================================================================
// Interrupt function - executed when the interrupt is called
// ======================================================================================
void keys_down_key_event() {

  // set the DOWN bit
  vol_key_pressed |= B00010000;

}

// ======================================================================================
// Interrupt function - executed when the interrupt is called
// ======================================================================================
void keys_left_key_event() {

  // set the LEFT bit
  vol_key_pressed |= B00001000;

}

// ======================================================================================
// Interrupt function - executed when the interrupt is called
// ======================================================================================
void keys_right_key_event() {

  // set the RIGTH bit
  vol_key_pressed |= B00000100;

}


// ======================================================================================
boolean keys_read_key_up() {
  
  if (digitalRead(DEF_KEY_UP_PIN) == HIGH) return true;
    
  return false;
    
}

// ======================================================================================
boolean keys_read_key_down() {
    
  if (digitalRead(DEF_KEY_DOWN_PIN) == HIGH) return true;
  
  return false;

}


// ======================================================================================
boolean keys_read_key_left() {
   
  if (digitalRead(DEF_KEY_LEFT_PIN) == HIGH) return true;
  
  return false;
  
}

// ======================================================================================
boolean keys_read_key_right() {
    
  if (digitalRead(DEF_KEY_RIGHT_PIN) == HIGH) return true;
  
  return false;
  
}






