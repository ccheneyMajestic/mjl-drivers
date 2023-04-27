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
LIBRARY = ./build/cortex-m0/libmjl_drivers.a

# Targets
all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SOURCE_DIRS)/%.c
	$(CC) $(CFLAGS) -c -I$(INCLUDE_DIRS) -o $@ $<

clean:
	rm -rf $(OBJ_DIR)
