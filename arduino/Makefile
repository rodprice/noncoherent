# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile


# --- assumes src and include directories exist
SRC := src
LOCAL_C_SRCS    := $(wildcard $(SRC)/*.c)
LOCAL_CPP_SRCS  := $(wildcard $(SRC)/*.cpp)
LOCAL_CC_SRCS   := $(wildcard $(SRC)/*.cc)
LOCAL_PDE_SRCS  := $(wildcard $(SRC)/*.pde)
LOCAL_INO_SRCS  := $(wildcard $(SRC)/*.ino)
LOCAL_AS_SRCS   := $(wildcard $(SRC)/*.S)

INC := $(realpath include)
CFLAGS   := -I$(INC)
CXXFLAGS := -I$(INC)

# --- specific to this project
LIB := radiohead
LOCAL_C_SRCS    += $(wildcard $(LIB)/*.c)
LOCAL_CPP_SRCS  += $(wildcard $(LIB)/*.cpp)
ARDUINO_LIBS = SPI SoftwareSerial

# --- pro mini
BOARD_TAG     = pro
BOARD_SUB     = 8MHzatmega328
# MONITOR_PORT  = $(firstword $(wildcard /dev/arduino-?))
MONITOR_PORT  = /dev/arduino-?

# --- arduino system paths
ARDMK_DIR     = /usr/share/arduino
ARDUINO_DIR   = /usr/share/arduino
AVR_TOOLS_DIR = /usr
AVRDUDE_CONF  = /etc/avrdude.conf
ARDUINO_QUIET = 1
CFLAGS   += -Wno-deprecated-declarations
CXXFLAGS += -Wno-deprecated-declarations

include $(ARDMK_DIR)/Arduino.mk


# --- leonardo (or pro micro w/leo bootloader)
#BOARD_TAG    = leonardo
#MONITOR_PORT = /dev/ttyACM0
#include /usr/share/arduino/Arduino.mk

# --- mega2560 ide 1.0
#BOARD_TAG    = mega2560
#ARDUINO_PORT = /dev/ttyACM0
#include /usr/share/arduino/Arduino.mk

# --- mega2560 ide 1.6
#BOARD_TAG    = mega
#BOARD_SUB    = atmega2560
#MONITOR_PORT = /dev/ttyACM0
#ARDUINO_DIR  = /where/you/installed/arduino-1.6.5
#include /usr/share/arduino/Arduino.mk

# --- nano ide 1.0
#BOARD_TAG    = nano328
#MONITOR_PORT = /dev/ttyUSB0
#include /usr/share/arduino/Arduino.mk

# --- nano ide 1.6
#BOARD_TAG   = nano
#BOARD_SUB   = atmega328
#ARDUINO_DIR = /where/you/installed/arduino-1.6.5
#include /usr/share/arduino/Arduino.mk

# --- sparkfun pro micro
#BOARD_TAG         = promicro16
#ALTERNATE_CORE    = promicro
#BOARDS_TXT        = $(HOME)/arduino/hardware/promicro/boards.txt
#BOOTLOADER_PARENT = $(HOME)/arduino/hardware/promicro/bootloaders
#BOOTLOADER_PATH   = caterina
#BOOTLOADER_FILE   = Caterina-promicro16.hex
#ISP_PROG     	   = usbasp
#AVRDUDE_OPTS 	   = -v
#include /usr/share/arduino/Arduino.mk

# --- chipkit
#BOARD_TAG = mega_pic32
#MPIDE_DIR = /where/you/installed/mpide-0023-linux64-20130817-test
#include /usr/share/arduino/chipKIT.mk

# --- pinoccio
#BOARD_TAG         = pinoccio256
#ALTERNATE_CORE    = pinoccio
#BOOTLOADER_PARENT = $(HOME)/arduino/hardware/pinoccio/bootloaders
#BOOTLOADER_PATH   = STK500RFR2/release_0.51
#BOOTLOADER_FILE   = boot_pinoccio.hex
#CFLAGS_STD        = -std=gnu99
#CXXFLAGS_STD      = -std=gnu++11
#include /usr/share/arduino/Arduino.mk

# --- fio
#BOARD_TAG = fio
#include /usr/share/arduino/Arduino.mk

# --- atmega-ng ide 1.6
#BOARD_TAG    = atmegang
#BOARD_SUB    = atmega168
#MONITOR_PORT = /dev/ttyACM0
#ARDUINO_DIR  = /where/you/installed/arduino-1.6.5
#include /usr/share/arduino/Arduino.mk

# --- arduino-tiny ide 1.0
#ISP_PROG     	    = usbasp
#BOARD_TAG          = attiny85at8
#ALTERNATE_CORE     = tiny
#ARDUINO_VAR_PATH   = $(HOME)/arduino/hardware/tiny/cores/tiny
#ARDUINO_CORE_PATH  = $(HOME)/arduino/hardware/tiny/cores/tiny
#AVRDUDE_OPTS 	    = -v
#include /usr/share/arduino/Arduino.mk

# --- arduino-tiny ide 1.6
#ISP_PROG       = usbasp
#BOARD_TAG      = attiny85at8
#ALTERNATE_CORE = tiny
#ARDUINO_DIR    = /where/you/installed/arduino-1.6.5
#include /usr/share/arduino/Arduino.mk

# --- damellis attiny ide 1.0
#ISP_PROG       = usbasp
#BOARD_TAG      = attiny85
#ALTERNATE_CORE = attiny-master
#AVRDUDE_OPTS   = -v
#include /usr/share/arduino/Arduino.mk

# --- damellis attiny ide 1.6
#ISP_PROG       = usbasp
#BOARD_TAG      = attiny
#BOARD_SUB      = attiny85
#ALTERNATE_CORE = attiny
#F_CPU          = 16000000L
#ARDUINO_DIR    = /where/you/installed/arduino-1.6.5
#include /usr/share/arduino/Arduino.mk

# --- teensy3
#BOARD_TAG 	 = teensy31
#ARDUINO_DIR = /where/you/installed/the/patched/teensy/arduino-1.0.6
#include /usr/share/arduino/Teensy.mk

# --- mighty 1284p
#BOARD_TAG         = mighty_opt
#BOARDS_TXT        = $(HOME)/arduino/hardware/mighty-1284p/boards.txt
#BOOTLOADER_PARENT = $(HOME)/arduino/hardware/mighty-1284p/bootloaders
#BOOTLOADER_PATH   = optiboot
#BOOTLOADER_FILE   = optiboot_atmega1284p.hex
#ISP_PROG     	   = usbasp
#AVRDUDE_OPTS 	   = -v
#include /usr/share/arduino/Arduino.mk

# --- atmega328p on breadboard
#BOARD_TAG    = atmega328bb
#ISP_PROG     = usbasp
#AVRDUDE_OPTS = -v
#BOARDS_TXT   = $(HOME)/arduino/hardware/breadboard/boards.txt
#include /usr/share/arduino/Arduino.mk
