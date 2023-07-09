#pragma once

#include <chip8/chip8.h>

#include <string>
#include <functional>

#include <SFML/Graphics.hpp>

/*
 * Emulator Viewer
 *  -> minimalistic viewer of chip8 display and input handling
 *  -> access Chip8 with auto& emu = emulator(); to load rom and change settings
 *
 *  Resolution:
 *  ---------------------------------
 *    chip 8: 64 x 32
 *    window: chip 8 * 16px = 1024 x 512
 *
 *  Input-Mapping:
 *  ---------------------------------
 *        chip8     |    keyboard
 *  ---------------------------------
 *  [1] [2] [3] [C] | [1] [2] [3] [C]
 *  [4] [5] [6] [D] | [Q] [W] [E] [R]
 *  [7] [8] [9] [E] | [A] [S] [D] [F]
 *  [A] [0] [B] [F] | [Y] [X] [C] [V]
 *  ---------------------------------
 *   ESC: close application
 *   PAGE_UP: increase resolution
 *   PAGE_DOWN: decrease resolution
 *   +: increase speed
 *   -: decrease speed
 *   space: print display and registers of chip8 emu to stdout
 *   ctrl + J: jump quirk
 *   ctrl + M: memory quirk
 *   ctrl + S: shifting quirk
 *   ctrl + R: VF reset quirk
 *
 *
 *  -----------------------------
 *  @author Nikolaus Rauch
 *  @date 27.12.2021
 */
namespace detail { struct CBWindow; }
class Viewer
{
public:
    Viewer();

    /* start emulation (settings and rom have to be loaded beforehand) */
    void run();

    /* access chip8 emulator */
    emu::Chip8& emulator();

private:
    struct
    {
        double accumTime;
        double fps;
        unsigned int frames;
    } m_fps_counter;

    struct
    {
        int m_scale = 16;
    } m_render;

private:
    emu::Chip8 m_emulator;

    sf::Image m_display_image;
    sf::Texture m_display_texture;
    sf::Sprite m_display_sprite;

    friend struct detail::CBWindow;
};

