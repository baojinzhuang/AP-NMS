src/apclient/src/messageProcess.o: src/apclient/src/messageProcess.c \
 src/apclient/inc/define.h
	arm-linux-gcc -g -O2 -Wall -Werror -fno-strict-aliasing -DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE -I. -I./src/apclient/inc -c src/apclient/src/messageProcess.c -o src/apclient/src/messageProcess.o
