cmake_minimum_required(VERSION 3.22)

project(embedded_linux_program)

include_directories(inc)

set(SOURCES main.c src/peripherals/mailbox_0.c src/utils.c)

add_executable(embedded_linux_program ${SOURCES})