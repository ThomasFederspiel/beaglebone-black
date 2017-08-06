MODULE_INCLUDES += 
MODULE_LIBRARIES += 

DYNAMIC_SYSTEM_LIBRARIES +=
STATIC_SYSTEM_LIBRARIES +=

EXTERNAL_PROJECTS += PruIPC
EXTERNAL_INCLUDES_PruIPC += ipcservicecore
EXTERNAL_LIBRARIES_PruIPC +=

EXTERNAL_PROJECTS += Toolbox
EXTERNAL_INCLUDES_Toolbox += logger container service concurrent exception common
#EXTERNAL_LIBRARIES_<project> += 

EXTERNAL_PROJECTS += CommonServices
EXTERNAL_INCLUDES_CommonServices += prumanagerservice
EXTERNAL_LIBRARIES_CommonServices += prumanagerservice

