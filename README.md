![](https://github.com/nikolausrauch/chip-8-emu/assets/13553309/4fe25169-64b9-4145-98b8-7ea1dd25ca91)
___
This project represents my initial venture into the world of emulation.   
Emulation communities often recommend starting with a Chip-8 interpreter as a way to grasp the fundamental concepts of emulation. 
Taking their advice, I started this project to get familiar with the basics of emulation.
I am not entirely satisfied with all of my design choices, but maybe the code can serve as a source of inspiration for others.

## What is Chip-8?
Chip-8 is a virtual machine designed to run games and applications on computers of its time. It was originally implemented on the COSMAC VIP and Telmac 1800 microcomputers. Despite its simplicity, Chip-8 enjoyed popularity due to its ease of programming and availability on various platforms.

The Chip-8 system used a 4KB memory, a 64x32-pixel monochrome display, and a 16-key hexadecimal keypad. Its instruction set consisted of 35 different opcodes, providing capabilities for graphics, input handling, and simple sound generation.
___

## About the Emulator
The interpreter is written in C++ and uses SFML for rendering and input handling.
To start the program you need to provide the path to a chip-8 rom file; and optionally set emulation quirks and speed.
```
$ chip-8-emu rom/example_rom.ch8 --quirks jmsr --speed 1000
```

![](https://github.com/nikolausrauch/chip-8-emu/assets/13553309/6c4d506f-006a-4e4c-b435-91ee5d301778)



Key | Function
--- | ---
`PageUp` | *Increase* resolution by 2x
`PageDown` | *Decrease* resolution by 1/2
`+` | *Increase* emulation speed
`-` | *Decrease* emulation speed
`ctrl + J` | jump quirk
`ctrl + M` | memory quirk
`ctrl + S` | shifting quirk
`ctrl + R` | VF register reset
`space` | print chip-8 display and registers to stdout

## :books: Useful Resources

[https://en.wikipedia.org/wiki/CHIP-8#Opcode_table](https://en.wikipedia.org/wiki/CHIP-8#Opcode_table)  
[http://devernay.free.fr/hacks/chip8/C8TECH10.HTM](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)   
[https://chip-8.github.io/links/](https://chip-8.github.io/links)    
[https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908](https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908)   
[https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference)   
[https://austinmorlan.com/posts/chip8_emulator/](https://austinmorlan.com/posts/chip8_emulator/)   
[https://tobiasvl.github.io/blog/write-a-chip-8-emulator/](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)  
[https://github.com/corax89/chip8-test-rom](https://github.com/corax89/chip8-test-rom)   
[https://johnearnest.github.io/chip8Archive/](https://johnearnest.github.io/chip8Archive)    
