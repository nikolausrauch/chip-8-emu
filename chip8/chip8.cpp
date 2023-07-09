#include "chip8.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>

namespace emu
{

namespace detail
{

/* font memory (loaded in at construction) */
constexpr std::array<uint8, 80> fontset =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

}


Chip8::Chip8()
{
    /* initialize CPU state */
    m_register.PC = Chip8::start_addr;
    m_register.I = 0x0;
    m_register.SP = 0x0;
    m_register.timer_delay = 0;
    m_register.timer_sound = 0;
    m_await_interrupt = false;
    std::fill(m_register.V.begin(), m_register.V.end(), 0);

    /* initialize memory */
    std::fill(m_keypad.begin(), m_keypad.end(), 0);
    std::fill(m_display.begin(), m_display.end(), false);
    std::fill(m_memory.begin(), m_memory.end(), 0);
    std::fill(m_stack.begin(), m_stack.end(), 0);

    /* load fonts into memory */
    std::copy(detail::fontset.begin(), detail::fontset.end(), m_memory.begin());
}

bool Chip8::load_rom(const std::filesystem::path& path)
{
    if(!std::filesystem::exists(path))
    {
        std::cerr << "[Chip8::load_rom] Rom " + path.string() + " not found!" << std::endl;
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    std::vector<uint8> code(std::istreambuf_iterator<char>(file), {});
    return load_rom(code);
}

bool Chip8::load_rom(const std::vector<uint8>& code)
{
    if(code.size() > end_addr - start_addr)
    {
        std::cerr << "[Chip8::load_rom] Rom size too large!" << std::endl;
        return false;
    }

    std::copy(code.begin(), code.end(), m_memory.begin() + 0x200);
    return true;
}

void Chip8::execute_cycle()
{
    /* fetch next opcode */
    auto op_code = m_memory[m_register.PC] << 8 | m_memory[m_register.PC + 1];

    /* decode instruction and execute */
    const auto& instruction = m_instructions.decode(op_code);
    m_register.PC += instruction.m_exec(*this, op_code);
}

void Chip8::tick()
{
    for(int i = 0; i < m_settings.m_cycles; i++)
    {
        execute_cycle();
    }

    /* timer tick (currently expects that tick is called at 60Hz) */
    if(m_register.timer_delay > 0) m_register.timer_delay--;;
    if(m_register.timer_sound > 0) m_register.timer_sound--;
}

Chip8::Registers& Chip8::regs()
{
    return m_register;
}

const Chip8::Registers &Chip8::regs() const
{
    return m_register;
}

Chip8::Display& Chip8::display()
{
    return m_display;
}

const Chip8::Display& Chip8::display() const
{
    return m_display;
}

Chip8::Keypad& Chip8::keypad()
{
    return m_keypad;
}

const Chip8::Keypad& Chip8::keypad() const
{
    return m_keypad;
}

Chip8::Memory& Chip8::memory()
{
    return m_memory;
}

const Chip8::Memory& Chip8::memory() const
{
    return m_memory;
}

Chip8::Settings& Chip8::settings()
{
    return m_settings;
}

const Chip8::Settings& Chip8::settings() const
{
    return m_settings;
}

std::ostream& operator<<(std::ostream& stream, const Chip8& emu)
{
    auto& display = emu.display();
    auto& regs = emu.regs();

    stream << "+----------------------------------------------------------------+\n";
    for(int y = 0; y < emu::Chip8::height_res; y++)
    {
        stream << "|";
        for(int x = 0; x < emu::Chip8::width_res; x++)
        {
            stream << (display[x + y*Chip8::width_res] ? "#" : " ");
        }
        stream << "|\n";
    }
    stream << "+----------------------------------------------------------------+\n";

    stream << "\n+---------------------------[Register]---------------------------+\n";
    stream << " [V0]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x0];
    stream << "    [V1]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x1] << "   ";
    stream << " [V2]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x2];
    stream << "    [V3]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x3] << "\n";

    stream << " [V4]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x4];
    stream << "    [V5]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x5] << "   ";
    stream << " [V6]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x6];
    stream << "    [V7]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x7] << "\n";

    stream << " [V8]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x0];
    stream << "    [V9]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x1] << "   ";
    stream << " [V10]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x0];
    stream << "   [V11]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x1] << "\n";

    stream << " [V12]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x0];
    stream << "   [V13]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x1] << "  ";
    stream << " [V14]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x0];
    stream << "   [V15]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.V[0x1] << "\n";

    stream << " [I]: " << std::setfill('0') << std::setw(4) << std::hex << (int)regs.I << "\n";
    stream << " [PC]: " << std::setfill('0') << std::setw(4) << std::hex << (int)regs.PC << "\n";
    stream << " [SP]: " << std::setfill('0') << std::setw(4) << std::hex << (int)regs.SP << "\n";

    stream << "\n";
    stream << " [Timer delay]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.timer_delay;
    stream << "     [Timer sound]: " << std::setfill('0') << std::setw(2) << std::hex << (int)regs.timer_sound;
    stream << "\n+----------------------------------------------------------------+\n";

    return stream;
}

}
