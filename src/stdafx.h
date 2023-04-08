#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <algorithm>
#include <atomic>
#include <boost/dynamic_bitset.hpp>
#include <filesystem>
#include <cassert>
#include <cctype>
#include <functional>
#include <memory>
#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_mouse.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_video.h>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>
