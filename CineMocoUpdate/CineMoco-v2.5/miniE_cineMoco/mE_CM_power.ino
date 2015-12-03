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


boolean external_old = false;  


// ======================================================================================
boolean power_is_external_supply_connected() { 

  if (battery_show_external_supply) {
    return isBit(power_status, B10000000); 
  }
  else 
  {
    return false; 
  }
  
}


// ===================================================================================
// this function reads the power states every 3 seconds
// ===================================================================================
void power_check() {
  
  unsigned long now = millis();
  
  // 
  if (power_measure_time + (unsigned long) BATTERY_DELAY <= now) {  
    
    // set the time we did the measurement the last time
    power_measure_time = now;  
    
    // check if there is an external supply connected
    power_read_external_supply();
    
    // read the charging level of the battery
    power_read_battery_state();
    
  }
  
}



// ===================================================================================
void power_read_external_supply() {
  
  int val = analogRead(DEF_POWER_EXT_SUPPLY_PIN);
  
  if (val > 50) {
    
    setBit(power_status, B10000000);    
    
  } else {
   
    deleteBit(power_status, B10000000);    
    
  }
  
}


// ===================================================================================
void power_read_battery_state() {
  
  int val = analogRead(DEF_POWER_BATTERY_PIN);
  
  // if we are running on battery
  if (!power_is_external_supply_connected()) {
  
    boolean do_calibration = false;
    
    // do we have a new min value?
    if ((val < battery_val_empty) &&
        (val >= 300)) {
      
      battery_val_empty = val; 
      do_calibration = true;  
    }
    
    // do we have a new max value?
    if (val > battery_val_full) {
      
      battery_val_full = val;
      do_calibration = true;
    }
    
    if (do_calibration) {
          
      // save the new maximum values    
      eeprom_write_battery_calibration();      
    }
  }
  
  // map the measured value to our 8 displayable battery levels
  val = map(val, 
            battery_val_empty, battery_val_full,
            0, 8); 
    
  // constraint to these 8 levels  
  val = constrain(val, 0, 7); 

 
  // store the val in the battery average buffer array
  battery_avg_pos++;
  if (battery_avg_pos == BATTERY_AVG_BUF_LEN) battery_avg_pos = 0;
  battery_avg_buffer[battery_avg_pos] = val;  
 
   
  // if external supply is connected
  if (power_is_external_supply_connected()) {
    
    power_status = (byte) val;
    
    // re-set the ext. power flag
    setBit(power_status, B10000000); 
    
  } else {
   
    power_status = (byte) val;   
    
  }
  
}

// ===================================================================================
// this function returns the average battery level (value 0-7)
// ===================================================================================
byte power_get_battery_level() {
  
  float res = 0;
  
  for (int i=0; i<BATTERY_AVG_BUF_LEN; i++) {
    res += battery_avg_buffer[i];  
  }
  
  return round(res / (float)BATTERY_AVG_BUF_LEN);
  //return (byte)((float) res / (float) BATTERY_AVG_BUF_LEN);
  
}
  
  

// ===================================================================================
// initializes the average buffer array with the first measured value to have a fast
// battery status display
// ===================================================================================
void power_init_battery_measurement() {
  
  // read the battery
  power_read_battery_state();
  
  // get the actual value out of the power_status 
  byte curVal = power_status;
  if (power_is_external_supply_connected()) curVal -= 128;
    
  // set the whole array with this initial value
  for (int i=0; i<BATTERY_AVG_BUF_LEN; i++) {
    battery_avg_buffer[i] = curVal;  
  }
    
}







