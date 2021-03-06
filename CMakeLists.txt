#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

#Set a new CMake project
project(binson C)

option (WITH_STATIC_LIB "Build binson lib as static instead of dynamic" OFF) 
option (WITH_BINSON_JSON_OUTPUT "Build lib with JSON output support" ON) 
option (WITH_EXAMPLES "Build examples from ./example" ON) 
option (WITH_TESTING "Build tests" ON)
option (WITH_FUZZING "Build fuzzing stress suite" ON) 

# The version number.
set (BINSON_MAJOR_VERSION 0)
set (BINSON_MINOR_VERSION 1)
set (BINSON_MICRO_VERSION 0)

set(BINSON_VERSION "${BINSON_MAJOR_VERSION}.${BINSON_MINOR_VERSION}.${BINSON_MICRO_VERSION}")

# SOVERSION scheme: CURRENT.AGE.REVISION
#   If there was an incompatible interface change:
#     Increment CURRENT. Set AGE and REVISION to 0
#   If there was a compatible interface change:
#     Increment AGE. Set REVISION to 0
#   If the source code was changed, but there were no interface changes:
#     Increment REVISION.
#set(LIBRARY_VERSION "0.0.5")
set(LIBRARY_VERSION ${BINSON_VERSION})
set(LIBRARY_SOVERSION "0")

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules
)

# Default is Debug build type.  Use  cmake -DCMAKE_BUILD_TYPE=Release  to override
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
  set(COMMON_C_FLAGS  "-fPIC -ansi -pedantic -pedantic-errors -Wall -W  -Wsign-conversion -Wshadow -Wpointer-arith -Wcast-align  -Wnested-externs -fshort-enums -fno-common -Dinline=  ")
  if(NOT WIN32)
    set(COMMON_C_FLAGS "${COMMON_C_FLAGS} -Werror")
  endif()  
  set(CMAKE_C_FLAGS "-g -O0 ${COMMON_C_FLAGS} ${CMAKE_C_FLAGS}" )
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${COMMON_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} " )
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O2 ${COMMON_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO} ")
  set(CMAKE_C_FLAGS_RELEASE "-O2 -s -DNDEBUG ${COMMON_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} ")  
endif()

# platform endiannes checks
include(TestBigEndian)
TEST_BIG_ENDIAN(BIGENDIAN)
if (NOT ${BIGENDIAN})
  set (IS_LITTLE_ENDIAN 1)
endif (NOT ${BIGENDIAN})

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(DEBUG 1)
else()
    set(NDEBUG 1)
endif()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/binson_config.h.in"
  "${PROJECT_BINARY_DIR}/binson_config.h"
  )
  
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")  

file(GLOB SOURCES "src/*.c")

if (WITH_STATIC_LIB)
  add_library(binson STATIC ${SOURCES})
else() 
  add_library(binson SHARED ${SOURCES})
endif()

# target_include_directories is not supported for early versions
#if (NOT("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_LESS 2.8.12))
#if (NOT("${CMAKE_}" VERSION_LESS 2.8.12))

if(${CMAKE_VERSION} VERSION_LESS 2.8.12)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
else ()
  target_include_directories(binson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)
endif()

message("${CMAKE_INCUDE_DIR}")

if (WITH_EXAMPLES)
    add_subdirectory(example)
endif (WITH_EXAMPLES)


########################################################################
## Testing

if (WITH_TESTING)
  enable_testing()
  include(CTest)
  #find_package(CMocka REQUIRED)  
  #include(AddCMockaTest)      
  add_subdirectory(test)
endif (WITH_TESTING)


########################################################################
## Build Summary

message( STATUS
    "\n====================|  Binson Build Summary  |====================="
    "\n"
    "\nBuild type:        ${CMAKE_BUILD_TYPE}"    
    "\nInstall prefix:    ${CMAKE_INSTALL_PREFIX}"
    "\n"
    "\nCC:                ${CMAKE_C_COMPILER}"
    "\n================================================================\n"
)

########################################################################
## Installation

install(TARGETS binson DESTINATION lib)

include(GNUInstallDirs)
install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
        
install(FILES
  "${PROJECT_BINARY_DIR}/binson_config.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )        

#install(FILES .... DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )


########################################################################
## Documentation

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(WITH_DOCS "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(WITH_DOCS)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()