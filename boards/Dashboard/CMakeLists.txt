CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
include (../avr.cmake)

<<<<<<< HEAD
project (Dashboard)

set (L_FUSE 0x65) # Default: 0x62
set (H_FUSE 0xD7)

new_board(can_api)

#add_avrlibrary(CAN_avr_lib)
=======
project (Dashboard C)

set (L_FUSE 0x65) # Default: 0x62
set(F_CPU 4000000UL)

new_board(can_api)
>>>>>>> master
