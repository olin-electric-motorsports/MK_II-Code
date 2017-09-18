CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
include (../avr.cmake)

project (Throttle C)

set (L_FUSE 0x65) # Default: 0x62
set (H_FUSE 0xD7)

new_board(can_api)

#add_avrlibrary(CAN_avr_lib)
