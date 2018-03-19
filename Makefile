CC=g++
CCFLAGS=-std=c++11 -Wall
SRC_DIR=src
BUILD_DIR=.
TARGET=OnionMessenger

SRC=$(wildcard $(SRC_DIR)/*.cc)
SRC+=$(wildcard $(SRC_DIR)/SelectServer/src/*.cc)

all: clean $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET):
	$(CC) $(CCFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET)

clean:
	rm $(TARGET) || true
