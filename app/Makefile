# TARGET #


TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif
endif

# COMMON CONFIGURATION #

NAME := Artic Base

BUILD_DIR := build
OUTPUT_DIR := output
SOURCE_DIRS := sources
INCLUDE_DIRS := $(SOURCE_DIRS) includes

EXTRA_OUTPUT_FILES :=

LIBRARY_DIRS := $(PORTLIBS) $(CTRULIB) $(DEVKITPRO)/libcwav $(DEVKITPRO)/libncsnd
LIBRARIES := ctru

VERSION_MAJOR := 1
VERSION_MINOR := 0
VERSION_MICRO := 0

BUILD_FLAGS := -march=armv6k -mtune=mpcore -mfloat-abi=hard
BUILD_FLAGS_CC := -g -Wall -Wno-strict-aliasing -O3 -mword-relocations \
					-fomit-frame-pointer -ffast-math $(ARCH) $(INCLUDE) -D__3DS__ $(BUILD_FLAGS) \
					-DVERSION_MAJOR=${VERSION_MAJOR} \
					-DVERSION_MINOR=${VERSION_MINOR} \
					-DVERSION_REVISION=${VERSION_MICRO}

BUILD_FLAGS_CXX := $(BUILD_FLAGS_CC) $(COMMON_FLAGS) -fno-rtti -fno-exceptions -std=gnu++20
RUN_FLAGS :=

# 3DS/Wii U CONFIGURATION #

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
	TITLE := Artic Base
	DESCRIPTION := 3DS Game Broadcaster
    AUTHOR := PabloMK7
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/3ds/
    LIBRARIES += ctru

    PRODUCT_CODE := CTR-P-ARBS
    UNIQUE_ID := 0xABA5E

    CATEGORY := Application
    USE_ON_SD := true

    MEMORY_TYPE := Application
    SYSTEM_MODE := 64MB
    SYSTEM_MODE_EXT := Legacy
    CPU_SPEED := 268MHz
    ENABLE_L2_CACHE := true

    ICON_FLAGS := --flags visible,recordusage

    BANNER_AUDIO := resources/audio.cwav
    
    BANNER_IMAGE := resources/banner.png
    
	ICON := resources/icon.png

endif

# INTERNAL #

include buildtools/make_base

re : clean all
.PHONY: re