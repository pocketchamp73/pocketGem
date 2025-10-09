# pocketGem Makefile for Next Thing CHIP (GTK/XFCE)

CC = gcc
CFLAGS = -Wall -Wextra -O2 `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

TARGET = pocketgem
SRC_DIR = src
BUILD_DIR = build
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install run help

help:
	@echo "pocketGem - Build Commands"
	@echo ""
	@echo "make          - Build the application"
	@echo "make run      - Build and run the application"
	@echo "make clean    - Remove build artifacts"
	@echo "make install  - Install to /usr/local/bin"

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: all
	install -m 755 $(TARGET) /usr/local/bin/
