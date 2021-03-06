# project name
PROJECT(matador)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project version
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

SET(APP_MAJOR_VERSION 0)
SET(APP_MINOR_VERSION 5)
SET(APP_PATCH_LEVEL 0)
SET(APP_VERSION "${APP_MAJOR_VERSION}.${APP_MINOR_VERSION}.${APP_PATCH_LEVEL}")

MESSAGE(STATUS "${PROJECT_NAME_UPPER} ${APP_VERSION}")


# Common compiler flags
# These are defined for clang/gcc compatibility.
# When non-compatible flags are implemented then we must specify them separately.
SET(GCC_CLANG_COMMON_FLAGS "-std=c++14 -Wall -Wextra -pedantic")
SET(GCC_CLANG_COMMON_FLAGS_DEBUG "-O0 -g -DDEBUG")
SET(GCC_CLANG_COMMON_FLAGS_RELEASE "-O3 -DNDEBUG")
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

IF(CMAKE_COMPILER_IS_GNUCXX)
    MESSAGE(STATUS "GCC detected - Adding compiler flags")
    SET(CMAKE_CXX_FLAGS "${GCC_CLANG_COMMON_FLAGS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "${GCC_CLANG_COMMON_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${GCC_CLANG_COMMON_FLAGS_RELEASE}")

ELSEIF(CMAKE_CXX_COMPILER MATCHES "clang")
    MESSAGE(STATUS "Clang detected - Adding compiler flags")
    SET(CMAKE_CXX_FLAGS "${GCC_CLANG_COMMON_FLAGS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "${GCC_CLANG_COMMON_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${GCC_CLANG_COMMON_FLAGS_RELEASE}")

ELSEIF(MSVC)
    MESSAGE(STATUS "MSVC detected - Adding compiler flags")
    SET(CMAKE_CXX_FLAGS "/std:c++14 /W3 /EHsc /bigobj")
    SET(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /Zi /D_DEBUG /DDEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
ENDIF()


MESSAGE(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "Architecture System: ${CMAKE_SYSTEM_NAME}")


SET(SYSTEM_NAME_LOWER)
STRING(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)

# add module path
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

IF (NOT CMAKE_BUILD_TYPE)
  MESSAGE(STATUS "No build type selected, default to Release")
  SET(CMAKE_BUILD_TYPE "Release")
ELSE()
  MESSAGE(STATUS "Current build type ${CMAKE_BUILD_TYPE}")
ENDIF()

OPTION(COVERAGE "Enable generation of code coverage" false)
OPTION(ARCH "Compiler architecture for Clang/GCC" "")

if (NOT MSVC AND (COVERAGE) AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_CXX_COMPILER MATCHES "clang")
  MESSAGE(STATUS "coverage for compiler ${CMAKE_CXX_COMPILER}")

  SET(GCOV_BINARY "gcov" CACHE STRING "gcov binary")

  MESSAGE(STATUS "gcov binary: ${GCOV_BINARY}")

  INCLUDE(CodeCoverage)
  MESSAGE(STATUS "will generate coverage data")
  SET(CMAKE_BUILD_TYPE "Debug")
  SET(GCOV_ADDITIONAL_FLAGS "-fprofile-arcs -ftest-coverage")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCOV_ADDITIONAL_FLAGS}")
  SET(CMAKE_C_FLAGS "{CMAKE_C_FLAGS} ${GCOV_ADDITIONAL_FLAGS}")
  SET(CMAKE_EXE_LINKER_FLAGS "${GCOV_ADDITIONAL_FLAGS}")

  SETUP_TARGET_FOR_COVERAGE(coverage test_matador src coverage "exec;all")
else()
  MESSAGE(STATUS "no coverage for compiler ${CMAKE_CXX_COMPILER}")
endif()

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
IF (WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
  SET(CMAKE_MODULE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
ELSE()
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_MODULE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ENDIF()

#
# configure include dir
#
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

SET(BACKENDS
  SQLite3
  MySQL
  ODBC
)

FOREACH(backend ${BACKENDS})
  STRING(TOUPPER ${backend} BACKEND_UPPER)
  OPTION(${PROJECT_NAME_UPPER}_${BACKEND_UPPER} "Enable or disable backend ${backend}" TRUE)
  OPTION(${PROJECT_NAME_UPPER}_${BACKEND_UPPER}_TEST "Enable or disable backend ${backend} tests" TRUE)

  IF (${PROJECT_NAME_UPPER}_${BACKEND_UPPER})
    FIND_PACKAGE(${backend})
    IF (${BACKEND_UPPER}_FOUND)
      MESSAGE(STATUS "Found backend ${backend}")
      ADD_DEFINITIONS(-D${PROJECT_NAME_UPPER}_${BACKEND_UPPER})
      IF (${PROJECT_NAME_UPPER}_${BACKEND_UPPER}_TEST)
        MESSAGE(STATUS "Enable ${backend} tests")
        ADD_DEFINITIONS(-D${PROJECT_NAME_UPPER}_${BACKEND_UPPER}_TEST)
      ENDIF()
    ELSE()
      MESSAGE(STATUS "No backend ${backend}")
    ENDIF()
  ELSE()
    MESSAGE(STATUS "Backend ${backend} disabled")
  ENDIF()
ENDFOREACH(backend)

IF (SQLITE3_FOUND)
  MESSAGE(STATUS "Adding SQLite3 include directory")
  INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIR})
ELSE ()
  MESSAGE(STATUS "Not adding SQLite3 include directory")
ENDIF ()

IF (MYSQL_FOUND)
  INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
ENDIF ()

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# https://cmake.org/Wiki/CMake_RPATH_handling
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_MACOSX_RPATH TRUE)

# contains the full path to the top level directory of your build tree
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

MESSAGE( STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR} )

ENABLE_TESTING()


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(test)

INSTALL(
  DIRECTORY ${PROJECT_BINARY_DIR}/doc/web/
  DESTINATION share/doc/matador
  COMPONENT doc
)

# Basic CPack configuration
SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR "zussel")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "matador - take your database by the horns")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/License")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")

SET(CPACK_PACKAGE_VERSION_MAJOR ${APP_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${APP_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${APP_PATCH_LEVEL})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
  if (${ARCH} MATCHES "32")
    SET(CPACK_SYSTEM_NAME "x86-${SYSTEM_NAME_LOWER}")
  elseif(${ARCH} MATCHES "64")
    SET(CPACK_SYSTEM_NAME "x86_64-${SYSTEM_NAME_LOWER}")
  else()
    SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}-${SYSTEM_NAME_LOWER}")
  endif()
endif()

# Tell CPack all of the components to install. The "ALL"
# refers to the fact that this is the set of components that
# will be included when CPack is instructed to put everything
# into the binary installer (the default behavior).
SET(CPACK_COMPONENTS_ALL libraries headers doc)

# Set the displayed names for each of the components to install.
# These will be displayed in the list of components inside the installer.
SET(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME ${PROJECT_NAME})
SET(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
SET(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "${PROJECT_NAME} Headers")

# Provide descriptions for each of the components to install.
# When the user hovers the mouse over the name of a component,
# the description will be shown in the "Description" box in the
# installer. If no descriptions are provided, the "Description"
# box will be removed.
#SET(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION
#  "Store all your objects in one container.")
SET(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Take your database by the horns")
SET(CPACK_COMPONENT_HEADERS_DESCRIPTION "C/C++ header files for ${PROJECT_NAME}")

# Put the components into two different groups: "Runtime" and "Development"
#SET(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
SET(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
SET(CPACK_COMPONENT_HEADERS_GROUP "Development")

# Expand the "Development" group by default, since we have so few components.
# Also, provide this group with a description.
SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_EXPANDED ON)
SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "All of the tools you'll ever need to develop software")

# It doesn't make sense to install the headers without the libraries
# (because you could never use the headers!), so make the headers component
# depend on the libraries component.
SET(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

# Create two installation types with pre-selected components.
# The "Developer" installation has just the library and headers,
# while the "Full" installation has everything.
SET(CPACK_ALL_INSTALL_TYPES Full Developer)
SET(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")
SET(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full)
SET(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full)
#SET(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Full)

SET(CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}/Package")

# Include CPack to introduce the appropriate targets
INCLUDE(CPack)
