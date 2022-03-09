GCC:=/mnt/d/program/qnx700/host/linux/x86_64/usr/bin/ntox86_64-gcc
#GCC:=gcc
OUT:=out
RM:=rm -rf
ECHO=echo
SOCKLIB:=-lsocket
LIB+=$(SOCKLIB)

out:
ifeq "$(wildcard $@)" ""
	mkdir $@
	echo "directory not existed"
else
	echo "directory existed"
endif

powertrain: out
	$(GCC) $(LIB) -o $(OUT)/$@ $@.c

clean:
	$(RM) $(OUT)