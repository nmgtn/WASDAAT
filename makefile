# portsf library should be installed (one-time) before running make. Use install-portsf.sh
# Installing portsf first on each system removes the requirement to detect OS and choose an archive file to use in make.

# In all cases:
all: WASDAAT.c
	mkdir -p bin
	gcc -Wall -std=gnu99 WASDAAT.c -o bin/WASDAAT -I./include -L./lib -lportsf -lm
