ifeq ($(MODULE),)
    $(error "MODULE not defined")
endif

INCLUDE_DIRS += $(addprefix -I$(BASE_DIR)/src/,$(MODULE))

MODULE_INCLUDES += $(MODULE) 

ifneq ($(MODULE_INCLUDES),)
    INCLUDE_DIRS += $(addprefix -I,$(call includedir,$(MODULE_INCLUDES)))
endif

ifneq ($(EXTERNAL_PROJECTS),)
EXTERNAL_INCLUDE_DIRS := $(foreach project, $(EXTERNAL_PROJECTS),$(if $(strip $(EXTERNAL_INCLUDES_$(project))),$(addprefix -I,$(call extincludedir,$(project),$(EXTERNAL_INCLUDES_$(project))))))

ifneq ($(EXTERNAL_INCLUDE_DIRS),)
    INCLUDE_DIRS += $(EXTERNAL_INCLUDE_DIRS)
endif

endif

ifneq ($(MODULE_LIBRARIES),)
    LIBRARY_DIRS += -L$(MODULE_LIB_DIR)/
    LD_STATIC_LIBRARIES += $(addprefix -l,$(MODULE_LIBRARIES))
    STATIC_LIBRARIES += $(addprefix $(MODULE_LIB_DIR)/,$(MODULE_LIBRARIES))
endif

ifneq ($(EXTERNAL_PROJECTS),)
EXTERNAL_LIB_DIRS := $(foreach project, $(EXTERNAL_PROJECTS),$(if $(strip $(EXTERNAL_LIBRARIES_$(project))),$(call extlibdir,$(project),)))

EXTERNAL_LIBRARIES := $(foreach project, $(EXTERNAL_PROJECTS),$(if $(strip $(EXTERNAL_LIBRARIES_$(project))),$(EXTERNAL_LIBRARIES_$(project))))

EXTERNAL_LIBRARY_PATHS := $(foreach project, $(EXTERNAL_PROJECTS),$(if $(strip $(EXTERNAL_LIBRARIES_$(project))),$(call extlibdir,$(project),$(EXTERNAL_LIBRARIES_$(project)))))

ifneq ($(EXTERNAL_LIB_DIRS),)
    LIBRARY_DIRS += $(addprefix -L,$(EXTERNAL_LIB_DIRS))
    LD_STATIC_LIBRARIES += $(addprefix -l,$(EXTERNAL_LIBRARIES))
    STATIC_LIBRARIES += $(EXTERNAL_LIBRARY_PATHS)
endif
endif

ifneq ($(EXPORT_INCLUDE),)
	INCLUDE_DIRS += $(addsuffix /include,$(addprefix -I$(BASE_DIR)/../,$(EXPORT_INCLUDE)))
endif
