
#author : Baojinzhuang
#date : 2020.01.09

EXENAME = webserver

SRCS_PATH = ./src

CFLAGS = \
	-g -O2 -Wall -Werror -fno-strict-aliasing \
	-DHSDPA_VER -DLINUX -D_DEBUG -DEANBLE_LOG_DECODED_MESSAGE \
	 -I/usr/include/mysql

SRCS = \
	$(wildcard $(SRCS_PATH)/webserver/src/*.c)

INCS = \
	-I. \
	-I$(SRCS_PATH)/webserver/inc

LIBS = \
	-lpthread \
	-L/usr/lib64/mysql -lmysqlclient 

LDFLAGS= \
	-L/usr/lib64/mysql

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
	mkdir -p ../webserver-bin/
	rm -rf ../webserver-bin/*
	cp $(EXENAME) ../webserver-bin/
	cp ./sh/*.sh ../webserver-bin/

clean:
	rm -rf $(OBJS) $(DEPS) $(EXENAME) core



