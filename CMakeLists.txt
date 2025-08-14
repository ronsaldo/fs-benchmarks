# UsgarRise cmake build script.
cmake_minimum_required(VERSION 3.5)
project(FSBenchmarks)

add_executable(FSBenchmarks Time.c Main.c Stdio.c Posix.c PosixExplicitOffset.c Win32FileApi.c)