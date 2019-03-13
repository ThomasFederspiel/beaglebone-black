MODULE_INCLUDES += servicecore motorregulator
MODULE_LIBRARIES += 

DYNAMIC_SYSTEM_LIBRARIES +=
STATIC_SYSTEM_LIBRARIES +=

EXTERNAL_PROJECTS += PruIPC
EXTERNAL_INCLUDES_PruIPC += ipcservicecore ipcdevices hostpruaccess prucore pruipc
EXTERNAL_LIBRARIES_PruIPC +=

EXTERNAL_PROJECTS += Toolbox
EXTERNAL_INCLUDES_Toolbox += logger container service concurrent exception common boostasio util protocol factory algorithms util console
#EXTERNAL_LIBRARIES_<project> += 

#EXTERNAL_PROJECTS += <project>
#EXTERNAL_INCLUDES_<project> += 
#EXTERNAL_LIBRARIES_<project> +=

