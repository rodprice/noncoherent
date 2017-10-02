# Simply Embedded tutorial makefile

# This however can be worked around with some additional compiler
# flags, to turn on link time optimization and garbage collection,
# which the new compiler does support, but is not turned on by
# default.

# add to compiler flags:

# > -fdata-sections
# > -ffunction-sections
# > -fno-asynchronous-unwind-tables
# > -flto

# and to linker flags
# > --gc-sections
# > --print-gc-sections

# this will cause the compiler to mark each function and data section
# in the resulting object file, which allows the linker to garbage
# collect unused functions (stripped functions are printed thanks to
# print-fc-sections)


# Path to the msp430 toolchain
TOOLCHAIN_ROOT?=/opt/ti/msp430_gcc

# Toolchain variables
CC:=$(TOOLCHAIN_ROOT)/bin/msp430-elf-gcc
TOOL_INC_DIR:=$(TOOLCHAIN_ROOT)/include
TOOL_LIB_DIR:=$(TOOLCHAIN_ROOT)/include # linker scripts live in the include dir

# Directories
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
BIN_DIR=$(BUILD_DIR)/bin
SRC_DIR=src
INC_DIR=include

# Attempt to create the output directories
ifneq ($(BUILD_DIR),)
$(shell [ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR))
$(shell [ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR))
$(shell [ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR))
endif 

# Source files
SRCS:=$(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

# ELF file output
ELF:=$(BIN_DIR)/app.out

# Compile flags
# CFLAGS:= -mmcu=msp430g2553 -mhwmult=none -c -O0 -g3 -ggdb -gdwarf-2 -Wall -Werror -Wextra -Wshadow -std=gnu90 -Wpedantic -MMD -I$(INC_DIR)
# CFLAGS:= -mmcu=msp430g2553 -c -Wall -std=gnu90 -MMD -I$(INC_DIR) -I$(TOOL_INC_DIR)
CFLAGS:= -mmcu=msp430g2231 -mhwmult=none -c -Wall -Os -std=gnu90 -MMD -I$(INC_DIR) -I$(TOOL_INC_DIR)
# Linker flags
# LDFLAGS:= -mmcu=msp430g2231 --gc-sections -L$(TOOL_LIB_DIR)
LDFLAGS:= -mmcu=msp430g2231 -L$(TOOL_LIB_DIR)

# Dependancies
DEPS:=$(OBJS:.o=.d)

# Rules
.PHONY: all
all: $(ELF)

$(ELF) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean: 
	rm -rf $(BUILD_DIR)

-include $(DEPS)
