BASE_DIR := $(CURDIR)/../..
 
include $(MK_DIR)/defs.mk
include $(MK_DIR)/platform.mk
include $(MK_DIR)/toolchains.mk
include $(MK_DIR)/functions.mk

ARCH := -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon

CROSS_COMPILE := $(call fixpathsep,$(CROSS_TOOLCHAIN)/$(CROSS_PREFIX))
CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
STRIP := $(CROSS_COMPILE)strip

PRU_HEX := $(call fixpathsep,$(PRU_TOOLCHAIN)/bin/hexpru)
PRU_ASM := $(call fixpathsep,$(PRU_TOOLCHAIN)/bin/$(PRU_ASSEMBLER_BIN))
PRU_CC := $(call fixpathsep,$(PRU_TOOLCHAIN)/bin/clpru)
PRU_LD := $(call fixpathsep,$(PRU_TOOLCHAIN)/bin/clpru)
PRU_AR := $(call fixpathsep,$(PRU_TOOLCHAIN)/bin/arpru)

DTC := $(DTC_TOOLCHAIN)dtc

#INCLUDE_DIRS += -I$(SYSROOT)/usr/include
INCLUDE_DIRS += -idirafter $(SYSROOT)/usr/include
LIBRARY_DIRS += -L$(SYSROOT)/usr/lib

ifneq ($(DYNAMIC_SYSTEM_LIBRARIES),)
	LD_DYNAMIC_LIBRARIES += $(addprefix -l, $(DYNAMIC_SYSTEM_LIBRARIES))
endif

ifneq ($(STATIC_SYSTEM_LIBRARIES),)
	LD_STATIC_LIBRARIES += $(addprefix -l, $(STATIC_SYSTEM_LIBRARIES))
endif

include $(MK_DIR)/modules.mk

ifneq ($(EXTERN_INCLUDE),)
	INCLUDE_DIRS += -I$(EXTERN_INCLUDE)
endif

CFLAGS += -MMD
CXXFLAGS += -MMD

ifneq ($(DEBUG),)
	CXXFLAGS += -ggdb
	CFLAGS += -ggdb
endif

ifeq ($(DEBUG),)
ifneq ($(OPTIMIZATION_LEVEL),)
	CXXFLAGS += -O$(OPTIMIZATION_LEVEL)
	CFLAGS += -O$(OPTIMIZATION_LEVEL)
endif
endif

CFLAGS += -std=c99 -Wall -D ARM_HOST $(ARCH) $(INCLUDE_DIRS)
CXXFLAGS += --std=$(CPP_STD) -Wall -D ARM_HOST $(ARCH) $(INCLUDE_DIRS)
LDFLAGS += --sysroot=$(SYSROOT) $(ARCH) $(LIBRARY_DIRS)
#LDFLAGS += $(ARCH) $(LIBRARY_DIRS)
ARFLAGS := rcs

ifneq ($(LD_STATIC_LIBRARIES),)
	LDFLAGS += -Wl,-Bstatic $(LD_STATIC_LIBRARIES)
endif

ifneq ($(LD_DYNAMIC_LIBRARIES),)
	LDFLAGS += -Wl,-Bdynamic $(LD_DYNAMIC_LIBRARIES)
endif

O_FILES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)
D_FILES := $(C_FILES:.c=.d) $(CPP_FILES:.cpp=.d)

O_FILES := $(addprefix $(MODULE_OBJ_DIR)/, $(O_FILES))
D_FILES := $(addprefix $(MODULE_OBJ_DIR)/, $(D_FILES))

# START PRU ------------

PRU_INCLUDE_DIRS += -I$(PRU_TOOLCHAIN)/include -i$(PRU_SSP)/include

include $(MK_DIR)/pru_modules.mk

PRU_ARCH := --silicon_version=2 --hardware_mac=on
PRU_CFLAGS += --c99 --c_extension=.pc $(PRU_ARCH) -O3 -D PRU_HOST $(PRU_INCLUDE_DIRS)
PRU_CXXFLAGS += --cpp_extension=.pcpp $(PRU_ARCH) -O3 -D PRU_HOST $(PRU_INCLUDE_DIRS) 
PRU_LDFLAGS += $(PRU_ARCH) -i$(PRU_TOOLCHAIN)/lib
PRU_ARFLAGS := -ru

PRU_ASM_LISTING := 1

ifneq ($(PRU_ASM_LISTING),)
	PRU_CFLAGS += -al
endif

PRU_ASM_INCLUDE_DIRS += -I$(PRU_SDK_DIR)/include

PRU_BIN_FILES := $(P_FILES:.p=.bin)
PRU_O_FILES := $(PRU_C_FILES:.pc=.obj) $(PRU_CPP_FILES:.pcpp=.obj)
PRU_D_FILES := $(PRU_C_FILES:.pc=.d) $(PRU_CPP_FILES:.pcpp=.d)

PRU_O_FILES := $(addprefix $(MODULE_OBJ_DIR)/, $(PRU_O_FILES))
PRU_D_FILES := $(addprefix $(MODULE_OBJ_DIR)/, $(PRU_D_FILES))

DTBO_FILES := $(DTS_FILES:.dts=.dtbo)

# END PRU ------------

SRC_DIRS := $(patsubst %/,%,$(dir $(C_FILES) $(CPP_FILES) $(C_FILES) $(CPP_FILES) $(PRU_C_FILES) $(PRU_CPP_FILES) $(PRU_C_FILES) $(PRU_CPP_FILES)))

ifneq ($(APPL),)
	APPL := $(MODULE_BIN_DIR)/$(APPL)
endif

ifneq ($(STATIC_LIBRARY),)
	STATIC_LIBRARY := $(MODULE_LIB_DIR)/$(STATIC_LIBRARY)
endif

ifneq ($(PRU_STATIC_LIBRARY),)
	PRU_STATIC_LIBRARY := $(MODULE_LIB_DIR)/$(PRU_STATIC_LIBRARY)
endif

ifneq ($(PRU_APPL),)
	PRU_APPL := $(MODULE_BIN_DIR)/$(PRU_APPL)
	PRU_APPL_ELF := $(PRU_APPL).elf
	PRU_APPL_BIN := $(PRU_APPL_ELF).text.bin
endif

all: createObjRepo createBinRepo createLibRepo $(APPL) $(PRU_APPL_BIN) $(STATIC_LIBRARY) $(PRU_STATIC_LIBRARY) $(PRU_BIN_FILES) $(DTBO_FILES)

$(APPL): $(O_FILES) $(STATIC_LIBRARIES)
	@echo "Linking $(notdir $@)" 
	$(S)$(LD) $(O_FILES) -o $@ $(LDFLAGS) 
ifeq ($(DEBUG),)
	$(S)$(STRIP) $@
endif

$(STATIC_LIBRARIES) :

$(STATIC_LIBRARY): $(O_FILES)
	@echo "Library $(notdir $@)" 
	$(S)$(AR) $(ARFLAGS) $@ $(O_FILES) 
	
$(PRU_STATIC_LIBRARY): $(PRU_O_FILES)
	@echo "PRU Library $(notdir $@)" 
	$(S)$(PRU_AR) $(PRU_ARFLAGS) $@ $(PRU_O_FILES) 
	
$(PRU_APPL_BIN): $(PRU_APPL_ELF) $(PRU_SPLIT_CMD)
	@echo "PRU Binary Splitting $(notdir $<)" 
	$(PRU_HEX) $(PRU_SPLIT_CMD) $< --outfile=$<.text.bin --outfile=$<.data.bin 

$(PRU_APPL_ELF): $(PRU_O_FILES) $(PRU_LD_STATIC_LIBRARIES) $(PRU_LNK_CMD)
	@echo "PRU Linking $(notdir $@)" 
	$(S)$(PRU_LD) $(PRU_LDFLAGS) -z $(PRU_LNK_CMD) -m $(basename $@).map -o $@ $(PRU_O_FILES) $(PRU_LD_STATIC_LIBRARIES)

$(MODULE_OBJ_DIR)/%.o : %.c
	@echo "Compiling $<" 
	$(S)$(CC) $(CDEFS) $(CFLAGS) -c -o $@ $<

$(MODULE_OBJ_DIR)/%.o : %.cpp
	@echo "Compiling $<" 
	$(S)$(CPP) $(CXX_DEFS) $(CXXFLAGS) -c -o $@ $<

$(MODULE_OBJ_DIR)/%.obj : %.pc
	@echo "PRU Compiling $<" 
	$(S)$(PRU_CC) $(PRU_CFLAGS) -ppd=$(MODULE_OBJ_DIR)/$*.d -ppa --obj_directory=$(MODULE_OBJ_DIR) -c $<
	
$(MODULE_OBJ_DIR)/%.obj : %.pcpp
	@echo "PRU Compiling $<" 
	$(S)$(PRU_CC) $(PRU_CXXFLAGS) -ppd=$(MODULE_OBJ_DIR)/$*.d -ppa --obj_directory=$(MODULE_OBJ_DIR) -c $<
	
%.bin : %.p
	@echo "PRU Assembly $<" 
	$(S)$(PRU_ASM) -V2 $(PRU_ASM_INCLUDE_DIRS) -b $<

%.dtbo : %.dts
	@echo "Converting $<" 
#dtc -O dtb -o /lib/firmware/bspwm_P9_29_11-00A0.dtbo -b 0 -@ /lib/firmware/bspwm_P9_29_11-00A0.dts
	$(S)$(CSHELL) $(DTC) -@ -O dtb -o $(realpath $@) $(realpath $<)


-include $(D_FILES)
-include $(PRU_D_FILES)

createObjRepo:
ifdef MODULE_OBJ_DIR
	@$(call make-repo,$(MODULE_OBJ_DIR))
endif
	
createBinRepo:
ifdef MODULE_BIN_DIR
	@$(call make-repo,$(MODULE_BIN_DIR))
endif
	
createLibRepo:
	@$(call make-repo,$(MODULE_LIB_DIR))
	
ifdef WINDOWS
define make-repo
	$(foreach dir, $(call fixpathsep,$(addprefix $(1)/,$(SRC_DIRS))),$(MKDIR) $(dir) &&) @echo off
endef
else
define make-repo
	$(foreach dir,$(call fixpathsep,$(addprefix $(1)/,$(SRC_DIRS))),$(MKDIR) $(dir);)
endef
endif
        
.PHONY	: clean createObjRepo createBinRepo all print
print-%  : ; @echo $* = $($*)

clean :
	-$(S)$(RMDIR) $(call fixpathsep,$(MODULE_OBJ_DIR)) $(DEV_NULL)
	
ifdef MODULE_BIN_DIR
	-$(S)$(RMDIR) $(call fixpathsep,$(MODULE_BIN_DIR)) $(DEV_NULL)
endif

ifdef STATIC_LIBRARY
	-$(S)$(RM) $(call fixpathsep,$(STATIC_LIBRARY)) $(DEV_NULL)
endif

ifdef PRU_STATIC_LIBRARY
	-$(S)$(RM) $(call fixpathsep,$(PRU_STATIC_LIBRARY)) $(DEV_NULL)
endif
	