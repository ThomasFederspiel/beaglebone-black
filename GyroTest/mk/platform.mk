ifdef WINDOWS 
	HOST_PATHSEP=\\
	RM := del /F
	MKDIR := $(subst /,\,$(MK_DIR))\winmkdir
	RMDIR := rmdir /s /q
	DEV_NULL := >nul 2>&1
else
	$(error Linux not supported)
	HOST_PATHSEP=/
	RM := rm -f
	MKDIR := mkdir -p
	DEV_NULL := >/dev/null 2>&1
endif

PATHSEP=$(strip $(HOST_PATHSEP))

