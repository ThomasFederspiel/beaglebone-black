MODULE_INCLUDES += servicecore
MODULE_LIBRARIES += 

DYNAMIC_SYSTEM_LIBRARIES +=
STATIC_SYSTEM_LIBRARIES +=

EXTERNAL_PROJECTS += PruIPC
EXTERNAL_INCLUDES_PruIPC += ipcdevices hostpruaccess prucore ipcservicecore pruipc prucore
EXTERNAL_LIBRARIES_PruIPC += 

EXTERNAL_PROJECTS += Toolbox
EXTERNAL_INCLUDES_Toolbox += service concurrent container exception logger common util fsm console
EXTERNAL_LIBRARIES_Toolbox += 

