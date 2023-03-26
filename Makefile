

CC=gcc
C_FLAGS=-Wall -o gpp.bin


build:
	$(CC) $(C_FLAGS) src/gpp/*.c
