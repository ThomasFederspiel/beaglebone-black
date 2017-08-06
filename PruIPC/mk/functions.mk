fixpathsep = $(subst /,$(PATHSEP),$(1))

#arg 1 - module
define includedir
$(addsuffix /include,$(addprefix $(BASE_DIR)/src/,$(1)))
endef

#arg 1 - project
define getextpath
$(if $(strip $(EXTERNAL_PATH_$(1))),$(EXTERNAL_PATH_$(1)),$(BASE_DIR)/../$(1))
endef

#arg 1 - project
#arg 2 - module
define extincludedir
$(addsuffix /include,$(addprefix $(call getextpath,$(1))/src/,$(2)))
endef

#arg 1 - project
#arg 2 - module
define extlibdir
$(if $(strip $(1)),$(if $(strip $(2)),$(addprefix $(call getextpath,$(1))/$(LIB_DIR)/,$(2)),$(BASE_DIR)/../$(1)/$(LIB_DIR)),"UNDEFINED")
endef
