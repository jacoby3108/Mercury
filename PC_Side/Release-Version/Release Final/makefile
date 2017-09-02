udp:udp.c axbtnmap.o joyapi.o
	gcc udp.c joyapi.o axbtnmap.o -lpthread -o udp

#  la opción -c no genera el ejecutable, sino el código objeto
#  (compilo No link-edito )
		
axbtnmap.o:axbtnmap.c axbtnmap.h
	gcc -c axbtnmap.c

joyapi.o:joyapi.c joyapi.h
	gcc -c joyapi.c

clean:
	rm *.o
