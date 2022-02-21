project(steamlib)

file(GLOB STEAMLIB_HEADERS
    "*.h"
	"../gameshared/*.h"
)

file(GLOB STEAMLIB_SOURCES
    "*.cpp"
    "../gameshared/q_shared.c"
)

set(STEAMWORKS_SDK_DIR ${CMAKE_HOME_DIRECTORY}/../third-party/steamworks/sdk)
include_directories("${STEAMWORKS_SDK_DIR}/public")

set(STEAMWORKS_LIBRARY steam_api)
# Please note, this is not in the 'libs' directory!
set(STEAMWORKS_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/build)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(STEAMWORKS_LIBRARY_DIR ${STEAMWORKS_SDK_DIR}/redistributable_bin/win64)
        set(STEAMWORKS_LIBRARY steam_api64)
    else()
        set(STEAMWORKS_LIBRARY_DIR ${STEAMWORKS_SDK_DIR}/redistributable_bin)
    endif()
    set(STEAMWORKS_LIBRARY_EXTENSION .dll)
    # This is not a typo, but using Visual Studio's 'Configuration' property!
    set(STEAMWORKS_OUTPUT_PATH "${CMAKE_HOME_DIRECTORY}/build/$(Configuration)")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(STEAMWORKS_LIBRARY_DIR ${STEAMWORKS_SDK_DIR}/redistributable_bin/osx)
    set(STEAMWORKS_LIBRARY_EXTENSION .dylib)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(STEAMWORKS_LIBRARY_DIR ${STEAMWORKS_SDK_DIR}/redistributable_bin/linux64)
    else()
        set(STEAMWORKS_LIBRARY_DIR ${STEAMWORKS_SDK_DIR}/redistributable_bin/linux32)
    endif()
    set(STEAMWORKS_LIBRARY_EXTENSION .so)
endif()

link_directories(${STEAMWORKS_LIBRARY_DIR})

add_library(steamlib SHARED ${STEAMLIB_SOURCES} ${STEAMLIB_HEADERS})
target_link_libraries(steamlib ${STEAMWORKS_LIBRARY})
qf_set_output_dir(steamlib libs)

add_custom_command(TARGET steamlib
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${STEAMWORKS_LIBRARY_DIR}/${STEAMWORKS_LIBRARY}${STEAMWORKS_LIBRARY_EXTENSION}"
        "${STEAMWORKS_OUTPUT_PATH}"
        
)
