

CC=gcc
C_FLAGS=-Wall -o gpp.bin -lm -foptimize-strlen
SOURCE_FILES := $(shell find -type f -name "*.c" -print | tr "\n" " ")


build:
	$(CC) $(C_FLAGS) $(SOURCE_FILES)
