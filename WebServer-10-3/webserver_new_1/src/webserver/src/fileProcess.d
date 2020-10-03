src/webserver/src/fileProcess.o: src/webserver/src/fileProcess.c \
 src/webserver/inc/function.h src/webserver/inc/define.h
	cc -g -O2 -Wall -Werror -fno-strict-aliasing -DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE -I/usr/include/mysql -I. -I./src/webserver/inc -c src/webserver/src/fileProcess.c -o src/webserver/src/fileProcess.o
