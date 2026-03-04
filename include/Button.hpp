

#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "Vector2D.hpp"
#include "Rect.hpp"

#include "Settings.hpp"
#include "EventManager.hpp"
#include "SDL_Management.hpp"

class Buttons;

class Button
{
    public:
        Button( Vector2D<int> pos ):
            rect_{ pos }
        {}

        virtual ~Button() = default;

        virtual bool update( const EventManager& event_manager ) = 0;
        virtual void render( SDL_Renderer* renderer ) const      = 0;
        
        Vector2D<int> get_pos() { return rect_.get_pos(); }
        void          set_pos( Vector2D<int> pos ) { rect_.set_pos(pos); }

    protected:
        Rect rect_;

};


class TextButton : public Button
{
    public:
        TextButton(       
            Vector2D<int> pos,
            SDL_Renderer* SDL_Renderer,
            const std::string& text,
            SDL_Color marked_color,
            SDL_Color unmarked_color,
            TTF_Font* font,
            std::function< void(void) > fun = nullptr
        );

        bool update( const EventManager& event_manager ) override;
        void render( SDL_Renderer* renderer ) const override;

    private:
        std::function< void(void) > fun_;
        Vector2D<int> pos_;

        struct Textures
        {
            Texture  marked;
            Texture  unmarked;
            Texture* current_ptr;
        } textures_;
};



class Buttons
{
    public:
        Buttons() = default;
        bool update( const EventManager& event_manager );
        void render( SDL_Renderer* renderer ) const;
        void add( std::unique_ptr<Button> button );
    private:
        std::vector<std::unique_ptr<Button>> buttons_;
};


#endif
