
#include "Settings.hpp"
#include "SDL_Management.hpp"

#include <iostream>

bool Init( SDL_Window*& window, SDL_Renderer*& renderer )
{
    bool success = true;

    if( SDL_Init( SDL_INIT_AUDIO | SDL_INIT_VIDEO ) )
    {
        std::cerr << "Błąd inicjalizacji SDL" << SDL_GetError() << std::endl;
    }

    else
    {
        window = SDL_CreateWindow( "name", 
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WIDTH, 
                                   HEIGHT,
                                   SDL_WINDOW_SHOWN);

        if( window == nullptr )
        {
            std::cerr << "Błąd tworzenia \"window\"" << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            renderer = SDL_CreateRenderer( window,
                                           -1,
                                           SDL_RENDERER_ACCELERATED);
            if(renderer == nullptr)
            {
                std::cerr << "Błąd tworzenia \"renderer\"" << IMG_GetError() << std::endl;
                success = false;
            }
            else
            {
                int img_flags = IMG_INIT_PNG;

                if( !(IMG_Init(img_flags) & img_flags) )
                {
                    std::cerr << "Błąd inicjalizacji \"IMG\"" << IMG_GetError() << std::endl;
                    success = false;
                }
                else
                {
                    if( TTF_Init() == -1 )
                    {
                        std::cerr << "Błąd inicjalizacji \"TTF\"" << TTF_GetError() << std::endl;
                        success = false;
                    }
                }
            }
        }
    }
    return success;
}

void Close( SDL_Window*& window, SDL_Renderer*& renderer )
{
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );

    window   = nullptr;
    renderer = nullptr;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}


FontManager::FontManager( const std::string& font_path, std::size_t font_size ):
    font_path_ { font_path },
    font_size_ { font_size }
{
    font_ = std::shared_ptr<TTF_Font>( TTF_OpenFont( font_path.c_str(), font_size),
                                       [](TTF_Font* f){ TTF_CloseFont(f); }         );                                  
}