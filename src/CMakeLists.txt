cmake_minimum_required(VERSION 4.2)

# This directory only aggregates the real targets: the platform-free
# request logic (core) and the IIS module DLL that adapts to it.
add_subdirectory(core)
add_subdirectory(module)
