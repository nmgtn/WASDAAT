# Detect OS (only works for Mac and Windows (Cygwin 6.3)
# This hopefully should make life easier for me switching between Andy's Windows install and Will's Mac
UNAME := $(shell uname)

# Decide which library archive name to use
ifeq ($(UNAME), CYGWIN_NT-6.3)
PORTSFLIB = -lportsfWin
else
PORTSFLIB = -lportsfMac
endif

# In all cases:
all: WASDAAT.c
	gcc -Wall -std=gnu99 -o WASDAAT WASDAAT.c -I./include -L./lib $(PORTSFLIB)
