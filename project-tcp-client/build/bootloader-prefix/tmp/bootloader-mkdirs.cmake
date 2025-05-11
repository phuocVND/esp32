# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/phuocvnd/esp/v5.4.1/esp-idf/components/bootloader/subproject"
  "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader"
  "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix"
  "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix/tmp"
  "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix/src/bootloader-stamp"
  "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix/src"
  "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/phuocvnd/Desktop/esp32/project-tcp-client/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
