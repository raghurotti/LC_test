project(lifecyclemgr)
cmake_minimum_required(VERSION 3.8)

# ============================================================================
# Variables to configure files
# ============================================================================

# TODO DBUS : change the DBUS related name for the server
set(DBUS_SERVICE "com.mm.${PROJECT_NAME}")
set(DBUS_SERVER_PREFIX "com::mm::${PROJECT_NAME}")
set(DBUS_PATH "/com/mm/${PROJECT_NAME}")
#set(CMAKE_CXX_FLAGS "-std=c++11")
include(FindPkgConfig)

# ============================================================================
# Project sources
# ============================================================================

message (STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

set(${PROJECT_NAME}_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/lifecycle_init.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/C_APP_IMP.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/LegatoServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/C_COMMONAPI_SERVER_LifecycleAPI_IMP.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/C_CONTROLLER_IMP.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/commonapi_client.cpp		
	)

set(PRJ_INC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/inc)

message (STATUS "PROJECT_NAME_SRC_FILE = ${${PROJECT_NAME}_SRC_FILES}")

#####  3rd Party
find_package(nre REQUIRED COMPONENTS core dlt)
# Generate NRE sources
find_package(NRE REQUIRED)
GENERATE_NRE_SRC(${PROJECT_NAME})

# Add generated sources files to the source files list
set(${PROJECT_NAME}_SRC_FILES
    ${${PROJECT_NAME}_SRC_FILES}
    ${GENERATED_SRC_FILES}
)
message (STATUS "PROJECT_NAME_SRC_FILE = ${${PROJECT_NAME}_SRC_FILES}")


# ============================================================================
# Project includes
# ============================================================================

# Generic includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${MY_SYSROOT_PATH}/usr/include
    ${GENERATED_API_DIR}
)

# ============================================================================
# Target and build options
# ============================================================================



add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SRC_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PRJ_INC_FILES})

message ("include_directories project ${PRJ_INC_FILES}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if ( DEFINED ${PROJECT_NAME}-CONFIG_GPROF )
    remove_definitions(-fomit-frame-pointer)
    add_definitions(-fno-omit-frame-pointer)
    set_target_properties (${PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS "-g -pg"
        LINK_FLAGS "-pg"
    )
endif ( DEFINED ${PROJECT_NAME}-CONFIG_GPROF )

target_link_libraries (
    ${PROJECT_NAME} PUBLIC
    pthread
    rt
    ${LEGATO_LDFLAGS}
    tbm
    nre::nre-core
    nre::nre-dlt
    ql_lib_lpm
)

# ============================================================================
# Installation
# ============================================================================
include(GNUInstallDirs)
#install (TARGETS  ${PROJECT_NAME} RUNTIME DESTINATION  ${INSTALL_PATH})
install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${LE_BIN_PATH})
