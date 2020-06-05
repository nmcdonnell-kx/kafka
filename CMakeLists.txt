# Environment variable for HDF5_HOME is required
if(NOT DEFINED ENV{BUILD_HOME})
        message(FATAL_ERROR "BUILD_HOME must be set to the location of your installed librdkafka repository")
endif()

cmake_minimum_required(VERSION 2.8)
project(kafka C)

# Default such that 'make install' moves binary & q script to QHOME
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE MinSizeRel)
ENDIF()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-strict-aliasing -Wno-parentheses -Wextra -Werror -Wsign-compare -DKXVER=3")

set(MY_LIBRARY_NAME kfk)
file(GLOB SRC_FILES kfk.c)
set_source_files_properties(${SRC_FILES} PROPERTIES LANGUAGE C)

if(STATIC_LINKING)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif(STATIC_LINKING)

message(STATUS "Generator : ${CMAKE_GENERATOR}")
message(STATUS "Build Tool : ${CMAKE_BUILD_TOOL}")
message(STATUS "KAFKA API : $ENV{BUILD_HOME}")

include_directories (
    $ENV{BUILD_HOME}/include
    ${CMAKE_BINARY_DIR}
)

find_library(KAFKA_LIBRARY
    NAMES rdkafka librdkafka
    HINTS "$ENV{BUILD_HOME}/lib/" "$ENV{BUILD_HOME}/lib/Win64"
)

file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/c/c/k.h" "${CMAKE_BINARY_DIR}/k.h" )

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(INSTALL_SCRIPT "install.bat")
    file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/w64/q.lib" "${CMAKE_BINARY_DIR}/q.lib" )
    set(LINK_LIBS "${CMAKE_BINARY_DIR}/q.lib")
else()
    set(INSTALL_SCRIPT "install.sh")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-strict-aliasing")
endif()
    
add_library (${MY_LIBRARY_NAME} SHARED ${SRC_FILES})

IF(APPLE)
   set_target_properties(${MY_LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
   set_target_properties(${MY_LIBRARY_NAME} PROPERTIES SUFFIX .so)
endif()

if(STATIC_LINKING)
  SET(CMAKE_EXE_LINKER_FLAGS "-static")
  SET_TARGET_PROPERTIES(${MY_LIBRARY_NAME} PROPERTIES LINK_SEARCH_END_STATIC 1)
endif(STATIC_LINKING)

target_link_libraries(${MY_LIBRARY_NAME} ${KAFKA_LIBRARY} ${LINK_LIBS})

# Check if 32-bit/64-bit machine
set(BITNESS 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BITNESS 64)
endif()

# Minimum build/install move binary and q-script to $QHOME & $QHOME/<QARCH>
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION "$ENV{QHOME}/${OSFLAG}${BITNESS}/" CONFIGURATIONS MinSizeRel Release)
install(DIRECTORY "q/" DESTINATION "$ENV{QHOME}" CONFIGURATIONS MinSizeRel Release FILES_MATCHING PATTERN "*.q")

install(FILES README.md LICENSE ${INSTALL_SCRIPT} DESTINATION ${PROJECT_BINARY_DIR}/kafka CONFIGURATIONS Release)
install(DIRECTORY examples DESTINATION ${PROJECT_BINARY_DIR}/kafka CONFIGURATIONS Release)
install(DIRECTORY q DESTINATION ${PROJECT_BINARY_DIR}/kafka CONFIGURATIONS Release)
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION ${PROJECT_BINARY_DIR}/kafka CONFIGURATIONS Release)

