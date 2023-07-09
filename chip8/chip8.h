#pragma once

#include "base.h"
#include "instruction.h"

#include <array>
#include <filesystem>
#include <ostream>
#include <vector>

namespace emu
{

/*
 *  Chip8 Interpreter
 *  -----------------------------
 *  Technical references:
 *      - https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
 *      - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 *
 *
 *  Memory Map: (from http://devernay.free.fr/hacks/chip8/C8TECH10.HTM):
 *  -----------------------------
 *
 *  +---------------+= 0xFFF (4095) End of Chip-8 RAM
 *  |               |
 *  |               |
 *  |               |
 *  |               |
 *  |               |
 *  | 0x200 to 0xFFF|
 *  |     Chip-8    |
 *  | Program / Data|
 *  |     Space     |
 *  |               |
 *  |               |
 *  |               |
 *  +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
 *  |               |
 *  |               |
 *  |               |
 *  +---------------+= 0x200 (512) Start of most Chip-8 programs
 *  | 0x000 to 0x1FF|
 *  | Reserved for  |
 *  |  interpreter  |
 *  +---------------+= 0x000 (0) Start of Chip-8 RAM
 *
 *
 *  Registers:
 *  -----------------------------
 *   - 16 general purpose 8-bit registers
 *   - one 16-bit register (usually memory addresses)
 *   - two special registers for delay and sound timer
 *
 *   - implicit registers for program counter (16-bit) and stack pointer (16-bit)
 *
 *
 *  Keypad:
 * -----------------------
 *    [1] [2] [3] [C]
 *    [4] [5] [6] [D]
 *    [7] [8] [9] [E]
 *    [A] [0] [B] [F]
 *
 *
 *  Display:
 *  -----------------------------
 *  64x32 monochrome display:
 *  +--------------------------------+
 *  | (0, 0)                 (63, 0) |
 *  |                                |
 *  |                                |
 *  |                                |
 *  |                                |
 *  |                                |
 *  | (0, 31)               (63, 31) |
 *  +--------------------------------+
 *  Chip-8 uses sprites to draw graphics on the screen; which have a size up to 15 bytes, resulting in a potential size of 8x15 pixels.
 *  The program can also utilize a set of sprites representing the hexadecimal digits 0 to F.
 *  These sprites consist of 5 bytes each and have dimensions of 8x5 pixels.
 *  Is stored in memory from 0x000 to 0x1FF.
 *
 *
 *
 *  Instructions:
 *  -----------------------------
 *    - 35 different instructions (math, graphics, control)
 *    - instructions are 2 bytes long and are stored most-significant-byte first
 *    - reference https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
 *
 *  -----------------------------
 *  @author Nikolaus Rauch
 *  @date 22.12.2021
 */
struct Chip8
{
    enum eKey
    {
        KEY_0 = 0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
        COUNT
    };

    /* memory pointer */
    static constexpr uint16 font_addr = 0x50;
    static constexpr uint16 start_addr = 0x200;
    static constexpr uint16 end_addr = 0xE8F;
    static constexpr uint16 width_res = 64;
    static constexpr uint16 height_res = 32;
    static constexpr uint16 memory_size = 0x1000;

    /* Hardware Components */
    using Display = std::array<bool, width_res * height_res>;
    using Keypad = std::array<bool, eKey::COUNT>;
    using Memory = std::array<uint8, memory_size>;

    struct Registers
    {
        std::array<uint8, 16> V;
        uint16 I;
        uint16 PC;
        uint16 SP;

        uint8 timer_delay;
        uint8 timer_sound;
    };

    struct Settings
    {
        bool m_vf_reset = false;
        bool m_memory = false;
        bool m_shifting = false;
        bool m_jumping = false;

        int m_cycles = 20;
    };


public:
    Chip8();

    /* load rom from file */
    bool load_rom(const std::filesystem::path& path);

    /* load rom from memory */
    bool load_rom(const std::vector<uint8>& code);

    /* executes a single instruction */
    void execute_cycle();

    /* should be called at 60hz (runs a hardcoded number of instructions -> m_cycles) */
    void tick();

    /* access internal data */
    Registers& regs();
    const Registers& regs() const;
    Display& display();
    const Display& display() const;
    Keypad& keypad();
    const Keypad& keypad() const;
    Memory& memory();
    const Memory& memory() const;
    Settings& settings();
    const Settings& settings() const;


private:
    Settings m_settings;
    Registers m_register;
    Display m_display;
    Keypad m_keypad;
    Memory m_memory;
    std::array<uint16, 16> m_stack;
    bool m_await_interrupt;

    Instruction m_instructions;

    friend struct Instruction;
};

std::ostream& operator<< (std::ostream& stream, const emu::Chip8& emu);

}
