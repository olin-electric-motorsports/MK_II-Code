CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
include (../avr.cmake)

project (Cooling C)

set (L_FUSE 0x65) # Default: 0x62
set (H_FUSE 0xD7)

new_board()
