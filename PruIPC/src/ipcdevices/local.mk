MODULE_INCLUDES += pruipc prucore pru hostpruaccess ipcservicecore
MODULE_LIBRARIES +=

DYNAMIC_SYSTEM_LIBRARIES +=
STATIC_SYSTEM_LIBRARIES +=
	
EXTERNAL_PROJECTS += Toolbox
EXTERNAL_INCLUDES_Toolbox += exception common logger service concurrent container util
EXTERNAL_LIBRARIES_<project> += 
