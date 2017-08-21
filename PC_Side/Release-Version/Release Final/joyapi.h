#ifndef JOYAPI_H

#define JOYAPI_H


enum boolean {True,False};
typedef struct js_event js_event_t;

//See Joystick API Documentation https://www.kernel.org/doc/Documentation/input/joystick-api.txt

/*
    {
		__u32 time;      event timestamp in milliseconds 
		__s16 value;     value 
		__u8 type;       event type 
		__u8 number;     axis/button number 
	};
 
*/
  

int joy_init(void);
enum boolean joy_read(js_event_t *);



#endif
