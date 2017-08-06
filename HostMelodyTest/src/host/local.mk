MODULE_INCLUDES += core export
MODULE_LIBRARIES += core

#DYNAMIC_SYSTEM_LIBRARIES += prussdrv boost_filesystem boost_system
DYNAMIC_SYSTEM_LIBRARIES += boost_filesystem boost_system pthread boost_regex
STATIC_SYSTEM_LIBRARIES +=

EXTERNAL_PROJECTS += CommonServices
EXTERNAL_INCLUDES_CommonServices += soundservice prumanagerservice motorregulator servicecore telnetservice
EXTERNAL_LIBRARIES_CommonServices += soundservice prumanagerservice motorregulator telnetservice

EXTERNAL_PROJECTS += PruIPC
EXTERNAL_PATH_PruIPC += ../../../PruIPC
EXTERNAL_INCLUDES_PruIPC += hostpruaccess ipcdevices prucore pruipc ipcservicecore
EXTERNAL_LIBRARIES_PruIPC += pruipcdevices pruipchostpruaccess pruipcservicecore pruipc

EXTERNAL_PROJECTS += Toolbox
EXTERNAL_INCLUDES_Toolbox += common util exception logger service concurrent container console
EXTERNAL_LIBRARIES_Toolbox += tboxcommon tboxutil tboxexception tboxlogger prussdrv tboxservice tboxconcurrent boostasio

