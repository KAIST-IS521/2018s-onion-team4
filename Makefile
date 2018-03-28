CC=g++
CCFLAGS=-std=c++11 -Wall
LIBPATH=-lncurses -lcurl -pthread
LIBPATH+=-L/usr/lib/x86_64-linux-gnu -lgpgme -lassuan -lgpg-error
SRC_DIR=src
BUILD_DIR=.
TARGET=OnionMessenger

SRC=$(wildcard $(SRC_DIR)/*.cc)
SRC+=$(wildcard $(SRC_DIR)/SelectServer/src/*.c)
SRC+=$(wildcard $(SRC_DIR)/UI/*.cc)
SRC+=$(wildcard $(SRC_DIR)/Packet/*.cc)
SRC+=$(wildcard $(SRC_DIR)/Utils/*.cc)

all: clean module $(BUILD_DIR)/$(TARGET)
module:
	git submodule init
	git submodule update

$(BUILD_DIR)/$(TARGET):
	$(CC) $(CCFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBPATH)

clean:
	rm $(TARGET) || true
