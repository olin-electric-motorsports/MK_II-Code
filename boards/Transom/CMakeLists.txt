CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
include (../avr.cmake)

set(L_FUSE 0x65) #Default:0x62
set(H_FUSE 0xD7)

project (Transom C)

new_board(transom)
