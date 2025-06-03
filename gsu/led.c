#include "su.h"

#define GLITCH_COUNT					2000

volatile ioport int LEDS_REG;

int OnBattery = 0;
static unsigned int Current_Leds_Val;

void leds_write(int leds_value)
{
	LEDS_REG = (unsigned)leds_value;
}

void leds_update_color(int led, Color color)
{
	/* LEDS_REG is a WRITE ONLY register must preserve 
	   remaining UART B RI and DCD bits in the register */
	switch(led)
	{
		case 1:
		switch(color)
		{
			case dark:		Current_Leds_Val &= ~(LED_1_RED | LED_1_GREEN);
			break;
			case green:		Current_Leds_Val = Current_Leds_Val & (~LED_1_RED) | LED_1_GREEN;
			break;
			case red:       Current_Leds_Val = Current_Leds_Val & (~LED_1_GREEN) | LED_1_RED;
			break;
			case orange:    Current_Leds_Val |= (LED_1_RED | LED_1_GREEN);
		}
		break;
		case 2:
		switch(color)
		{
			case dark:		Current_Leds_Val &= ~(LED_2_RED | LED_2_GREEN);
			break;
			case green:		Current_Leds_Val = Current_Leds_Val & (~LED_2_RED) | LED_2_GREEN;
			break;
			case red:       Current_Leds_Val = Current_Leds_Val & (~LED_2_GREEN) | LED_2_RED;
			break;
			case orange:    Current_Leds_Val |= (LED_2_RED | LED_2_GREEN);
		}
		break;
		case 3:
		switch(color)
		{
			case dark:		Current_Leds_Val &= ~(LED_3_RED | LED_3_GREEN);
			break;
			case green:		Current_Leds_Val = Current_Leds_Val & (~LED_3_RED) | LED_3_GREEN;
			break;
			case red:       Current_Leds_Val = Current_Leds_Val & (~LED_3_GREEN) | LED_3_RED;
			break;
			case orange:    Current_Leds_Val |= (LED_3_RED | LED_3_GREEN);
		}
	}
    /* Bit 7 is currently unused */
	Current_Leds_Val &=~0xC0;
	LEDS_REG = Current_Leds_Val;
}

typedef struct LED_BLINK_STRUCT_TAG
{
	Color color;
	Color background;
	Color current_color;
} 
LED_BLINK_STRUCT, *PLED_BLINK_STRUCT;

LED_BLINK_STRUCT led_blink_struct[3];

timer_service_func switch_led_color(int led)
{
	PLED_BLINK_STRUCT plbs = &(led_blink_struct[led-1]);
	
	if((plbs->current_color) != (plbs->color))
	{
		plbs->current_color = plbs->color;
	}
	else
	{
		plbs->current_color = plbs->background;		
	}
	
	leds_update_color(led, plbs->current_color);
	
	return (timer_service_func)1;		
}

void leds_update_background(int led, Color color, Color background, short period)
{
	static int led_timer_handler[3]={-1,-1,-1};
	
	if(led < 1 || led > 3)
		return;
	
	if(period<=0)
	{
		if(led_timer_handler[led-1] != -1)
			timer_disable(led_timer_handler[led-1]);
	}
    else
    {
    	if(led_timer_handler[led-1] == -1)
    		if(!timer_add(&(led_timer_handler[led-1]), 0,// Not one_shot!!
	   			(timer_service_func)switch_led_color,(void *)led,0))
			{
				DebugPrintf("Unable to create timer for LED\r\n");
				led_timer_handler[led-1] = -1;
			}
		
		if(led_timer_handler[led-1] != -1)
			if(!timer_enable(led_timer_handler[led-1],(unsigned int)period))
				DebugPrintf("Unable to enable timer for LED\r\n");
	}
	
	led_blink_struct[led-1].color = color;
	led_blink_struct[led-1].background = background;
	led_blink_struct[led-1].current_color = color;	
	
	leds_update_color(led,color);
}

void leds_blink(int led, short period)
{
	Color color, background;
	
	color =	led_blink_struct[led-1].color;
	background = led_blink_struct[led-1].background;
		
	leds_update_background(led,color,background,period); 
}

void Set_Battery_Status(void)
{
  static BYTE OldBatteryStatus = 0;
  static BYTE GlitchStatus = 0;
  static DWORD glitch_cnt = GLITCH_COUNT;
  BYTE BatteryStatus;
  char strB[32];

  BatteryStatus = (0x00F0 & LEDS_REG) >> 4;
  if(OldBatteryStatus != BatteryStatus)
  {
    if(glitch_cnt == GLITCH_COUNT)
    {
      GlitchStatus = BatteryStatus;
      glitch_cnt--;
    }
    else if(GlitchStatus == BatteryStatus)
    {
      glitch_cnt--;
    }
    else if((GlitchStatus != BatteryStatus) && (glitch_cnt != 0))
    {
       glitch_cnt = GLITCH_COUNT;
    }
    if((GlitchStatus == BatteryStatus) && (glitch_cnt == 0))
    {
      glitch_cnt = GLITCH_COUNT;
      OldBatteryStatus = BatteryStatus;
    
//      sprintf(strB,"current battery status: 0x%X \r\n",BatteryStatus);
//      DebugPrintf(strB);
    
      switch(BatteryStatus)
      {
        case 0:			// low power
        case 2:			// low power
        case 4:			// low power
        case 6:			// low power
          OnBattery = 0;
          leds_update(1,red,100);
          break;
        
        case 5:			// no battery
          OnBattery = 0;
          leds_update(1,green,100);
          break;
      
        case 3:			// power ok
        case 7:			// power ok
          OnBattery = 0;
          leds_update(1,green,0);
          break;
      
        case 8:			// low battery
          OnBattery = 1;
          leds_update(1,orange,100);
          break;
         case 12:			// on battery
          OnBattery = 1;
          leds_update(1,orange,0);
          break;
      
        default:
          leds_update(1,red,0);
          sprintf(strB,"Invalid battery status: %d \r\n",BatteryStatus);
          DebugPrintf(strB);
          break;
      } // end switch
    }  // end else if((GlitchStatus ...
  } // end if(OldBatteryStatus ...
}


BOOLEAN GetBoardVersion(void)
{
  BYTE ReadRegVal, WriteRegVal, cmpVal;
  BOOLEAN bWsu003;
  
  ReadRegVal = LEDS_REG & 0x00C0;
  WriteRegVal = (~ReadRegVal) & 0x00C0;
  
  LEDS_REG = WriteRegVal & 0x00C0;
  cmpVal = LEDS_REG & 0x00C0;
  
  if((cmpVal == WriteRegVal) && (cmpVal != ReadRegVal))
  
    bWsu003 = TRUE;
  else
    bWsu003 = FALSE;
  
  return bWsu003;
}
