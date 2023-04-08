#include "stdafx.h"
#include "Exceptions.hpp"
#include "SdlHelpers.hpp"

SDLInit::SDLInit()
{
    if (SDL_Init(0) != 0)
    {
        auto message = "SDL_Init(0) failed with '" + SDLGetError() + "'.";
        throw SDLException(message);
    }
}

SDLInit::SDLInit(SDLSubSystem subsystem)
{
    if (SDL_Init(static_cast<uint32_t>(subsystem)) != 0)
    {
        auto message = "SDL_Init(" + std::to_string(static_cast<uint32_t>(subsystem)) + ") failed with '" + SDLGetError() + "'.";
        throw SDLException(message);
    }
}

SDLInit::~SDLInit()
{
    SDL_Quit();
}

SDLInitSubSystem::SDLInitSubSystem(SDLSubSystem subsystem) : m_subsystem(subsystem)
{
    if (SDL_InitSubSystem(static_cast<uint32_t>(m_subsystem)) != 0)
    {
        auto message = "SDL_InitSubsystem(" + std::to_string(static_cast<uint32_t>(subsystem)) + ") failed with '" + SDLGetError() + "'.";
        throw SDLException(message);
    }
}

SDLInitSubSystem::~SDLInitSubSystem()
{
    SDL_QuitSubSystem(static_cast<uint32_t>(m_subsystem));
}

bool IMGInit::m_init = false;

IMGInit::IMGInit(IMGSubSystem subsystem)
{
    auto result = IMG_Init(static_cast<uint32_t>(subsystem));
    if ((result & static_cast<uint32_t>(subsystem)) != static_cast<uint32_t>(subsystem))
    {
        auto message = "IMG_Init(" + std::to_string(static_cast<uint32_t>(subsystem)) + ") failed with '" + IMGGetError() + "'.";
        throw SDLException(message);
    }

    IMGInit::m_init = true;
}

IMGInit::~IMGInit()
{
    IMG_Quit();
    IMGInit::m_init = false;
}

TTFInit::TTFInit()
{
    if (TTF_Init() != 0)
    {
        auto message = "TTF_Init() failed with '" + TTFGetError() + "'.";
        throw SDLException(message);
    }
}

TTFInit::~TTFInit()
{
    TTF_Quit();
}

SDLSurface::SDLSurface(const std::filesystem::path& fileName) : m_pSurface(nullptr)
{
    assert(IMGInit::IsInit());
    m_pSurface = IMG_Load(fileName.string().c_str());
    if (m_pSurface == nullptr)
    {
        auto message = "IMG_Load(" + fileName.string() + ") failed with '" + IMGGetError() + "'.";
        throw SDLException(message);
    }
}

SDLSurface::SDLSurface(SDLSurface&& other)
{
    m_pSurface = other.m_pSurface;
    other.m_pSurface = nullptr;
}

SDLSurface::SDLSurface(SDL_Surface* pSurface)
{
    assert(pSurface != nullptr);
    m_pSurface = pSurface;
}

SDLSurface::~SDLSurface()
{
    Cleanup();
}

void SDLSurface::Cleanup()
{
    if (m_pSurface != nullptr)
        SDL_FreeSurface(m_pSurface);
}

SDLTexture::SDLTexture(SDL_Texture* pTexture, int width, int height) : m_pTexture(pTexture), m_width(width), m_height(height)
{
    assert(m_pTexture != nullptr);
}

SDLTexture::SDLTexture(SDLTexture&& other) : m_pTexture(nullptr), m_width(0), m_height(0)
{
    *this = std::move(other);
}

SDLTexture::~SDLTexture()
{
    Cleanup();
}

void SDLTexture::Cleanup()
{
    if (m_pTexture != nullptr)
        SDL_DestroyTexture(m_pTexture);
}

SDLTexture& SDLTexture::operator=(SDLTexture& rhs)
{
    if (this != &rhs)
    {
        Cleanup();
        m_pTexture = rhs.m_pTexture;
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        rhs.m_pTexture = nullptr;
        rhs.m_width = 0;
        rhs.m_height = 0;
    }

    return *this;
}

SDLTexture& SDLTexture::operator=(SDLTexture&& rhs)
{
    return *this = rhs;
}

TTFFont::TTFFont(const std::filesystem::path& fileName, int size)
{
    m_pFont = TTF_OpenFont(fileName.string().c_str(), size);
    if (m_pFont == nullptr)
    {
        auto message = "TTF_OpenFont(" + fileName.string() + ") failed with '" + TTFGetError() + "'.";
        throw SDLException(message);
    }
}

TTFFont::TTFFont(TTFFont&& other)
{
    assert(m_pFont == nullptr);
    m_pFont = other.m_pFont;
    other.m_pFont = nullptr;
}

TTFFont::~TTFFont()
{
    if (m_pFont != nullptr)
        TTF_CloseFont(m_pFont);
}

SDLColorHolder::SDLColorHolder(SDL_Renderer* pRenderer, const SDL_Color& newColor) : m_pRenderer(pRenderer)
{
    assert(pRenderer != nullptr);
    SDL_GetRenderDrawColor(m_pRenderer, &m_color.r, &m_color.g, &m_color.b, &m_color.a);

    if (m_color != newColor)
    {
        SDL_SetRenderDrawColor(m_pRenderer, newColor.r, newColor.g, newColor.b, newColor.a);
        // track we need to restore the original
        m_pRenderer = reinterpret_cast<SDL_Renderer*>(reinterpret_cast<size_t>(m_pRenderer) | 1);
    }
}

SDLColorHolder::~SDLColorHolder()
{
    if (reinterpret_cast<size_t>(m_pRenderer) & 1)
    {
        m_pRenderer = reinterpret_cast<SDL_Renderer*>(reinterpret_cast<size_t>(m_pRenderer) & ~1);
        SDL_SetRenderDrawColor(m_pRenderer, m_color.r, m_color.g, m_color.b, m_color.a);
    }
}
