CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

##################
# Config Section #
##################

find_program(AVR_CC avr-gcc)
find_program (AVR_OBJCOPY avr-objcopy)
find_program (AVRDUDE avrdude)
set (PROGRAMMER       avrispmkII)
set (PORT             usb)
set (MCU              atmega16m1)

set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_PROCESSOR avr)
set (CMAKE_C_COMPILER ${AVR_CC})


#############
# Functions #
#############
function(new_board PROJ)

    project (${PROJ} C)
    set (elf_file ${PROJ}.elf)
    set (hex_file ${PROJ}.hex)
    set (map_file ${PROJ}.map)

    #add_executable (${PROJ} main.c)
    add_executable (${elf_file} main.c)
    set (CMAKE_C_FLAGS "-mmcu=${MCU} -g -Os -Wall -Wunused -Wl,-Map=${PROJ}.map -lm")
    #set_target_properties (${PROJ} PROPERTIES OUTPUT_NAME "${PROJ}.elf")
    set_target_properties (${elf_file} PROPERTIES OUTPUT_NAME ${elf_file})

    add_custom_target (strip_${PROJ} ALL avr-strip ${elf_file} DEPENDS ${PROJ})
    add_custom_target (hex_${PROJ} ALL ${AVR_OBJCOPY} -R .eeprom -O ihex ${elf_file} ${hex_file} 
        DEPENDS strip_${PROJ})

#    set (elf_file ${PROJECT_NAME}.elf)
#    set (hex_file ${PROJECT_NAME}.hex)
#    set (map_file ${PROJECT_NAME}.map)
#
#    add_executable (${PROJECT_NAME} main.c)
#    set (CMAKE_C_FLAGS "-mmcu=${MCU} -g -Os -Wall -Wunused -Wl,-Map=${PROJECT_NAME}.map -lm")
#    set_target_properties (${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
#
#    add_custom_target (hex ALL ${AVR_OBJCOPY} -R .eeprom -O ihex ${elf_file} ${hex_file} 
#        DEPENDS ${PROJECT_NAME})
#
#    message (${PROJECT_NAME})

#    set(CFLAGS "-mmcu=${MCU} -g -Os -Wall -Wunused")
#    set(ADDITIONAL_FLAGS "-std=gnu99 -DF_CPU=1000000 -lm")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} ${ADDITIONAL_FLAGS}")
#    set(LDFLAGS "-mmcu=${MCU} -Wl,-Map=${PROJ}.map -lm")
#    set(AVRFLAGS "-p ${MCU} -v -c ${PROGRAMMER} -P ${PORT}")

#    include_directories("${PROJECT_SOURCE_DIR}")
#    include_directories("${CMAKE_SOURCE_DIR}/lib")
#    add_executable (${PROJ} main.c)
#    set_target_properties(${PROJ} PROPERTIES OUTPUT_NAME "${PROJ}.elf")
#
#    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJ}.hex;${PROJ}.map")

#    file (GLOB files "*.c")
#    foreach (f ${files})
#        if (NOT ${f} MATCHES "main.c")
#            add_library (LocalLibs ${f})
#        endif(NOT ${f} MATCHES "main.c")
#    endforeach()
#
#    target_link_libraries (${PROJ} LocalLibs)

#    add_custom_target (
#        elf_${PROJ} ALL
#        ${AVRC} ${PROJ} ${LDFLAGS} -o "${PROJ}.elf"
#        DEPENDS ${PROJ}
#        COMMENT "Compiling ${PROJ}.elf"
#        )
#
#    add_custom_target (
#        hex_${PROJ} ALL
#        ${OBJCOPY} -O ihex -R .eeprom "${PROJ}.elf" "${PROJ}.hex"
#        DEPENDS elf_${PROJ}
#        COMMENT "Compiling ${PROJ}.hex"
#        )
#
#    add_custom_target (
#        flash_${PROJ}
#        sudo ${AVRDUDE} ${AVRFLAGS} -U flash:w:"${PROJ}.hex"
#        DEPENDS hex_${PROJ}
#        COMMENT "Flashing ${PROJ}.hex"
#        )
endfunction(new_board)

##################
# Subdirectories #
##################
#project (MKII) # Not really a project...

# file(GLOB SOURCEDIRS "boards/*")
# foreach (dir ${SOURCEDIRS} )
#     add_subdirectory (${dir})
# endforeach()

add_subdirectory (boards/Blinky)

