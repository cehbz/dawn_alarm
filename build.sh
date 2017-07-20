#! /bin/bash
set -e
set -x

/Applications/Arduino.app/Contents/Java/arduino-builder \
  -dump-prefs \
  -logger=machine \
  -hardware /Applications/Arduino.app/Contents/Java/hardware \
  -hardware /Users/charles/Library/Arduino15/packages \
  -tools /Applications/Arduino.app/Contents/Java/tools-builder \
  -tools /Applications/Arduino.app/Contents/Java/hardware/tools/avr \
  -tools /Users/charles/Library/Arduino15/packages \
  -built-in-libraries /Applications/Arduino.app/Contents/Java/libraries \
  -libraries /Users/charles/projects/personal/Arduino/libraries \
  -fqbn=esp8266:esp8266:nodemcuv2:CpuFrequency=80,UploadSpeed=921600,FlashSize=4M3M \
  -ide-version=10802 \
  -build-path ./build \
  -warnings=more \
  -build-cache ./build/cache \
  -prefs=build.warn_data_percentage=75 \
  -prefs=runtime.tools.esptool.path=/Users/charles/Library/Arduino15/packages/esp8266/tools/esptool/0.4.9 \
  -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/Users/charles/Library/Arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2 \
  -prefs=runtime.tools.mkspiffs.path=/Users/charles/Library/Arduino15/packages/esp8266/tools/mkspiffs/0.1.2 \
  -verbose \
  /Users/charles/projects/personal/Arduino/esp8266/dawn_alarm/dawn_alarm.ino
/Applications/Arduino.app/Contents/Java/arduino-builder \
  -compile \
  -logger=machine \
  -hardware /Applications/Arduino.app/Contents/Java/hardware \
  -hardware /Users/charles/Library/Arduino15/packages \
  -tools /Applications/Arduino.app/Contents/Java/tools-builder \
  -tools /Applications/Arduino.app/Contents/Java/hardware/tools/avr \
  -tools /Users/charles/Library/Arduino15/packages \
  -built-in-libraries /Applications/Arduino.app/Contents/Java/libraries \
  -libraries  /Users/charles/projects/personal/Arduino/libraries \
  -fqbn=esp8266:esp8266:nodemcuv2:CpuFrequency=80,UploadSpeed=921600,FlashSize=4M3M \
  -ide-version=10802 \
  -build-path ./build \
  -warnings=more \
  -build-cache ./build/cache \
  -prefs=build.warn_data_percentage=75 \
  -prefs=runtime.tools.esptool.path=/Users/charles/Library/Arduino15/packages/esp8266/tools/esptool/0.4.9 \
  -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/Users/charles/Library/Arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2 \
  -prefs=runtime.tools.mkspiffs.path=/Users/charles/Library/Arduino15/packages/esp8266/tools/mkspiffs/0.1.2 \
  -verbose \
  /Users/charles/projects/personal/Arduino/esp8266/dawn_alarm/dawn_alarm.ino
