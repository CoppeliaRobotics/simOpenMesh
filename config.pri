# openmesh location
OPENMESH_DIR = "e:/Openmesh 6.3"

# location of openmesh header files
OPENMESH_INCLUDEPATH = "$${OPENMESH_DIR}/include"

# openmesh libraries to link
OPENMESH_LIBS = "$${OPENMESH_DIR}/lib/OpenMeshCore.lib" "$${OPENMESH_DIR}/lib/OpenMeshTools.lib"
#OPENMESH_LIBS = -L"$${OPENMESH_DIR}/lib" -lOpenMeshCore -lOpenMeshTools

exists(../config.pri) { include(../config.pri) }

