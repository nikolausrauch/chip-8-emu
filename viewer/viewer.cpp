#include "viewer.h"

#include "window.h"

#include <cassert>
#include <iostream>

namespace detail
{

struct CBWindow : public Window
{
    CBWindow(Viewer& viewer, const std::string& title, const VideoMode& mode, uint32_t style = eWindowStyle::BASIC, const WindowHints& hints = WindowHints(60, false), const ContextSettings& settings = ContextSettings())
        : Window(title, mode, style, hints, settings), m_viewer(viewer)
    {

    }

    void onKey(int key, bool ctrl, bool press) override
    {
        auto& keypad = m_viewer.emulator().keypad();

        if(!ctrl)
        {
            if(key == sf::Keyboard::Key::Num1) keypad[emu::Chip8::KEY_1] = press;
            if(key == sf::Keyboard::Key::Num2) keypad[emu::Chip8::KEY_2] = press;
            if(key == sf::Keyboard::Key::Num3) keypad[emu::Chip8::KEY_3] = press;
            if(key == sf::Keyboard::Key::Num4) keypad[emu::Chip8::KEY_C] = press;

            if(key == sf::Keyboard::Key::Q) keypad[emu::Chip8::KEY_4] = press;
            if(key == sf::Keyboard::Key::W) keypad[emu::Chip8::KEY_5] = press;
            if(key == sf::Keyboard::Key::E) keypad[emu::Chip8::KEY_6] = press;
            if(key == sf::Keyboard::Key::R) keypad[emu::Chip8::KEY_D] = press;

            if(key == sf::Keyboard::Key::A) keypad[emu::Chip8::KEY_7] = press;
            if(key == sf::Keyboard::Key::S) keypad[emu::Chip8::KEY_8] = press;
            if(key == sf::Keyboard::Key::D) keypad[emu::Chip8::KEY_9] = press;
            if(key == sf::Keyboard::Key::F) keypad[emu::Chip8::KEY_E] = press;

            if(key == sf::Keyboard::Key::Y || key == sf::Keyboard::Key::Z) keypad[emu::Chip8::KEY_A] = press;
            if(key == sf::Keyboard::Key::X) keypad[emu::Chip8::KEY_0] = press;
            if(key == sf::Keyboard::Key::C) keypad[emu::Chip8::KEY_B] = press;
            if(key == sf::Keyboard::Key::V) keypad[emu::Chip8::KEY_F] = press;
        }

        /***** emulator controls *****/
        if(key == sf::Keyboard::Key::Escape && press)
        {
            close();
        }

        if(key == sf::Keyboard::Key::PageUp && press)
        {
            m_viewer.m_render.m_scale *= 2;
            m_viewer.m_render.m_scale = std::min(m_viewer.m_render.m_scale, 64);
            m_viewer.m_display_sprite.setScale(m_viewer.m_render.m_scale, m_viewer.m_render.m_scale);
            size(emu::Chip8::width_res * m_viewer.m_render.m_scale, emu::Chip8::height_res * m_viewer.m_render.m_scale);

            sf::FloatRect visibleArea(0, 0, emu::Chip8::width_res * m_viewer.m_render.m_scale, emu::Chip8::height_res * m_viewer.m_render.m_scale);
            size(visibleArea.width, visibleArea.height);
            renderWindow().setView(sf::View(visibleArea));
        }

        if(key == sf::Keyboard::Key::PageDown && press)
        {
            m_viewer.m_render.m_scale /= 2;
            m_viewer.m_render.m_scale = std::max(m_viewer.m_render.m_scale, 4);
            m_viewer.m_display_sprite.setScale(m_viewer.m_render.m_scale, m_viewer.m_render.m_scale);

            sf::FloatRect visibleArea(0, 0, emu::Chip8::width_res * m_viewer.m_render.m_scale, emu::Chip8::height_res * m_viewer.m_render.m_scale);
            size(visibleArea.width, visibleArea.height);
            renderWindow().setView(sf::View(visibleArea));
        }

        if(key == sf::Keyboard::Key::Add && press)
        {
            auto& cycles = m_viewer.m_emulator.settings().m_cycles;
            cycles += 1;
        }

        if(key == sf::Keyboard::Key::Subtract && press)
        {
            auto& cycles = m_viewer.m_emulator.settings().m_cycles;
            cycles -= 1;
            cycles = std::max(1, cycles);
        }

        if(key == sf::Keyboard::Key::Space && press)
        {
            std::cout << m_viewer.emulator() << std::endl;
        }

        if(ctrl && key == sf::Keyboard::Key::J)
        {
            auto& setting = m_viewer.emulator().settings().m_jumping;
            setting = !setting;
        }

        if(ctrl && key == sf::Keyboard::Key::S)
        {
            auto& setting = m_viewer.emulator().settings().m_shifting;
            setting = !setting;
        }

        if(ctrl && key == sf::Keyboard::Key::M)
        {
            auto& setting = m_viewer.emulator().settings().m_memory;
            setting = !setting;
        }

        if(ctrl && key == sf::Keyboard::Key::R)
        {
            auto& setting = m_viewer.emulator().settings().m_vf_reset;
            setting = !setting;
        }
    }

private:
    Viewer& m_viewer;
};

}


Viewer::Viewer() : m_fps_counter{ 0.0, 0.0, 0 }
{

}

void Viewer::run()
{
    /* Window Creation */
    Window::ContextSettings context;
    context.mMajorVersion = 2;
    context.mMinorVersion = 0;

    Window::VideoMode mode;
    mode.mWidth = emu::Chip8::width_res * m_render.m_scale;
    mode.mHeight = emu::Chip8::height_res * m_render.m_scale;
    mode.mBitsPerPixel = 32;

    auto window = detail::CBWindow(*this, "Chip-8 Emulator", mode);

    /* display */
    m_display_image.create(emu::Chip8::width_res, emu::Chip8::height_res);
    m_display_texture.loadFromImage(m_display_image);
    m_display_sprite.setTexture(m_display_texture);
    m_display_sprite.setScale(16, 16);

    /* Timer init */
    m_fps_counter.fps = 60;
    m_fps_counter.frames = 0;

    /* Main Loop */
    while(window.open())
    {
        window.clear();
        window.pollEvents();

        /* Timer and fps counter*/
        float elapsed = 1.0 / 60.0f;
        m_fps_counter.accumTime += elapsed;
        m_fps_counter.frames++;
        if(m_fps_counter.accumTime >= 1.0)
        {
            m_fps_counter.fps = static_cast<double>(m_fps_counter.frames) * 0.5 + m_fps_counter.fps * 0.5;
            m_fps_counter.accumTime -= 1.0f;
            m_fps_counter.frames = 0;
        }

        /* update */
        m_emulator.tick();

        /* render */
        {
            const auto& display = m_emulator.display();

            for(int y = 0; y < emu::Chip8::height_res; y++)
            {
                for(int x = 0; x < emu::Chip8::width_res; x++)
                {
                    m_display_image.setPixel(x, y,
                                             display[x + y*emu::Chip8::width_res] ?
                                sf::Color{255, 255, 255} : sf::Color{0, 0, 0});
                }
            }

            m_display_texture.loadFromImage(m_display_image);
            window.draw(m_display_sprite);
        }

        window.display();
    }
}

emu::Chip8& Viewer::emulator()
{
    return m_emulator;
}
