CC=g++
CCFLAGS=-std=c++11 -Wall
LIBPATH=-lncurses -L/usr/lib/x86_64-linux-gnu -lgpgme -lassuan -lgpg-error -lpthread
SRC_DIR=src
BUILD_DIR=.
TARGET=OnionMessenger

SRC=$(wildcard $(SRC_DIR)/*.cc)
SRC+=$(wildcard $(SRC_DIR)/SelectServer/src/*.c)
SRC+=$(wildcard $(SRC_DIR)/ui/*.cc)

all: clean $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET):
	$(CC) $(CCFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBPATH)

clean:
	rm $(TARGET) || true
