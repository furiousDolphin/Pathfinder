
#include <memory>
#include <iomanip>

#include "App.hpp"
#include "GameMode.hpp"
#include "EditorMode.hpp"

App::App( SDL_Window* window, SDL_Renderer* renderer ):
    window_          { window            },
    renderer_        { renderer          },
    event_manager_   {                   },
    mode_type_       { ModeType::EDITOR  },
    fonts_           {                   },
    app_context_     { window,
                       renderer, 
                       event_manager_, 
                       mode_type_,
                       fonts_            },
    modes_map_       {                   }
{

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    fonts_.emplace( FontTypes::MINECRAFT_18, FontManager( "data/fonts/MinecraftBold-nMK1.otf", 18 ) );
    fonts_.emplace( FontTypes::MINECRAFT_24, FontManager( "data/fonts/MinecraftBold-nMK1.otf", 24 ) );
    fonts_.emplace( FontTypes::MINECRAFT_36, FontManager( "data/fonts/MinecraftBold-nMK1.otf", 36 ) );

    modes_map_.emplace(ModeType::GAME,   std::make_unique<GameMode>  ( app_context_ ) );
    modes_map_.emplace(ModeType::EDITOR, std::make_unique<EditorMode>( app_context_ ) );

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

}

App::~App() = default;

void App::run()
{
    while( !event_manager_.get_quit() )
    {
        current_time_ = SDL_GetTicks();
        delta_time_ = (current_time_ - last_time_) / 1000.0f;
        last_time_ = current_time_;

        modes_map_.at( mode_type_ )->run( delta_time_ );
        event_manager_.update();
    } 
}

