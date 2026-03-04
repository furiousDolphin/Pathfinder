
#ifndef SETTINGS_
#define SETTINGS_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <unordered_map>

#include "SDL_Management.hpp"
#include "Vector2D.hpp"
#include "Texture.hpp"
#include "Rect.hpp"
#include "EventManager.hpp"

//--------------------------------

inline constexpr int WIDTH  = 1080;
inline constexpr int HEIGHT = 720;
inline constexpr int TILE_SIZE = 64;

//--------------------------------

enum class ModeType 
{ 
    GAME, 
    EDITOR 
};

enum class FontTypes
{
    MINECRAFT_18,
    MINECRAFT_24,
    MINECRAFT_36
};

struct AppContext
{   
    SDL_Window*      window;
    SDL_Renderer*    renderer;
    EventManager&    event_manager;
    ModeType&        mode_type;
    std::unordered_map< FontTypes, FontManager >& fonts;
};

#endif 