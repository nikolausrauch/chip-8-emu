#include "viewer/viewer.h"

#include <cmath>
#include <iostream>
#include <cstdlib>

/*
 * Chip 8 emulation program:
 * ---------------------------
 * arguments:
 *      chip-8-emu " << "<path> [--quirks jmsr] [--speed 500]
 *
 *      <path>: filepath to rom
 *
 *      --quirks: optional quirks
 *               j : jumping
 *               m : memory
 *               s : shifting
 *               r : vf reset
 *
 *      --speed: optional speed in hz (default 500hz)
 */
int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "[chip-8-emu] Missing rom file." << std::endl;
        std::cerr << "             Usage: " << "chip-8-emu " << "<path> [--quirks jmsr] [--speed 500]" << std::endl;
        return EXIT_FAILURE;
    }

    /* sfml viewer and chip8 emulator */
    Viewer viewer;
    auto& emulator = viewer.emulator();

    /* emulation default settings */
    auto& settings = emulator.settings();
    settings.m_cycles = static_cast<int>(std::ceil(500 / 60.0f));
    settings.m_jumping = false;
    settings.m_memory = false;
    settings.m_shifting = false;
    settings.m_vf_reset = false;

    /* load rom */
    if(!emulator.load_rom(argv[1]))
    {
        std::cerr << "[chip-8-emu] Couldn't load rom file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    /* parse options */
    for(int i = 2; i < argc; i++)
    {
        std::string arg(argv[i]);

        if(arg == "--quirks" && argc >= i + 1)
        {
            std::string options(argv[i+1]);
            for(char o : options)
            {
                switch (o)
                {
                    case 'j': settings.m_jumping = true; break;
                    case 'm': settings.m_memory = true; break;
                    case 's': settings.m_shifting = true; break;
                    case 'r': settings.m_vf_reset = true; break;
                default: break;
                }
            }

            i++;
        }

        if(arg == "--speed" && argc >= i + 1)
        {
            int speed = std::abs(std::atoi(argv[i+1]));
            settings.m_cycles = static_cast<int>(std::ceil(speed / 60.0f));

            i++;
        }
    }

    /* start emulation */
    viewer.run();

    return EXIT_SUCCESS;
}
