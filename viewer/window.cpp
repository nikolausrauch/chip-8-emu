#include "window.h"


Window::Window(const std::string &title, const Window::VideoMode &mode, uint32_t style, const Window::WindowHints &hints, const Window::ContextSettings &settings)
    : mRenderWindow(
          sf::VideoMode(mode.mWidth, mode.mHeight, mode.mBitsPerPixel),
          title,
          style,
          sf::ContextSettings(settings.mDepthBits, settings.mStencilBits, settings.mAALevel, settings.mMajorVersion, settings.mMinorVersion, settings.mAttributeFlags)),
      mCurrentMode(mode),
      mCurrentContext(settings),
      mCurrentHints(hints),
      mStyle(style),
      mStatus(eStatus::OPEN | eStatus::VISIBLE),
      mTitle(title)
{
    mRenderWindow.setVisible(true);
    mRenderWindow.setFramerateLimit(mCurrentHints.mFramerate);
    mRenderWindow.setVerticalSyncEnabled(mCurrentHints.mVsyncEnabled);
    mRenderWindow.setSize(sf::Vector2u(mode.mWidth, mode.mHeight));

    mStatus |= mRenderWindow.hasFocus() ? eStatus::FOCUSED : 0x0;

    const sf::ContextSettings& contextSettings = mRenderWindow.getSettings();
    mCurrentContext.mAALevel = contextSettings.antialiasingLevel;
    mCurrentContext.mAttributeFlags = contextSettings.attributeFlags;
    mCurrentContext.mStencilBits = contextSettings.stencilBits;
    mCurrentContext.mDepthBits = contextSettings.stencilBits;
    mCurrentContext.mMajorVersion = contextSettings.majorVersion;
    mCurrentContext.mMinorVersion = contextSettings.minorVersion;
}

Window::~Window()
{

}

void Window::title(const std::string &title)
{
    mRenderWindow.setTitle(title);
    mTitle = title;
}

void Window::size(unsigned int width, unsigned int height)
{
    mCurrentMode.mHeight = height;
    mCurrentMode.mWidth = width;
    mRenderWindow.setSize(sf::Vector2u(width,height));
}

void Window::icon(unsigned int width, unsigned int height, const uint8_t *pixels)
{
    mRenderWindow.setIcon(width, height, pixels);
}

std::string Window::title() const
{
    return mTitle;
}

sf::Vector2u Window::size() const
{
    return sf::Vector2u(mCurrentMode.mWidth, mCurrentMode.mHeight);
}

const Window::ContextSettings& Window::currentContext() const
{
    return mCurrentContext;
}

const Window::VideoMode &Window::videoMode() const
{
    return mCurrentMode;
}

void Window::show()
{
    mRenderWindow.setVisible(true);
    mStatus |= eStatus::VISIBLE;
}

void Window::hide()
{
    mRenderWindow.setVisible(false);
    mStatus &= ~eStatus::VISIBLE;
}

void Window::close()
{
    mRenderWindow.close();
}

void Window::pollEvents()
{
    sf::Event event;
    while(mRenderWindow.pollEvent(event))
    {
        switch(event.type)
        {
        case sf::Event::Closed:
            mStatus &= ~eStatus::OPEN;
            close();
            break;
        case sf::Event::Resized:
        {
            mCurrentMode.mWidth = event.size.width;
            mCurrentMode.mHeight = event.size.height;
            onResize(event.size.width, event.size.height);
            break;
        }
        case sf::Event::GainedFocus:
            mStatus |= eStatus::FOCUSED;
            break;
        case sf::Event::LostFocus:
            mStatus &= ~eStatus::FOCUSED;
            break;
        case sf::Event::KeyPressed:
            onKey(event.key.code, event.key.control, true);
            break;
        case sf::Event::KeyReleased:
            onKey(event.key.code, event.key.control, false);
            break;
        case sf::Event::TextEntered:
            onChar(event.text.unicode);
            break;
        case sf::Event::MouseEntered:
            break;
        case sf::Event::MouseLeft:
            break;
        case sf::Event::MouseMoved:
            break;
        case sf::Event::MouseButtonPressed:
            onMouseButton(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button, 0, true);
            break;
        case sf::Event::MouseButtonReleased:
            onMouseButton(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button, 0, false);
            break;
        case sf::Event::MouseWheelScrolled:
            onMouseScroll(event.mouseWheelScroll.delta);
        default: break;
        }
    }

}

static Window* gLastActiveContext = nullptr;
void Window::makeCurrent()
{
    if (gLastActiveContext != this)
    {
        mRenderWindow.setActive(true);
        gLastActiveContext = this;
    }
}

void Window::display()
{
        mRenderWindow.display();
}

void Window::clear(const sf::Color &color)
{
    mRenderWindow.clear(color);
}

void Window::framerate(unsigned int fps)
{
    mCurrentHints.mFramerate = fps;
    mRenderWindow.setFramerateLimit(fps);
}

void Window::vsync(bool enable)
{
    mCurrentHints.mVsyncEnabled = enable;
    mRenderWindow.setVerticalSyncEnabled(enable);
}

void Window::fullscreen(bool enable)
{
    uint32_t style = (enable) ? eWindowStyle::FULLSCREEN : eWindowStyle::BASIC;

    if(mStyle & style)
        return;

    mStyle = style;
    mRenderWindow.create(
                sf::VideoMode(mCurrentMode.mWidth, mCurrentMode.mHeight, mCurrentMode.mBitsPerPixel),
                mTitle,
                mStyle,
                sf::ContextSettings(mCurrentContext.mDepthBits, mCurrentContext.mStencilBits, mCurrentContext.mAALevel, mCurrentContext.mMajorVersion, mCurrentContext.mMinorVersion, mCurrentContext.mAttributeFlags));
}

unsigned int Window::framerate() const
{
    return mCurrentHints.mFramerate;
}

bool Window::vsync() const
{
    return mCurrentHints.mVsyncEnabled;
}

bool Window::open()
{
    return mRenderWindow.isOpen();
}

bool Window::focused()
{
    return mStatus & eStatus::FOCUSED;
}

void Window::draw(const sf::Drawable &obj)
{
    mRenderWindow.draw(obj);
}

sf::RenderWindow &Window::renderWindow()
{
    return mRenderWindow;
}

std::vector<Window::VideoMode> Window::fullscrenModes()
{
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    std::vector<Window::VideoMode> results;
    results.reserve(modes.size());

    for(const sf::VideoMode& m : modes)
        results.push_back(Window::VideoMode{m.width, m.height, m.bitsPerPixel});

    return results;
}

Window::VideoMode Window::desktopMode()
{
    sf::VideoMode mode = sf::VideoMode::getDesktopMode();

    return Window::VideoMode{mode.width, mode.height, mode.bitsPerPixel};
}

void Window::onKey(int key, bool ctrl, bool press)
{

}

void Window::onChar(unsigned int c)
{

}

void Window::onMouseButton(int x, int y, int button, int mod, bool pressed)
{

}

void Window::onMouseScroll(double delta)
{

}

void Window::onResize(int width, int height)
{

}

Window::ContextSettings::ContextSettings()
    : mAttributeFlags(eContextType::DEFAULT),
      mMajorVersion(3),
      mMinorVersion(0),
      mDepthBits(0),
      mStencilBits(0),
      mAALevel(0)
{

}

Window::WindowHints::WindowHints()
    : mFramerate(0), mVsyncEnabled(false)
{
}

Window::WindowHints::WindowHints(unsigned int fps, bool vsync)
    : mFramerate(fps), mVsyncEnabled(vsync)
{

}

bool Window::VideoMode::isValid()
{
    return sf::VideoMode(mWidth, mHeight, mBitsPerPixel).isValid();
}
