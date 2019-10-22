# Generated on 2019-4-30 18:10:57 by version CURRENT_PROJECT_VERSION of mulle-xcode-to-cmake
# Command line:
#    mulle-xcode-to-cmake -t unit_c_kit -t unit_objc_kit -t UnitKit -t ukrun export UnitKit.xcodeproj


cmake_minimum_required (VERSION 3.4)
project( UnitKit C)

#
# For mulle-sde support (mulle-objc >= 0.15 needed):
#    mulle-sde init -m foundation/objc-porter executable
#
set( FIND_FOUNDATION_LIBRARY ON) # default

if( NOT DEPENDENCY_DIR)
   set( DEPENDENCY_DIR "$ENV{DEPENDENCY_DIR}")
endif()
if( NOT DEPENDENCY_DIR)
   set( DEPENDENCY_DIR "$PROJECT_SOURCE_DIR}/dependency")
endif()

if( EXISTS "${DEPENDENCY_DIR}")
   if( NOT ADDICTION_DIR)
      get_filename_component( ADDICTION_DIR "${DEPENDENCY_DIR}" DIRECTORY)
      set( ADDICTION_DIR "${ADDICTION_DIR}/addiction")
   endif()

   message( STATUS "Determine link commandline from dependency linkorder")

   # prefer mulle-sde Foundation
   set( CMAKE_FIND_FRAMEWORK "LAST")

   # set install for RPATH
   # https://stackoverflow.com/questions/43551483/how-to-set-rpath-in-cmake
   set( CMAKE_MACOSX_RPATH ON)
   SET( CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
   SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

   include_directories( BEFORE SYSTEM
      ${DEPENDENCY_DIR}/include
      ${ADDICTION_DIR}/include
   )
   set( CMAKE_FRAMEWORK_PATH
      ${CMAKE_FRAMEWORK_PATH}
      ${DEPENDENCY_DIR}/Frameworks
      ${ADDICTION_DIR}/Frameworks
   )
   set( CMAKE_LIBRARY_PATH
      ${CMAKE_LIBRARY_PATH}
      ${DEPENDENCY_DIR}/lib
      ${ADDICTION_DIR}/lib
   )

   set( FIND_FOUNDATION_LIBRARY OFF) # default

   execute_process( COMMAND mulle-sde stash-dir
                    OUTPUT_VARIABLE MULLE_SOURCETREE_STASH_DIR)
   string( STRIP "${MULLE_SOURCETREE_STASH_DIR}" MULLE_SOURCETREE_STASH_DIR)

   #
   # the linkorder command is run in the project directory of unitkit-fixes
   # the stash directory may be managed by some other project though!
   #
   execute_process( COMMAND mulle-sde linkorder --stash-dir "${MULLE_SOURCETREE_STASH_DIR}" --startup --simplify --output-format cmake
                    OUTPUT_VARIABLE MULLE_SDE_LINKER_FLAGS
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                    RESULT_VARIABLE RVAL)
   if( NOT ${RVAL} EQUAL 0)
      message( FATAL_ERROR "Failed to procure linkorder from mulle-sde. Old version ?")
   endif()
   message( STATUS "MULLE_SOURCETREE_STASH_DIR=\"${MULLE_SOURCETREE_STASH_DIR}\"")
   message( STATUS "PROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"")
   message( STATUS "MULLE_SDE_LINKER_FLAGS=\"${MULLE_SDE_LINKER_FLAGS}\"")
   set( MULLE_SDE_LINKER_FLAGS
        ${MULLE_SDE_LINKER_FLAGS}
        CACHE INTERNAL "cache this for posterity"
   )
else()
   message( STATUS "This is not a mulle-sde build (\"${DEPENDENCY_DIR}\" does not exist)")
endif()


message( STATUS "CMAKE_C_FLAGS=\"${CMAKE_C_FLAGS}\"")

#
# Platform specific definitions
#

if( APPLE)
   # # CMAKE_OSX_SYSROOT must be set for CMAKE_OSX_DEPLOYMENT_TARGET (cmake bug)
   # if( NOT CMAKE_OSX_SYSROOT)
   #    set( CMAKE_OSX_SYSROOT "/" CACHE STRING "SDK for OSX" FORCE)   # means current OS X
   # endif()
   #
   # # baseline set to 10.6 for rpath
   # if( NOT CMAKE_OSX_DEPLOYMENT_TARGET)
   #   set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6" CACHE STRING "Deployment target for OSX" FORCE)
   # endif()

   set( CMAKE_POSITION_INDEPENDENT_CODE FALSE)

   set( BEGIN_ALL_LOAD "-all_load")
   set( END_ALL_LOAD)
else()
   set( CMAKE_POSITION_INDEPENDENT_CODE TRUE)

   if( WIN32)
   # windows
   else()
   # linux / gcc
      set( BEGIN_ALL_LOAD "-Wl,--whole-archive")
      set( END_ALL_LOAD "-Wl,--no-whole-archive")
   endif()
endif()


##
## unit_c_kit Files
##

# uncomment this for mulle-objc to search libraries first
# set( CMAKE_FIND_FRAMEWORK "LAST")

set( UNIT_C_KIT_INCLUDE_DIRS
Source/SharedLibrarySource
)

set( UNIT_C_KIT_PRIVATE_HEADERS
Source/SharedLibrarySource/uk_handler.h
)

set( UNIT_C_KIT_SOURCES
Source/SharedLibrarySource/uk_handler.c
Source/SharedLibrarySource/uk_handler_plain_c.c
)


##
## unit_objc_kit Files
##

# uncomment this for mulle-objc to search libraries first
# set( CMAKE_FIND_FRAMEWORK "LAST")

set( UNIT_OBJC_KIT_INCLUDE_DIRS
Source/FrameworkSource
Source/SharedLibrarySource
)

set( UNIT_OBJC_KIT_PUBLIC_HEADERS
Source/FrameworkSource/UKTest.h
Source/FrameworkSource/UKTestHandler.h
Source/FrameworkSource/UnitKit.h
Source/FrameworkSource/UnitKitParentIncludes.h
)

set( UNIT_OBJC_KIT_PRIVATE_HEADERS
Source/SharedLibrarySource/uk_handler.h
)

set( UNIT_OBJC_KIT_SOURCES
Source/SharedLibrarySource/uk_handler.c
Source/SharedLibrarySource/uk_handler_objc.m
)


##
## UnitKit Files
##

# uncomment this for mulle-objc to search libraries first
# set( CMAKE_FIND_FRAMEWORK "LAST")

set( UNIT_KIT_INCLUDE_DIRS
Source/FrameworkSource
Source/SharedLibrarySource
)

set( UNIT_KIT_PUBLIC_HEADERS
Source/FrameworkSource/UKRunner.h
Source/FrameworkSource/UKTest.h
Source/FrameworkSource/UKTestHandler.h
Source/FrameworkSource/UnitKit.h
Source/FrameworkSource/UnitKitParentIncludes.h
)

set( UNIT_KIT_PROJECT_HEADERS
Source/FrameworkSource/_UnitKitParentIncludes.h
)

set( UNIT_KIT_RESOURCES
LICENSE
NOTICE
README
Source/FrameworkSource/English.lproj/InfoPlist.strings
Source/FrameworkSource/English.lproj/UKRunner.strings
Source/FrameworkSource/English.lproj/UKTestHandler.strings
)

set( UNIT_KIT_SOURCES
Source/FrameworkSource/UKRunner.m
Source/FrameworkSource/UKTestHandler.m
)



##
## ukrun Files
##

# uncomment this for mulle-objc to search libraries first
# set( CMAKE_FIND_FRAMEWORK "LAST")

set( UKRUN_SOURCES
Source/ToolSource/main.m
)


if( FIND_FOUNDATION_LIBRARY)
   find_library( FOUNDATION_LIBRARY Foundation)
   message( STATUS "FOUNDATION_LIBRARY is ${FOUNDATION_LIBRARY}")
endif()


set( UKRUN_DEPENDENCIES
   ${FOUNDATION_LIBRARY}
)

set( UKRUN_STATIC_DEPENDENCIES
)



##
## unit_c_kit
##

add_library( unit_c_kit STATIC
${UNIT_C_KIT_SOURCES}
${UNIT_C_KIT_PUBLIC_HEADERS}
${UNIT_C_KIT_PROJECT_HEADERS}
${UNIT_C_KIT_PRIVATE_HEADERS}
${UNIT_C_KIT_RESOURCES}
)

target_include_directories( unit_c_kit
   PUBLIC
Source/FrameworkSource
Source/SharedLibrarySource
)

install( TARGETS unit_c_kit DESTINATION "lib")


##
## unit_objc_kit
##

add_library( unit_objc_kit STATIC
${UNIT_OBJC_KIT_SOURCES}
${UNIT_OBJC_KIT_PUBLIC_HEADERS}
${UNIT_OBJC_KIT_PROJECT_HEADERS}
${UNIT_OBJC_KIT_PRIVATE_HEADERS}
${UNIT_OBJC_KIT_RESOURCES}
)

target_include_directories( unit_objc_kit
   PUBLIC
Source/FrameworkSource
Source/SharedLibrarySource
)

install( TARGETS unit_objc_kit DESTINATION "lib")


##
## UnitKit
##

add_library( UnitKit SHARED
${UNIT_KIT_SOURCES}
${UNIT_KIT_PUBLIC_HEADERS}
${UNIT_KIT_PROJECT_HEADERS}
${UNIT_KIT_PRIVATE_HEADERS}
${UNIT_KIT_RESOURCES}
)

target_include_directories( UnitKit
   PUBLIC
Source/FrameworkSource
Source/SharedLibrarySource
)


if (APPLE)
   set_target_properties( UnitKit PROPERTIES
FRAMEWORK TRUE
FRAMEWORK_VERSION A
# MACOSX_FRAMEWORK_IDENTIFIER "com.mulle-kybernetik.UnitKit"
# VERSION "0.0.0"
# SOVERSION  "0.0.0"
PUBLIC_HEADER "${UNIT_KIT_PUBLIC_HEADERS}"
PRIVATE_HEADER "${UNIT_KIT_UNIT_KIT_PUBLIC_HEADERS}"
RESOURCE "${UNIT_KIT_RESOURCES}"
)

   target_link_libraries( UnitKit
"-undefined dynamic_lookup"
   )

   # export some symbols from "ukrun" for macos
   set( MULLE_SDE_LINKER_FLAGS
      ${MULLE_SDE_LINKER_FLAGS}
      "-Wl,-exported_symbol" "-Wl,___register_mulle_objc_universe"
      "-Wl,-exported_symbol" "-Wl,__mulle_atinit"
      "-Wl,-exported_symbol" "-Wl,_mulle_atexit"
   )

   set( CMAKE_INSTALL_RPATH
      "@loader_path/../Frameworks/"
      "@loader_path/../lib/"
   )
   install( TARGETS UnitKit DESTINATION "Frameworks")
else()
   set( CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")

   install( TARGETS UnitKit DESTINATION "lib" )
   install( FILES ${UNIT_KIT_PUBLIC_HEADERS} DESTINATION "include/UnitKit")
   install( FILES ${UNIT_KIT_RESOURCES} DESTINATION "share/UnitKit")
endif()


##
## ukrun
##

add_executable( ukrun
${UKRUN_SOURCES}
${UKRUN_PUBLIC_HEADERS}
${UKRUN_PROJECT_HEADERS}
${UKRUN_PRIVATE_HEADERS}
${UKRUN_RESOURCES}
)

target_include_directories( ukrun
   PUBLIC
Source/FrameworkSource
Source/
Source/SharedLibrarySource
)

add_dependencies( ukrun UnitKit)

target_link_libraries( ukrun
${BEGIN_ALL_LOAD}
UnitKit
${END_ALL_LOAD}
${UKRUN_DEPENDENCIES}
${UKRUN_STATIC_DEPENDENCIES}
${MULLE_SDE_LINKER_FLAGS}
)

install( TARGETS ukrun DESTINATION "bin")
