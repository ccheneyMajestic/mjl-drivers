# Majestic Labs Drivers
# C. Cheney 2023.06.08
# Build using WSL on Windows
# Updated by C. Cheney 2023.12.30


# Compiler and linker settings
TARGETS := cortex-m0 cortex-m0plus cortex-m4
# Platform specific flags
FLAGS_cortex-m4 := -mfloat-abi=hard -mfpu=fpv4-sp-d16
# Common flags
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
CFLAGS  = -mcpu=$(TARGET) -mthumb -Wall -O2 -ffunction-sections -ffat-lto-objects
ASFLAGS = -mcpu=$(TARGET) -mthumb -Wa,-alh=./build/$@/
LDFLAGS = -mcpu=$(TARGET) -mthumb --specs=nosys.specs


# Directories
INCLUDE_DIRS = ./include
SOURCE_DIRS = ./src
BUILD_DIR = ./build
OBJ_DIR = obj

# Sources and objects
LIB_SOURCES = $(wildcard $(SOURCE_DIRS)/*.c)
VERSION_FILE := ./version.txt
VERSION := $(shell cat $(VERSION_FILE))
NEW_VERSION := $(shell echo $$(($(VERSION)+1)))

# Library
LIBRARY = ./build/$(TARGET)/libmjl_drivers_v$(NEW_VERSION)_$(TARGET).a

# Build library for all targets
all: update_version $(TARGETS)

# For each target, create the object directory
$(TARGETS): %: $(BUILD_DIR)/%/$(OBJ_DIR)
	@echo "Building target $@"
	$(MAKE) TARGET=$@ build_library

# Make the OBJ directory if it doesn't already exist
$(BUILD_DIR)/%/$(OBJ_DIR):
	@echo "Creating folder $@"
	mkdir -p $@
	
# Build the library for the given target 
build_library: $(LIBRARY)

# All of the ojbects for which a .o file should be made 
OBJS = $(LIB_SOURCES:$(SOURCE_DIRS)/%.c=$(BUILD_DIR)/$(TARGET)/$(OBJ_DIR)/%.o)

# Package the objects into a library 
$(LIBRARY): $(OBJS)
	$(AR) rcs $@ $^
	@echo "Created Libary $@"


# Create an object for each .o for each target 
$(BUILD_DIR)/$(TARGET)/$(OBJ_DIR)/%.o: $(SOURCE_DIRS)/%.c
	$(CC) $(CFLAGS) $(FLAGS_$(TARGET)) -c -I$(INCLUDE_DIRS) -o $@ $<

# Delete the full build directory
clean:
	rm -rf $(BUILD_DIR)

# Read the file version and increment to a new value
update_version:
	@echo "Building MJL Driver library v$(NEW_VERSION)..."
	@echo $(NEW_VERSION) > $(VERSION_FILE)

# Treat all as always stale
.PHONY: all