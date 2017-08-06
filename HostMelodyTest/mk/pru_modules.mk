ifeq ($(MODULE),)
    $(error "MODULE not defined")
endif

PRU_MODULE_INCLUDES += $(MODULE) 

PRU_INCLUDE_DIRS += $(addprefix -I$(BASE_DIR)/src/,$(MODULE))

ifneq ($(PRU_MODULE_INCLUDES),)
    PRU_INCLUDE_DIRS += $(addprefix -I,$(call includedir,$(PRU_MODULE_INCLUDES)))
endif

ifneq ($(PRU_EXTERNAL_PROJECTS),)
PRU_EXTERNAL_INCLUDE_DIRS := $(foreach project, $(PRU_EXTERNAL_PROJECTS),$(if $(strip $(PRU_EXTERNAL_INCLUDES_$(project))),$(addprefix -I,$(call extincludedir,$(project),$(PRU_EXTERNAL_INCLUDES_$(project))))))

ifneq ($(PRU_EXTERNAL_INCLUDE_DIRS),)
    PRU_INCLUDE_DIRS += $(PRU_EXTERNAL_INCLUDE_DIRS)
endif
endif

ifneq ($PRU_MODULE_LIBRARIES),)
    PRU_LD_STATIC_LIBRARIES += $(addprefix $(MODULE_LIB_DIR)/,$(addprefix lib,$(addsuffix .lib,$(PRU_MODULE_LIBRARIES))))
endif

ifneq ($(PRU_EXTERNAL_PROJECTS),)
PRU_EXTERNAL_LD_STATIC_LIBRARIES := $(foreach project, $(PRU_EXTERNAL_PROJECTS),$(if $(strip $(PRU_EXTERNAL_LIBRARIES_$(project))),$(call extlibdir,$(project),$(addprefix lib,$(addsuffix .lib,$(PRU_EXTERNAL_LIBRARIES_$(project)))))))

ifneq ($(PRU_EXTERNAL_LD_STATIC_LIBRARIES),)
    PRU_LD_STATIC_LIBRARIES += $(PRU_EXTERNAL_LD_STATIC_LIBRARIES)
endif
endif


