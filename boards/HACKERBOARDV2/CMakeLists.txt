CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
include (../avr.cmake)

project (Hackerboard_V2 C)

#set (L_FUSE 0x65) # Default: 0x62
#set (L_FUSE 0x3F) # Default: 0x62
set (L_FUSE 0x65) # Default: 0x62

#define F_CPU(4000000L)

new_board(can_api)
