#pragma once

#include "base.h"

#include <array>
#include <cassert>
#include <string>

namespace emu
{

struct Chip8;

/*
 *  Chip8 Instruction set (Table):
 *  -----------------------------
 *    -> Maintains mapping from op code to C++ function
 *
 *    - 35 different instructions (math, graphics, control)
 *    - instructions are 2 bytes long and are stored most-significant-byte first
 *
 *    References:
 *      - https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
 *      - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 *
 *  -----------------------------
 *  @author Nikolaus Rauch
 *  @date 22.12.2021
 */
struct Instruction
{
    /* OP Code Parsing Helper */
    struct OpCode
    {
        OpCode(uint16 data);

        uint16 data() const;
        uint8 cmd() const;
        uint8 n() const;
        uint8 nn() const;
        uint16 nnn() const;
        uint8 x() const;
        uint8 y() const;

    private:
        uint16 m_data;
    };

    /* Instruction Info and Code */
    struct Operation
    {
        std::string m_op_code = "0000";
        uint16 (*m_exec)(Chip8& chip8, const OpCode op_code) = [](Chip8& chip8, const OpCode op_code) -> uint16 { assert(false); return 0; };
    };


public:
    Instruction();

    /* retrieve function from operation code (called by Chip8 execute_cycle) */
    const Operation& decode(const OpCode op_code);

private:
    std::array<Operation, 35> m_table;
};

}
