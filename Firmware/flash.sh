cmake --build build && openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program /Users/badsheep1/Projects/crispy-journey/Firmware/build/Firmware verify reset exit"
