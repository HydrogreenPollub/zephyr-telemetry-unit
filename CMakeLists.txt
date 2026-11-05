cmake_minimum_required(VERSION 3.20.0)

if(NOT DEFINED BOARD)
    set(BOARD telemetry_unit CACHE STRING "Zephyr target board")
endif()
if(NOT DEFINED BOARD_ROOT)
    set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Root directory for custom boards")
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(telemetry_unit LANGUAGES C)

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE src/main.c src/telemetry_can.c)

if(CONFIG_BOOTLOADER_MCUBOOT)
    set(SIGNED_BIN  ${CMAKE_BINARY_DIR}/zephyr/zephyr.signed.bin)
    set(ELF_FILE    ${CMAKE_BINARY_DIR}/zephyr/zephyr.elf)
    set(HEADER_BIN  ${CMAKE_BINARY_DIR}/zephyr/mcuboot_header.bin)

    add_custom_target(patch_elf_mcuboot_header ALL
        COMMAND ${PYTHON_EXECUTABLE} -c
            "h=open('${SIGNED_BIN}','rb').read(${CONFIG_ROM_START_OFFSET}); open('${HEADER_BIN}','wb').write(h)"
        COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/patch_elf_mcuboot_header.py
            ${ELF_FILE} ${HEADER_BIN} ${CONFIG_ROM_START_OFFSET}
        COMMENT "Patching MCUboot image header into zephyr.elf for GDB"
        VERBATIM
    )
    add_dependencies(patch_elf_mcuboot_header zephyr_final)
endif()
