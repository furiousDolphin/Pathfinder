

#include "Button.hpp"

TextButton::TextButton(       Vector2D<int>               pos,
                              SDL_Renderer*               renderer,
                        const std::string&                text,
                              SDL_Color                   marked_color,
                              SDL_Color                   unmarked_color,
                              TTF_Font*                   font, 
                              std::function< void(void) > fun ):

    Button   { pos                                      },
    pos_     { pos                                      },
    textures_{ { renderer, text,   marked_color, font },
               { renderer, text, unmarked_color, font },
               { nullptr                              } },
    fun_     { fun                                      }
{
    auto w = textures_.marked.get_width();
    auto h = textures_.marked.get_width();
 
    rect_.set_w( w );
    rect_.set_h( h );
}

bool TextButton::update( const EventManager& event_manager )
{
    bool collision = false;

    if( rect_.collide_point( event_manager.mouse_pos() ) )
    {
        collision = true;
        textures_.current_ptr = &textures_.marked;
        if( event_manager.left_got_clicked() )
        { this->fun_(); }
    }

    else
    { textures_.current_ptr = &textures_.unmarked; }

    return collision;
}

void TextButton::render( SDL_Renderer* renderer ) const
{
    textures_.current_ptr->render( rect_.get_pos() );
}



bool Buttons::update( const EventManager& event_manager )
{
    bool any = false;

    for( auto& button : buttons_ )
    {
        if(button->update( event_manager ))
            any = true;
    }
    return any;
}

void Buttons::render( SDL_Renderer* renderer ) const
{
    for( const auto& button : buttons_ )
    {
        button->render(renderer);
    }
}

void Buttons::add( std::unique_ptr<Button> button )
{
    buttons_.push_back( std::move(button) );
}

