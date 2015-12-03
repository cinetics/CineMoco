/*

    (c) 2015 Alvarocalvo, Airic Lenz
        
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




// ======================================================================================
// Functions to manage EEPROM memory
// ======================================================================================



// ======================================================================================
boolean eeprom_is_version_ok()
{
  int v;
  int sv;
  eeprom_read(1,v);
  delay(10);
  eeprom_read(3,sv);
  delay(10);
  return (v == DEF_VERSION && sv == DEF_SUBVERSION);
  
}

// ======================================================================================
boolean eeprom_is_config_saved()
{
  byte not_saved = EEPROM.read(0);
  return(!not_saved);
}

// ======================================================================================
void eeprom_write_version()
{
   int v  = DEF_VERSION;
   int sv = DEF_SUBVERSION;
   eeprom_write(1,v);
   delay(10);
   eeprom_write(3,sv);
   delay(10);
  
}

// ======================================================================================
void eeprom_saved( boolean saved ) // change the flag eeprom memory status
{ 
  EEPROM.write(0,!saved);
}

// ======================================================================================
void eeprom_write(int addr, byte& value, byte length)
{
  byte* p = (byte*)(void*)&value;
  for(byte i=0;i<length;i++)
    EEPROM.write(addr++,*p++);
  eeprom_saved(true); //change our flag: the memory is saved
}

// ======================================================================================
void eeprom_write(int addr,int& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_write(addr,*p, sizeof(int));
}

// ======================================================================================
void eeprom_write(int addr,long& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_write(addr,*p, sizeof(long));
}

// ======================================================================================    
void eeprom_write(int addr,unsigned int& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_write(addr,*p,sizeof(int));
}

// ======================================================================================
void eeprom_write(int addr,unsigned long& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_write(addr,*p,sizeof(long));
}

// ======================================================================================
void eeprom_write(int addr, float& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_write(addr,*p,sizeof(float));
}

// ======================================================================================
void eeprom_write(int addr,byte& value)
{  
  EEPROM.write(addr,value);
  eeprom_saved(true);
}

// ======================================================================================
void eeprom_write(int addr,short int& value)
{  
  EEPROM.write(addr,value);
  eeprom_saved(true);
}






// ======================================================================================
void eeprom_read(int addr,byte& value,byte length)
{
  byte* p = (byte*)(void*)&value;
  for(byte i=0;i<length;i++)
    *p++=EEPROM.read(addr++);
}

// ======================================================================================
void eeprom_read(int addr,byte& value)
{
  value = EEPROM.read(addr);
}

// ======================================================================================
void eeprom_read(int addr,int& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_read(addr,*p,sizeof(int));
}

// ======================================================================================
void eeprom_read(int addr,long& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_read(addr,*p,sizeof(long));
  
}

// ======================================================================================
void eeprom_read(int addr,unsigned int& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_read(addr,*p,sizeof(int));
}

// ======================================================================================
void eeprom_read(int addr,unsigned long& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_read(addr,*p,sizeof(value));
  
}

// ======================================================================================
void eeprom_read(int addr,float& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_read(addr,*p,sizeof(value)); 
}

// ======================================================================================
void eeprom_read(int addr,short int& value)
{
  byte* p = (byte*)(void*)&value;
  eeprom_read(addr,*p,sizeof(value)); 
}




// ======================================================================================
// This function writes all settings to the eeprom
// ======================================================================================    
void eeprom_write_config() {
  
  byte btmp;           // tmp byte var
  int address;         // our address  
  unsigned long date;  // temp date var 
    
    
  // 6-9: battery calibration
  
  // --------------------------
  // 10-299: bytes for miniE
  
  address =  10;
 
  eeprom_write(address, cycle_length);             address += 4; 
  
  eeprom_write(address, camera_focus_time);        address += 4; 
  eeprom_write(address, camera_exp_time);          address += 4; 
  eeprom_write(address, camera_post);              address += 4; 
  eeprom_write(address, camera_shot_max);          address += 4;  
  
  eeprom_write(address, exposure_status);          address += 1;
  eeprom_write(address, HDR_shot_number);          address += 1;
  eeprom_write(address, HDR_exposure_first);       address += 4;
  eeprom_write(address, HDR_exposure_last);        address += 4; 
  eeprom_write(address, HDR_bracket);              address += 1;    

  //eeprom_write(address, motor_ramp);               address += 4; 
  eeprom_write(address, motor_ramp_percent);       address += 1; 
  eeprom_write(address, motor_ramp_jog);           address += 4; 
  eeprom_write(address, motor_cycle_distance);     address += 4; 
  eeprom_write(address, motor_speed);              address += 4; 
  eeprom_write(address, motor_speed_jog);          address += 1; 
  
  eeprom_write(address, motor_video_speed);        address += 1;
  eeprom_write(address, motor_video_ramp_in);      address += 1;
  eeprom_write(address, motor_video_ramp_out);     address += 1;
  eeprom_write(address, motor_video_dist);         address += 4;

  eeprom_write(address, motor_status);             address += 1;
  eeprom_write(address, motor_post);               address += 4;
  eeprom_write(address, motor_calibration_index);  address += 1;
  eeprom_write(address, motor_step_sizes_index);   address += 1;
  
  //eeprom_write(address, motor_step_delay_max);   address += 4; 
  //eeprom_write(address, motor_step_delay_min);   address += 4; 

  
  eeprom_write(address, run_fps);                  address += 1;
  eeprom_write(address, run_record_time);          address += 4; 
  eeprom_write(address, run_play_time);            address += 4;
  eeprom_write(address, run_dist);                 address += 4; 
  
  eeprom_write(address, system_mode);              address += 1; 
  eeprom_write(address, system_move_style);        address += 1; 
  eeprom_write(address, com_id);                   address += 1; 
  eeprom_write(address, motor_key_status);         address += 1;     
  
  eeprom_write(address, camera_type_index );       address += 1; 
  
  // --------------------------
  // 300-499: bytes for general settings
  
  address =  300;
  
  //write into eeprom UI configurations
  eeprom_write(address, backlight_level);          address += 1;
  eeprom_write(address, backlight_wait);           address += 2;
    
  // eeprom_write(address, battery_show_external_supply); address += 1;    
 
  
  eeprom_write(address, ui_settings);              address += 1;
  eeprom_write(address, ui_setup_style);           address += 1; 
 
  // --------------------------
  // 500-999:  dragonframe data
    
  address =  500;
  
  eeprom_write(address, dragonframe_startUSBMode); address += 1;
    
 
}


// ======================================================================================
// This function reades all settings from the eeprom
// ======================================================================================    
void eeprom_load_config() {
  
  byte btmp;           // tmp byte var
  int address;         // our address  
  unsigned long date;  // temp date var 
    
  // 6-9: battery calibration
    
  // --------------------------
  // 6-299: bytes for miniE
  
  address =  10;
  
  eeprom_read(address, cycle_length);               address += 4;  
  
  eeprom_read(address, camera_focus_time);          address += 4;  
  eeprom_read(address, camera_exp_time);            address += 4;  
  eeprom_read(address, camera_post);                address += 4;  
  eeprom_read(address, camera_shot_max);            address += 4;  
      
  eeprom_read(address, exposure_status);            address += 1;
  eeprom_read(address, HDR_shot_number);            address += 1;
  eeprom_read(address, HDR_exposure_first);         address += 4;
  eeprom_read(address, HDR_exposure_last);          address += 4; 
  eeprom_read(address, HDR_bracket);                address += 1;
  
  //eeprom_read(address, motor_ramp);                 address += 4; 
  eeprom_read(address, motor_ramp_percent);         address += 1; 
  eeprom_read(address, motor_ramp_jog);             address += 4; 
  eeprom_read(address, motor_cycle_distance);       address += 4; 
  eeprom_read(address, motor_speed);                address += 4; 
  eeprom_read(address, motor_speed_jog);            address += 1; 
  
  eeprom_read(address, motor_video_speed);          address += 1;
  eeprom_read(address, motor_video_ramp_in);        address += 1;
  eeprom_read(address, motor_video_ramp_out);       address += 1;
  eeprom_read(address, motor_video_dist);           address += 4;

  eeprom_read(address, motor_status);               address += 1;
  eeprom_read(address, motor_post);                 address += 4;
  eeprom_read(address, motor_calibration_index);    address += 1;
  eeprom_read(address, motor_step_sizes_index);     address += 1;
  //eeprom_read(address, motor_step_delay_max);     address += 4; 
  //eeprom_read(address, motor_step_delay_min);     address += 4; 
  
  
  eeprom_read(address, run_fps);                    address += 1;
  eeprom_read(address, run_record_time);            address += 4; 
  eeprom_read(address, run_play_time);              address += 4;
  eeprom_read(address, run_dist);                   address += 4; 

  eeprom_read(address, system_mode);                address += 1; 
  eeprom_read(address, system_move_style);          address += 1; 
  eeprom_read(address, com_id);                     address += 1; 
  eeprom_read(address, motor_key_status);           address += 1; 
 
  eeprom_read(address, camera_type_index);          address += 1;   
 
 
  // --------------------------
  // 300-499: bytes for general settings
  
  address =  300;

  eeprom_read(address, backlight_level);            address += 1;  
  eeprom_read(address, backlight_wait);             address += 2;
  
  // eeprom_read(address, battery_show_external_supply); address += 1;  

    
  // menu style
  eeprom_read(address, ui_settings);                address += 1; 
  eeprom_read(address, ui_setup_style);             address += 1;   
    
  // --------------------------
  // 500-...  dragonframe stuff
  
  address =  500;
   
  eeprom_read(address, dragonframe_startUSBMode);   address += 1; 
 
 
    
}


// ======================================================================================
// Function to restart the Arduino
// ======================================================================================
void eeprom_write_battery_calibration() {
  
  int address;         // our address  
  
  // --------------------------
  // 6-9: battery calibration
  address =  6;
  
  eeprom_write(address, battery_val_empty);   address += 2; 
  eeprom_write(address, battery_val_full);    address += 2; 
  
}


// ======================================================================================
// Function to restart the Arduino
// ======================================================================================
void eeprom_load_battery_calibration() {
  
  int address;         // our address  
  
  // --------------------------
  // 6-9: battery calibration
  address =  6;
  
  eeprom_read(address, battery_val_empty);    address += 2; 
  eeprom_read(address, battery_val_full);     address += 2;
  
}






