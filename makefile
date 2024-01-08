# Majestic Labs Drivers
# C. Cheney 2023.06.08
# Build using WSL on Windows
# Updated by C. Cheney 2023.12.30


# Compiler and linker settings
TARGETS := cortex-m0 cortex-m0plus cortex-m4
# TARGETS := cortex-m0
# # Platform specific flags
FLAGS_cortex-m4 := -mfloat-abi=hard -mfpu=fpv4-sp-d16
# Common flags
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
CFLAGS  = -mcpu=$(TARGET) -mthumb -Wall -O2 -ffunction-sections -ffat-lto-objects
ASFLAGS = -mcpu=$(TARGET) -mthumb -Wa,-alh=$(BUILD_DIR)/$@/
LDFLAGS = -mcpu=$(TARGET) -mthumb --specs=nosys.specs


# Constants
INCLUDE_DIRS = ./include
SOURCE_DIRS = ./src
BUILD_DIR = ./build
OBJ_DIR = obj
INC_DIR = include
LIB_NAME = libmjl_drivers

# Sources and objects - These are evaluated for individual targets 
LIB_SOURCES = $(wildcard $(SOURCE_DIRS)/*.c)
OBJS = $(LIB_SOURCES:$(SOURCE_DIRS)/%.c=$(BUILD_DIR)/$(TARGET)/$(OBJ_DIR)/%.o)
FULL_NAME = $(LIB_NAME)_v$(VERSION)_$(TARGET)
LIBRARY = $(BUILD_DIR)/$(TARGET)/$(FULL_NAME).a

# Treat the following targets as always stale
.PHONY: all 

# Build library for all targets
all: update_version $(TARGETS)

# Run the update_version script — Increment the build number
update_version:
	$(eval LIB_VERSION := $(shell ./update_version.sh))
	@echo "Building MJL Driver Library v$(LIB_VERSION)"

# Build for each target
$(TARGETS):
	@echo "Building $(LIB_VERSION) for $@"
	$(MAKE) TARGET=$@ VERSION=$(LIB_VERSION) build_target_library

# #######################  For each target ##################################

# Called for each target with TARGET passed in
build_target_library: setup_lib $(LIBRARY)

# Dynamically evaluate the variables for the library
setup_lib:
	@echo " ---------------------------- Building for $(TARGET) ---------------------------- "
	mkdir -p $(BUILD_DIR)/$(TARGET)/$(OBJ_DIR)

# Package all .o files into a .a file for a specified target 
$(LIBRARY): $(OBJS)
	$(AR) rcs $@ $^ 
	$(eval ZIP :=  $(BUILD_DIR)/$(TARGET)/$(FULL_NAME).zip)
	zip $(ZIP) -j $@ > /dev/null
	zip -r $(ZIP) $(INCLUDE_DIRS) > /dev/null
	rm $@
	@echo " * Created Libary $(ZIP)"

# Compile each .c into a .o
$(BUILD_DIR)/$(TARGET)/$(OBJ_DIR)/%.o: $(SOURCE_DIRS)/%.c
	$(CC) $(CFLAGS) $(FLAGS_$(TARGET)) -c -I$(INCLUDE_DIRS) -o $@ $<

# Delete the full build directory
clean:
	rm -rf $(BUILD_DIR)
