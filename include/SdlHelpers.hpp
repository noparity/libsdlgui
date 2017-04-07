#ifndef SDLHELPERS_HPP
#define SDLHELPERS_HPP

#include <boost/filesystem.hpp>
#include <SDL_error.h>
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>

// list of SDL subsystems that can be initialized
enum class SDLSubSystem
{
    Timer = SDL_INIT_TIMER,
    Audio = SDL_INIT_AUDIO,
    Video = SDL_INIT_VIDEO,
    Joystick = SDL_INIT_JOYSTICK,
    Haptic = SDL_INIT_HAPTIC,
    GameController = SDL_INIT_GAMECONTROLLER,
    Events = SDL_INIT_EVENTS,
    NoParachute = SDL_INIT_NOPARACHUTE
};

inline std::string SDLGetError() { return std::string(SDL_GetError()); }
inline std::string IMGGetError() { return std::string(IMG_GetError()); }
inline std::string TTFGetError() { return std::string(TTF_GetError()); }

// class that wraps initialization and shutdown of SDL
class SDLInit
{
public:
    SDLInit();
    SDLInit(SDLSubSystem subsystem);
    ~SDLInit();
};

// class that wraps initialization and shutdown of specific SDL subsystems
class SDLInitSubSystem
{
private:
    SDLSubSystem m_subsystem;

public:
    SDLInitSubSystem(SDLSubSystem subsystem);
    ~SDLInitSubSystem();
};

// list of IMG subsystems that can be initialized
enum class IMGSubSystem
{
    Png = IMG_INIT_PNG
};

// class that wraps initialization and shutdown of IMG
class IMGInit
{
private:
    IMGSubSystem m_subsystem;
    static bool m_init;

public:
    IMGInit(IMGSubSystem subsystem);
    ~IMGInit();

    static bool IsInit() { return m_init; }
};

// class that wraps initialization and shutdown of TTF
class TTFInit
{
public:
    TTFInit();
    ~TTFInit();
};

// thin wrapper around an SDL_Surface*
class SDLSurface
{
private:
    SDL_Surface* m_pSurface;
    SDLSurface(const SDLSurface&);

    void Cleanup();
public:
    SDLSurface() { m_pSurface = nullptr; }
    SDLSurface(const boost::filesystem::path& fileName);
    SDLSurface(SDLSurface&& other);
    SDLSurface(SDL_Surface* pSurface);
    ~SDLSurface();

    operator SDL_Surface*() const { return m_pSurface; }
    SDL_Surface* operator->() const { return m_pSurface; }
};

// thin wrapper around an SDL_Texture*
class SDLTexture
{
private:
    SDL_Texture* m_pTexture;
    int m_width;
    int m_height;
    SDLTexture(const SDLTexture&);

    void Cleanup();
public:
    SDLTexture() : m_pTexture(nullptr), m_width(0), m_height(0) {}
    SDLTexture(SDL_Texture* pTexture, int width, int height);
    SDLTexture(SDLTexture&& other);
    ~SDLTexture();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    operator SDL_Texture*() const { return m_pTexture; }
    SDLTexture& operator=(SDLTexture& rhs);
    SDLTexture& operator=(SDLTexture&& rhs);
};

// encapsulates a TTF_Font*
class TTFFont
{
private:
    TTF_Font* m_pFont;

public:
    TTFFont(const boost::filesystem::path& fileName, int size);
    TTFFont(TTFFont&& other);
    ~TTFFont();

    operator TTF_Font*() const { return m_pFont; }
    TTF_Font* Get() const { return m_pFont; }
};

// returns an SDL_Color struct from the specified arguments
inline SDL_Color SDLColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_Color color = { r, g, b, a };
    return color;
}

inline bool operator==(const SDL_Color& lhs, const SDL_Color& rhs)
{
    static_assert(sizeof(SDL_Color) == sizeof(uint32_t), "sizeof SDL_Color != sizeof DWORD");
    return *(reinterpret_cast<uint32_t const*>(&lhs)) == *(reinterpret_cast<uint32_t const*>(&rhs));
}

inline bool operator!=(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return !(lhs == rhs);
}

// returns a color of lower intensity
inline SDL_Color operator/(const SDL_Color& lhs, uint8_t rhs)
{
    return SDLColor(lhs.r / rhs, lhs.g / rhs, lhs.b / rhs, lhs.a / rhs);
}

// returns an SDL_Rect struct from the specified arguments
inline SDL_Rect SDLRect(int x, int y, int w, int h)
{
    SDL_Rect rect = { x, y, w, h };
    return rect;
}

// returns an SDL_Rect struct from the specified arguments. the x and
// y values are adjusted with respect to relative.x and relative.y.
inline SDL_Rect SDLRect(int x, int y, int w, int h, const SDL_Rect& relative)
{
    SDL_Rect rect = { relative.x + x, relative.y + y, w, h };
    return rect;
}

inline bool operator==(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
    return lhs.h == rhs.h && lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
    return !(lhs == rhs);
}

// returns an SDL_Point struct from the specified arguments
inline SDL_Point SDLPoint(int x, int y)
{
    SDL_Point point = { x, y };
    return point;
}

inline bool operator==(const SDL_Point& lhs, const SDL_Point& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const SDL_Point& lhs, const SDL_Point& rhs)
{
    return !(lhs == rhs);
}

// class used for switching the drawing color.  when the
// object goes out of scope the original color is restored.
class SDLColorHolder
{
private:
    SDL_Renderer* m_pRenderer;
    SDL_Color m_color;

public:
    SDLColorHolder(SDL_Renderer* pRenderer, const SDL_Color& newColor);
    ~SDLColorHolder();
};

// returns true if the bottom rect is occluded by top
inline bool SDLRectOcclusion(const SDL_Rect& top, const SDL_Rect& bottom)
{
    return (bottom.x >= top.x && bottom.y >= top.y &&
        (bottom.x + bottom.w) <= (top.x + top.w) &&
        (bottom.y + bottom.h) <= (top.y + top.h));
}

// returns true if the specified points are within rect
inline bool SDLPointInRect(const SDL_Point& point, const SDL_Rect& rect)
{
    return (point.x >= rect.x && point.x <= (rect.x + rect.w) &&
        point.y >= rect.y && point.y <= (rect.y + rect.h));
}

#endif // SDLHELPERS_HPP
