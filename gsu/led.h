#ifndef __LED_H__
#define __LED_H__

#define LED_1_RED	0x01
#define LED_1_GREEN	0x02
#define LED_2_RED	0x04
#define LED_2_GREEN	0x08
#define LED_3_RED	0x10
#define LED_3_GREEN	0x20

#define LEDS_REG port3000

typedef enum Color_tag
{
	dark,green,red,orange
} Color;

void leds_write(int leds_value);
#define leds_update(led,color,period) leds_update_background(led,color,dark,period)
void leds_update_background(int led, Color color, Color background, short period);
void leds_blink(int led, short period);
void Set_Battery_Status(void);
BOOLEAN GetBoardVersion(void);
#endif

