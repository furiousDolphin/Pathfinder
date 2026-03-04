#ifndef TEXTURE_
#define TEXTURE_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <stdexcept>
#include <filesystem>
#include <memory>
#include <string>

#include "Vector2D.hpp"

namespace fs = std::filesystem;

class SDLTextureDeleter
{
    public:
        void operator()( SDL_Texture* texture ) const noexcept
        {
            if(texture != nullptr) { SDL_DestroyTexture(texture); }
        }
};

class Texture
{
    public:
        Texture() = default;

        explicit Texture( SDL_Renderer* renderer ):
            renderer_{ renderer }
        {}

        Texture( SDL_Renderer* renderer, const std::string& path ):
            Texture{renderer}
        {
            if( !load_file(path) )
                throw std::invalid_argument( "Nie oalo sie zaladowac pliku -> " + path );
        }

        Texture( SDL_Renderer* renderer, const std::string& text, SDL_Color color, TTF_Font* font ):
            Texture{renderer}
        {
            if( !load_text( text, color, font ) )
                throw std::invalid_argument( "Nie udalo sie zaladowac tekstu -> " + text );
        }

        Texture( const Texture& )            = delete;
        Texture& operator=( const Texture& ) = delete;

        Texture( Texture&& other )            noexcept = default;
        Texture& operator=( Texture&& other ) noexcept = default;

        ~Texture() = default;

        void set_blend_mode( SDL_BlendMode blending ) 
        {
            if( texture_ != nullptr ) 
                SDL_SetTextureBlendMode( texture_.get(), blending );
        }

        void set_alpha( Uint8 alpha ) 
        {
            if( texture_ != nullptr ) 
                SDL_SetTextureAlphaMod( texture_.get(), alpha );
        }

        void render( int x, int y, const SDL_Rect* clip = nullptr, double angle = 0.0,
                     const SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE ) const
        {
            if( texture_ != nullptr )
            {
                SDL_Rect rect{ x, y, width_, height_ };

                if( clip != nullptr )
                {
                    rect.w = clip->w;
                    rect.h = clip->h;
                }

                SDL_RenderCopyEx( renderer_, texture_.get(), clip, &rect, angle, center, flip );
            }
        }

        void render( const Vector2D<int>& v, const SDL_Rect* clip = nullptr, double angle = 0.0,
                     const SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE ) const
        {
            this->render( v.x, v.y, clip, angle, center, flip );
        }


        SDL_Texture* get() const { return texture_.get(); }

        int get_width()  const { return width_;  }
        int get_height() const { return height_; }

    private:
        std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture_;

        SDL_Renderer* renderer_;

        int width_;
        int height_;

        bool load_file( const std::string& path )
        {
            char* base_path = SDL_GetBasePath();

            if( !base_path )
                throw std::invalid_argument( "nie udalo sie pobrac base path" );

            fs::path exe_path{ base_path };
            fs::path project_root = exe_path.parent_path().parent_path();
            fs::path file_path = project_root / path;
            
            SDL_Surface* surface = IMG_Load( file_path.string().c_str() );

            if( surface == nullptr )
            {
                SDL_Log( "IMG_Load error: %s", IMG_GetError() );
                return false;
            }

            SDL_SetColorKey( surface, 
                             SDL_TRUE, 
                             SDL_MapRGB( surface->format, 0, 0xff, 0xff ) );

            texture_.reset( SDL_CreateTextureFromSurface( renderer_, surface ) );

            if( texture_ == nullptr )
            {
                SDL_Log( "SDL_CreateTextureFromSurface error: %s", SDL_GetError() );
                SDL_FreeSurface(surface);
                return false;
            }

            width_  = surface->w;
            height_ = surface->h;

            SDL_FreeSurface( surface );
            return true;
        }

        bool load_text( const std::string& text, SDL_Color color, TTF_Font* font )
        {
            SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);

            if( surface == nullptr )
            {
                SDL_Log( "TTF_RenderText_Solid error: %s", TTF_GetError() );
                return false;
            }

            texture_.reset( SDL_CreateTextureFromSurface( renderer_, surface ) );
            if( texture_ == nullptr )
            {
                SDL_Log( "SDL_CreateTextureFromSurface error: %s", SDL_GetError() );
                SDL_FreeSurface(surface);
                return false;
            }

            width_  = surface->w;
            height_ = surface->h;

            SDL_FreeSurface(surface);
            return true;
        }
};


#endif