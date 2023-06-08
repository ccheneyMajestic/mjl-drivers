# Compiler and linker settings
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
CFLAGS = -mcpu=cortex-m0 -mthumb -Wall -O2 -ffunction-sections -ffat-lto-objects
ASFLAGS = -mcpu=cortex-m0 -mthumb -Wa,-alh=./build/cortex-m0/
LDFLAGS = -mcpu=cortex-m0 -mthumb --specs=nosys.specs

# Directories
INCLUDE_DIRS = ./include
SOURCE_DIRS = ./src
OBJ_DIR = ./build/cortex-m0/obj

# Sources and objects
LIB_SOURCES = $(wildcard $(SOURCE_DIRS)/*.c)
OBJS = $(LIB_SOURCES:$(SOURCE_DIRS)/%.c=$(OBJ_DIR)/%.o)

# Library
VERSION_FILE := ./build/cortex-m0/version.txt
VERSION := $(shell cat $(VERSION_FILE))
NEW_VERSION := $(shell echo $$(($(VERSION)+1)))
LIBRARY = ./build/cortex-m0/libmjl_drivers_v$(NEW_VERSION).a

# Targets
all: update_version $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SOURCE_DIRS)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -I$(INCLUDE_DIRS) -o $@ $<

# Make the OBJ directory if it doesn't already exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

update_version:
	@echo $(NEW_VERSION) > $(VERSION_FILE)
