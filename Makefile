TCC:=/mnt/d/program/qnx700/host/linux/x86_64/usr/bin/ntox86_64-gcc
HCC:=gcc
OUT:=out
RM:=rm -rf
ECHO=echo
LIB_SOCKET:=-lsocket
LIB+=$(LIB_SOCKET)

#D_DEBUG:=DEBUG
D_SER_ADDR:=-DSER_ADDR=\"192.168.179.129\"
DEF+=$(D_DEBUG)
DEF+=$(D_SER_ADDR)

.PHONY: test

all: bj app powertrain
	@echo
	@echo "*** build finish ***"
	@echo

powertrain: out
	$(TCC) $(LIB) $(DEF) -o $(OUT)/$@ $@.c

app: out
	$(TCC) -o $(OUT)/app-read app-read.c
	$(TCC) -o $(OUT)/app-read2 app-read2.c
	$(TCC) -o $(OUT)/app-write app-write.c
	$(TCC) -o $(OUT)/app-devctl app-devctl.c

bj: out
	$(HCC) -o $(OUT)/bj-tcp-client bj-tcp-client.c
	$(HCC) -o $(OUT)/bj-tcp-server bj-tcp-server.c
	$(HCC) -o $(OUT)/listener listener.c
	$(HCC) -o $(OUT)/talker talker.c	

out:
ifeq "$(wildcard $@)" ""
	mkdir $@
	echo "directory not existed"
else
	echo "directory existed"
endif

clean:
	$(RM) $(OUT)