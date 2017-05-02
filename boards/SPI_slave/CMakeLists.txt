CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
include (../avr.cmake)

project (SPI_Rx C)

set (L_FUSE 0x65) # Default: 0x62

new_board(can_api)
