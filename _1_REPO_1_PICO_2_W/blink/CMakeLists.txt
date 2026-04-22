# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PICO_2_W_TEST C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

set(PROJ_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SRC_DIR ${PROJ_DIR}/src)
set(BSP_DIR ${PROJ_DIR}/src/bsp)
set(AP_DIR ${PROJ_DIR}/src/ap)
set(PIO_DIR ${PROJ_DIR}/src/pio)
set(GATT_DIR ${PROJ_DIR}/src/gatt)

add_executable(PICO_2_W_TEST)

target_sources(PICO_2_W_TEST PRIVATE
               ${SRC_DIR}/main.c
               ${AP_DIR}/ap_task.c
               ${AP_DIR}/ap_test.c
               ${AP_DIR}/ap_bt_standalone_server.c
               ${AP_DIR}/ap.c
               ${BSP_DIR}/bsp.c)

target_include_directories(PICO_2_W_TEST PRIVATE
    ${SRC_DIR}
    ${SRC_DIR}/ap
    ${SRC_DIR}/bsp
)

# pull in common dependencies
target_link_libraries(PICO_2_W_TEST 
                      pico_stdlib
                      hardware_adc)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(PICO_2_W_TEST 
                          pico_cyw43_arch_none
                          pico_btstack_ble
                          pico_btstack_cyw43)

pico_btstack_make_gatt_header(PICO_2_W_TEST PRIVATE 
                             ${GATT_DIR}/temp_sensor.gatt)

endif()

# create map/bin/hex file etc.
pico_add_extra_outputs(PICO_2_W_TEST)

# add url via pico_set_program_url
