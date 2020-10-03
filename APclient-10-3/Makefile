
#author : Baojinzhuang
#date : 2019.12.10
CC=arm-linux-gcc

EXENAME = apclient

SRCS_PATH = ./src

CFLAGS = \
	-g -O2 -Wall -Werror -fno-strict-aliasing \
	-DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE

SRCS = \
	$(wildcard $(SRCS_PATH)/apclient/src/*.c)

INCS = \
	-I. \
	-I$(SRCS_PATH)/apclient/inc

LIBS = \
	-lpthread


OBJS = $(SRCS:%.c=%.o)

DEPS = $(SRCS:%.c=%.d)
 
all : $(EXENAME)

$(EXENAME) : $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(EXENAME)

ifneq ($(MAKECMDGOALS),clean)
sinclude $(DEPS)
endif

depend : $(DEPS)

%.d : %.c
	-rm -rf $*.d;\
	$(CC) -MM $(CFLAGS) $(INCS) $<|sed 's,^.*:,$*.o:,' > log.$$$$1;\
	echo $(CC) $(CFLAGS) $(INCS) -c $< -o $*.o > log.$$$$2;\
	cat log.$$$$1 log.$$$$2|sed 's/^$(CC)/\t$(CC)/g' > $@;\
	rm -rf log.$$$$*;\

install:
	mkdir -p ../apclient-bin/
	rm -rf ../apclient-bin/*
	cp $(EXENAME) ../apclient-bin/
	cp ./sh/*.sh ../apclient-bin/

clean:
	rm -rf $(OBJS) $(DEPS) $(EXENAME) core



