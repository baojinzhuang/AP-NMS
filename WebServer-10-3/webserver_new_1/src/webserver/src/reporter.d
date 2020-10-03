src/webserver/src/reporter.o: src/webserver/src/reporter.c src/webserver/inc/define.h
	cc -g -O2 -Wall -Werror -fno-strict-aliasing -DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE -I/usr/include/mysql -I. -I./src/webserver/inc -c src/webserver/src/reporter.c -o src/webserver/src/reporter.o
