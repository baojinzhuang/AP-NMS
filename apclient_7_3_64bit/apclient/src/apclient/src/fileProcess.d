src/apclient/src/fileProcess.o: src/apclient/src/fileProcess.c src/apclient/inc/define.h
	cc -g -O2 -Wall -Werror -fno-strict-aliasing -DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE -I. -I./src/apclient/inc -c src/apclient/src/fileProcess.c -o src/apclient/src/fileProcess.o
