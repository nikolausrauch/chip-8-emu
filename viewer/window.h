#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <type_traits>
#include <utility>

/*
 *  Window:
 *  -----------------------------
 *  -> encapsulates SFML Window
 *  -> access event handling via inheritance
 *       virtual void onKey(int key, int mod, bool press);
 *       virtual void onChar(unsigned int c);
 *       virtual void onMouseButton(int x, int y, int button, int mod, bool pressed);
 *       virtual void onMouseScroll(double delta);
 *       virtual void onResize(int width, int height);
 *
 *  -----------------------------
 *  @author Nikolaus Rauch
 *  @date 28.09.2017
 */
struct Window
{
    enum eStatus : uint32_t
    {
        OPEN    = 1 << 0,
        FOCUSED = 1 << 1,
        VISIBLE = 1 << 2
    };

public:
    enum eContextType : std::underlying_type<sf::ContextSettings::Attribute>::type
    {
        DEFAULT = sf::ContextSettings::Attribute::Default,
        CORE    = sf::ContextSettings::Attribute::Core,
        DEBUG   = sf::ContextSettings::Attribute::Debug
    };

    enum eWindowStyle : uint32_t
    {
        NONE        = 0,
        TITLEBAR    = 1 << 0,
        RESIZE      = 1 << 1,
        CLOSE       = 1 << 2,
        FULLSCREEN  = 1 << 3,

        BASIC       = TITLEBAR | RESIZE | CLOSE
    };

    struct ContextSettings
    {
        ContextSettings();

        unsigned int mAttributeFlags;

        unsigned int mMajorVersion;
        unsigned int mMinorVersion;

        unsigned int mDepthBits;
        unsigned int mStencilBits;

        unsigned int mAALevel;
    };

    struct VideoMode
    {
        unsigned int mWidth;
        unsigned int mHeight;

        unsigned int mBitsPerPixel;

        bool isValid();
    };

    struct WindowHints
    {
        WindowHints();
        WindowHints(unsigned int fps, bool vsync);

        unsigned int mFramerate;
        bool mVsyncEnabled;
    };

public:
    Window(const std::string& title, const VideoMode& mode, uint32_t style = eWindowStyle::BASIC, const WindowHints& hints = WindowHints(), const ContextSettings& settings = ContextSettings());
    Window(const Window&) = delete;
    Window(Window&& w) = delete;
    Window& operator = (const Window&) = delete;
    Window& operator = (Window&& w) = delete;
    ~Window();

    void title(const std::string& title);
    std::string title() const;

    void size(unsigned int width, unsigned int height);
    sf::Vector2u size() const;

    void icon(unsigned int width, unsigned int height, const uint8_t* pixels);

    const ContextSettings& currentContext() const;
    const VideoMode& videoMode() const;

    void show();
    void hide();
    void close();

    void pollEvents();
    void makeCurrent();

    void display();
    void clear(const sf::Color& color = sf::Color::Black);

    unsigned int framerate() const;
    void framerate(unsigned int fps);

    void vsync(bool enable);
    bool vsync() const;

    bool open();
    bool focused();

    void fullscreen(bool enable);

    void draw(const sf::Drawable& obj);
    sf::RenderWindow& renderWindow();

    static std::vector<Window::VideoMode> fullscrenModes();
    static Window::VideoMode desktopMode();

    /* overload for event handling */
    virtual void onKey(int key, bool ctrl, bool press);
    virtual void onChar(unsigned int c);
    virtual void onMouseButton(int x, int y, int button, int mod, bool pressed);
    virtual void onMouseScroll(double delta);
    virtual void onResize(int width, int height);

private:
    sf::RenderWindow    mRenderWindow;
    VideoMode           mCurrentMode;
    ContextSettings     mCurrentContext;
    WindowHints         mCurrentHints;
    uint32_t            mStyle;
    uint32_t            mStatus;

    std::string         mTitle;
};
