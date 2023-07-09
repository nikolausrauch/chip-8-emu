#include "instruction.h"

#include "chip8.h"

#include <cstring>

namespace emu
{

namespace detail
{

enum eCode : int
{
    _00E0 = 0, _00EE,
    _1NNN,_2NNN, _3XNN, _4XNN, _5XY0, _6XNN, _7XNN,
    _8XY0, _8XY1, _8XY2, _8XY3, _8XY4, _8XY5, _8XY6, _8XY7, _8XYE,
    _9XY0,
    _ANNN, _BNNN, _CXNN, _DXYN,
    _EX9E, _EXA1,
    _FX07, _FX0A, _FX15, _FX18, _FX1E, _FX29, _FX33, _FX55, _FX65,
    UNKOWN
};

/* Decode from OP Code to instruction table index */
eCode mapping(const Instruction::OpCode op_code)
{
    switch(op_code.cmd())
    {

    case 0x0:
        switch(op_code.nn())
        {
        case 0xE0: return eCode::_00E0;
        case 0xEE: return eCode::_00EE;
        default: return eCode::UNKOWN;
        }
    case 0x1: return eCode::_1NNN;
    case 0x2: return eCode::_2NNN;
    case 0x3: return eCode::_3XNN;
    case 0x4: return eCode::_4XNN;
    case 0x5: return eCode::_5XY0;
    case 0x6: return eCode::_6XNN;
    case 0x7: return eCode::_7XNN;
    case 0x8:
        switch(op_code.n())
        {
        case 0x0: return eCode::_8XY0;
        case 0x1: return eCode::_8XY1;
        case 0x2: return eCode::_8XY2;
        case 0x3: return eCode::_8XY3;
        case 0x4: return eCode::_8XY4;
        case 0x5: return eCode::_8XY5;
        case 0x6: return eCode::_8XY6;
        case 0x7: return eCode::_8XY7;
        case 0xE: return eCode::_8XYE;
        default: return eCode::UNKOWN;
        }
    case 0x9: return eCode::_9XY0;
    case 0xA: return eCode::_ANNN;
    case 0xB: return eCode::_BNNN;
    case 0xC: return eCode::_CXNN;
    case 0xD: return eCode::_DXYN;
    case 0xE:
        switch(op_code.nn())
        {
        case 0x9E: return eCode::_EX9E;
        case 0xA1: return eCode::_EXA1;
        default: return eCode::UNKOWN;
        }
    case 0xF:
        switch(op_code.nn())
        {
        case 0x07: return eCode::_FX07;
        case 0x0A: return eCode::_FX0A;
        case 0x15: return eCode::_FX15;
        case 0x18: return eCode::_FX18;
        case 0x1E: return eCode::_FX1E;
        case 0x29: return eCode::_FX29;
        case 0x33: return eCode::_FX33;
        case 0x55: return eCode::_FX55;
        case 0x65: return eCode::_FX65;
        }
    default: return eCode::UNKOWN;
    }
}

}


Instruction::OpCode::OpCode(uint16 data)
    : m_data(data)
{

}

uint16 Instruction::OpCode::data() const
{
    return m_data;
}

uint8 Instruction::OpCode::cmd() const
{
    return (m_data & 0xF000) >> 12;
}

uint8 Instruction::OpCode::n() const
{
    return (m_data & 0x000F);
}

uint8 Instruction::OpCode::nn() const
{
    return (m_data & 0x00FF);
}

uint16 Instruction::OpCode::nnn() const
{
    return (m_data & 0x0FFF);
}

uint8 Instruction::OpCode::x() const
{
    return (m_data & 0x0F00) >> 8;
}

uint8 Instruction::OpCode::y() const
{
    return (m_data & 0x00F0) >> 4;
}


Instruction::Instruction()
{
    m_table[detail::eCode::_00E0] =
    {
        "00E0",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            std::memset(chip8.m_display.data(), false, Chip8::width_res*Chip8::height_res);
            return 2;
        }
    };

    m_table[detail::eCode::_00EE] =
    {
        "00EE",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.SP--;
            chip8.m_register.PC = chip8.m_stack[chip8.m_register.SP];

            return 2;
        }
    };

    m_table[detail::eCode::_1NNN] =
    {
        "1NNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.PC = op_code.nnn();
            return 0;
        }
    };

    m_table[detail::eCode::_2NNN] =
    {
        "2NNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_stack[chip8.m_register.SP] = chip8.m_register.PC;
            chip8.m_register.SP++;
            chip8.m_register.PC = op_code.nnn();
            return 0;
        }
    };

    m_table[detail::eCode::_3XNN] =
    {
        "3XNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            return 2 + 2 * (chip8.m_register.V[op_code.x()] == op_code.nn());
        }
    };

    m_table[detail::eCode::_4XNN] =
    {
        "4XNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            return 2 + 2 * (chip8.m_register.V[op_code.x()] != op_code.nn());
        }
    };

    m_table[detail::eCode::_5XY0] =
    {
        "5XY0",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            return 2 + 2 * (chip8.m_register.V[op_code.x()] == chip8.m_register.V[op_code.y()]);
        }
    };

    m_table[detail::eCode::_6XNN] =
    {
        "6XNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] = op_code.nn();
            return 2;
        }
    };

    m_table[detail::eCode::_7XNN] =
    {
        "7XNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] += op_code.nn();
            return 2;
        }
    };

    m_table[detail::eCode::_8XY0] =
    {
        "8XY0",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] = chip8.m_register.V[op_code.y()];
            return 2;
        }
    };

    m_table[detail::eCode::_8XY1] =
    {
        "8XY1",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] |= chip8.m_register.V[op_code.y()];
            if(chip8.m_settings.m_vf_reset) chip8.m_register.V[0xF] = 0;
            return 2;
        }
    };

    m_table[detail::eCode::_8XY2] =
    {
        "8XY2",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] &= chip8.m_register.V[op_code.y()];
            if(chip8.m_settings.m_vf_reset) chip8.m_register.V[0xF] = 0;
            return 2;
        }
    };

    m_table[detail::eCode::_8XY3] =
    {
        "8XY3",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] ^= chip8.m_register.V[op_code.y()];
            if(chip8.m_settings.m_vf_reset) chip8.m_register.V[0xF] = 0;
            return 2;
        }
    };

    m_table[detail::eCode::_8XY4] =
    {
        "8XY4",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            auto& vx = chip8.m_register.V[op_code.x()];
            auto& vy = chip8.m_register.V[op_code.y()];
            chip8.m_register.V[0xF] = vy > (0xFF - vx);

            vx += vy;
            return 2;
        }
    };

    m_table[detail::eCode::_8XY5] =
    {
        "8XY5",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            auto& vx = chip8.m_register.V[op_code.x()];
            auto& vy = chip8.m_register.V[op_code.y()];
            chip8.m_register.V[0xF] = !(vy >= vx);

            vx -= vy;
            return 2;
        }
    };

    m_table[detail::eCode::_8XY6] =
    {
        "8XY6",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            if(chip8.m_settings.m_shifting)
            {
                auto& vx = chip8.m_register.V[op_code.x()];
                chip8.m_register.V[0xF] = vx & 0x1;
                vx >>= 1;
            }
            else
            {
                auto& vx = chip8.m_register.V[op_code.x()];
                auto& vy = chip8.m_register.V[op_code.y()];
                chip8.m_register.V[0xF] = vy & 0x1;
                vx = vy >> 1;
            }

            return 2;
        }
    };

    m_table[detail::eCode::_8XY7] =
    {
        "8XY7",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            auto& vx = chip8.m_register.V[op_code.x()];
            auto& vy = chip8.m_register.V[op_code.y()];
            chip8.m_register.V[0xF] = vx <= vy;

            vx = vy - vx;
            return 2;
        }
    };

    m_table[detail::eCode::_8XYE] =
    {
        "8XYE",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            if(chip8.m_settings.m_shifting)
            {
                auto& vx = chip8.m_register.V[op_code.x()];
                chip8.m_register.V[0xF] = vx >> 7;
                vx <<= 1;
            }
            else
            {
                auto& vx = chip8.m_register.V[op_code.x()];
                auto& vy = chip8.m_register.V[op_code.y()];
                chip8.m_register.V[0xF] = vy >> 7;
                vx = vy << 1;
            }

            return 2;
        }
    };

    m_table[detail::eCode::_9XY0] =
    {
        "9XY0",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            return 2 + 2 * (chip8.m_register.V[op_code.x()] != chip8.m_register.V[op_code.y()]);
        }
    };

    m_table[detail::eCode::_ANNN] =
    {
        "ANNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.I = op_code.nnn();
            return 2;
        }
    };

    m_table[detail::eCode::_BNNN] =
    {
        "BNNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            if(chip8.m_settings.m_jumping) chip8.m_register.PC = chip8.m_register.V[op_code.x()] + op_code.nnn();
            else chip8.m_register.PC = chip8.m_register.V[0] + op_code.nnn();
            return 0;
        }
    };

    m_table[detail::eCode::_CXNN] =
    {
        "CXNN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] = (rand() % 0xFF) & op_code.nn();
            return 2;
        }
    };

    m_table[detail::eCode::_DXYN] =
    {
        "DXYN",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            const auto vx = chip8.m_register.V[op_code.x()] % 64;
            const auto vy = chip8.m_register.V[op_code.y()] % 32;
            const auto I = chip8.m_register.I;
            const auto height = op_code.n();

            chip8.m_register.V[0xF] = 0;

            for(int y = 0; y < height; y++)
            {
                auto pixel = chip8.m_memory[I + y];
                for(int x = 0; x < 8; x++)
                {
                    bool draw = (pixel & (0x80 >> x)) != 0;
                    auto index = vx + x + (vy + y) * Chip8::width_res;

                    chip8.m_register.V[0xF] = chip8.m_register.V[0xF] || (chip8.m_display[index] && draw);
                    chip8.m_display[index] ^= draw;
                }
            }

            return 2;
        }
    };

    m_table[detail::eCode::_EX9E] =
    {
        "EX9E",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            return 2 + 2 * chip8.m_keypad[ chip8.m_register.V[op_code.x()] ];
        }
    };

    m_table[detail::eCode::_EXA1] =
    {
        "EXA1",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            return 2 + 2 * !chip8.m_keypad[ chip8.m_register.V[op_code.x()] ];
        }
    };

    m_table[detail::eCode::_FX07] =
    {
        "FX07",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.V[op_code.x()] = chip8.m_register.timer_delay;
            return 2;
        }
    };

    m_table[detail::eCode::_FX0A] =
    {
        "FX0A",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_await_interrupt = true;

            for(unsigned int i = 0; i < chip8.m_keypad.size(); i++)
            {
                if(chip8.m_keypad[i])
                {
                    chip8.m_register.V[op_code.x()] = i;
                    chip8.m_await_interrupt = false;
                }
            }

            return chip8.m_await_interrupt ? 0 : 2;
        }
    };

    m_table[detail::eCode::_FX15] =
    {
        "FX15",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.timer_delay = chip8.m_register.V[op_code.x()];
            return 2;
        }
    };

    m_table[detail::eCode::_FX18] =
    {
        "FX18",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.timer_sound = chip8.m_register.V[op_code.x()];
            return 2;
        }
    };

    m_table[detail::eCode::_FX1E] =
    {
        "FX1E",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            const auto vx = chip8.m_register.V[op_code.x()];
            auto& I = chip8.m_register.I;
            //chip8.m_register.V[0xF] = (I + vx) > 0xFFFF;

            I += vx;
            return 2;
        }
    };

    m_table[detail::eCode::_FX29] =
    {
        "FX29",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            chip8.m_register.I = chip8.m_register.V[op_code.x()] * 0x5;
            return 2;
        }
    };

    m_table[detail::eCode::_FX33] =
    {
        "FX33",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            const auto vx = chip8.m_register.V[ op_code.x() ];

            chip8.m_memory[ chip8.m_register.I + 0 ] = vx / 100;
            chip8.m_memory[ chip8.m_register.I + 1 ] = (vx / 10) % 10;
            chip8.m_memory[ chip8.m_register.I + 2 ] = (vx % 100) % 10;

            return 2;
        }
    };

    m_table[detail::eCode::_FX55] =
    {
        "FX55",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            const auto x = op_code.x();

            for(int i = 0; i <= x; i++)
            {
                chip8.m_memory[ chip8.m_register.I + i ] = chip8.m_register.V[i];
            }

            if(chip8.m_settings.m_memory) chip8.m_register.I += x + 1;

            return 2;
        }
    };

    m_table[detail::eCode::_FX65] =
    {
        "FX65",
        [](Chip8& chip8, const OpCode op_code) -> uint16
        {
            const auto x = op_code.x();

            for(int i = 0; i <= x; i++)
            {
                chip8.m_register.V[i] = chip8.m_memory[ chip8.m_register.I + i ];
            }

            if(chip8.m_settings.m_memory) chip8.m_register.I += x + 1;

            return 2;
        }
    };
}

const Instruction::Operation& Instruction::decode(const OpCode op_code)
{
    return m_table[ detail::mapping(op_code) ];
}

}

