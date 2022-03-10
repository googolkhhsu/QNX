TCC:=/mnt/d/program/qnx700/host/linux/x86_64/usr/bin/ntox86_64-gcc
HCC:=gcc
OUT:=out
RM:=rm -rf
ECHO=echo
SOCKLIB:=-lsocket
LIB+=$(SOCKLIB)

.PHONY: test

all: powertrain bj
	@echo
	@echo done

powertrain: out
	$(TCC) $(LIB) -o $(OUT)/$@ $@.c

bj:
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