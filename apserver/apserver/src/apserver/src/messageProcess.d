src/apserver/src/messageProcess.o: src/apserver/src/messageProcess.c \
 src/apserver/inc/define.h
	cc -g -O2 -Wall -Werror -fno-strict-aliasing -DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE -I. -I./src/apserver/inc -c src/apserver/src/messageProcess.c -o src/apserver/src/messageProcess.o
