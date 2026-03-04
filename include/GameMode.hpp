#ifndef GAMEMODE_HPP_
#define GAMEMODE_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>


#include "Mode.hpp"
#include "Settings.hpp"
#include "Button.hpp"

class GameMode : public Mode
{
    public:
        GameMode( AppContext& app_context );
        
        void create_buttons();

        void update();
        void render();
        void run( float dt );

    private:
        Buttons buttons_;
};

#endif