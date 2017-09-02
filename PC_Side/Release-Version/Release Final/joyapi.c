/*
 * jstest.c  Version 1.2
 *
 * Copyright (c) 1996-1999 Vojtech Pavlik
 *
 * Sponsored by SuSE
 */

/*
 * This program can be used to test all the features of the Linux
 * joystick API, including non-blocking and select() access, as
 * well as version 0.x compatibility mode. It is also intended to
 * serve as an example implementation for those who wish to learn
 * how to write their own joystick using applications.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

//  NO olvidar poner el JoyStick en ANALOG !!!!!!!!!!!!!!!!!!!!!!!

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <linux/input.h>
#include <linux/joystick.h>

#include "axbtnmap.h"
#include <math.h>

#include "joyapi.h"


static int fd;
//static enum boolean is_event;
static js_event_t js;



int mainTB (int argc, char **argv)
{
	joy_init();
	
	while (1) {
		
		
		if (joy_read(&js)==True){
			
			printf("<<<<<<<< EVENT <<<<<<<<<<<<<\n");
			printf("Event: type %d, time %d, number %d, value %d\n",js.type, js.time, js.number, js.value); 
		}	
		
		usleep(50000);
		//usleep(1000000);
	}
	
}



int joy_init(void)
{

	//Open joy

	if ((fd = open("/dev/input/js0", O_RDONLY)) < 0) {
		perror("jstest");
		return -1;
	}

	fcntl(fd, F_SETFL, O_NONBLOCK); // Reading in nonblocking mode.
	
	printf("Init Joy ... Done!!!\n");

	return 0;
}



/*
 * Reading in nonblocking mode.
 */

enum boolean joy_read(js_event_t *js)
{
	enum boolean is_event;
	
			is_event=False;

			// Read joy
			if(read(fd, js, sizeof(struct js_event)) == sizeof(struct js_event))  {
			
					is_event=True;
			}

			if (errno != EAGAIN) {  // Check if read was succesfull
			
				is_event=False;
			}
						
			return(is_event);

}





