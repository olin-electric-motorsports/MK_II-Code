find_program (AVR_CC avr-gcc)
find_program (AVR_OBJCOPY avr-objcopy)
find_program (AVRDUDE avrdude)
set (PROGRAMMER       avrispmkII)
set (PORT             usb)
set (MCU              atmega16m1)

set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_PROCESSOR avr)

macro (new_board)
    include_directories ("${PROJECT_SOURCE_DIR}/../../lib/")
    set (elf_file ${CMAKE_PROJECT_NAME}.elf)
    set (hex_file ${CMAKE_PROJECT_NAME}.hex)
    set (map_file ${CMAKE_PROJECT_NAME}.map)

    file(GLOB srcs *.c)

    # Add the necessary source files for all libraries required
    foreach (avr_lib ${ARGN})
        set (srcs ${srcs} ${PROJECT_SOURCE_DIR}/../../lib/${avr_lib}.c ${PROJECT_SOURCE_DIR}/../../lib/${avr_lib}.h)
    endforeach (avr_lib ${ARGN})

    message( STATUS "Sources: ${srcs}" )


    add_executable (${CMAKE_PROJECT_NAME} ${srcs})

    #add_executable (${CMAKE_PROJECT_NAME} main.c)
    set (CMAKE_C_FLAGS "-mmcu=${MCU} -g -Os -Wall -Wunused -Wl,-Map=${map_file} -lm")
    set_target_properties (${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${elf_file})

    add_custom_target (
        hex 
        ALL ${AVR_OBJCOPY} -R .eeprom -O ihex ${elf_file} ${hex_file} 
        DEPENDS ${CMAKE_PROJECT_NAME}
        )

    add_custom_target (
        flash
        sudo ${AVRDUDE} -p ${MCU} -v -c ${PROGRAMMER} -P ${PORT} -U flash:w:"${CMAKE_PROJECT_NAME}.hex"
        DEPENDS hex
        COMMENT "Flashing ${CMAKE_PROJECT_NAME}.hex"
        )

    add_custom_target (
        get_fuses
        sudo ${AVRDUDE} -p ${MCU} -c ${PROGRAMMER} -P ${PORT} -n
            -U lfuse:r:-:b
            -U hfuse:r:-:b
            COMMENT "Get fuses from ${MCU}"
        )
    
    if( DEFINED L_FUSE )
        add_custom_target (
            set_fuses
            sudo ${AVRDUDE} -p ${MCU} -c ${PROGRAMMER} -P ${PORT}
                -U lfuse:w:${L_FUSE}:m
                COMMENT "Setup:\n Low Fuse: ${L_FUSE}"
            )
    endif( DEFINED L_FUSE )

endmacro (new_board)

