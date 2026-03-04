

#ifndef MODE_HPP_
#define MODE_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <memory>

#include "Settings.hpp"

class Mode
{
    public:
        Mode( AppContext& app_context ):
            app_context_{ app_context }
        {}

        virtual void run( float dt ) = 0;
        
    protected:
        AppContext& app_context_;
};

#endif