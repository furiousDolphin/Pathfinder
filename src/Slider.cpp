
#include <stdexcept>

#include "Slider.hpp"

void Sliders::update( const EventManager& event_manager )
{
    for ( auto& slider : sliders_ )
    {
        slider->update( event_manager);
    }
}

void Sliders::render( SDL_Renderer* renderer )
{
    for ( auto& slider : sliders_ )
    {
        slider->render( renderer );
    }
}

void Sliders::add( std::unique_ptr<Slider> slider )
{
    sliders_.push_back( std::move(slider) );
}


BasicSlider::BasicSlider( 
    Vector2D<int> pos, 
    slider_textures_t  textures, 
    double max_val, 
    double min_val, 
    double init_val, 
    std::function<void(double)> func
):
    Slider{pos},
    textures_{std::move(textures)},
    func_{func},
    pressed_{false},
    max_val_ { std::max( min_val, max_val) },
    min_val_ { std::min( min_val, max_val) },
    cur_val_ { std::clamp( init_val, min_val_, max_val_ ) },
    cur_button_texture_{ textures.at(SliderTextureKeys::BUTTON_RELEASED) },
    platform_texture_{ textures_.at(SliderTextureKeys::PLATFORM) }
{
    auto w = platform_texture_->get_width();
    auto h = platform_texture_->get_height();

    platform_rect_ = Rect{ 
        rect_.get_pos(), w, h,
    };

    const auto& button_pressed_texture = textures_.at(SliderTextureKeys::BUTTON_PRESSED);
    auto w1 = button_pressed_texture->get_width();
    auto h1 = button_pressed_texture->get_height();
    
    const auto& button_released_texture = textures_.at(SliderTextureKeys::BUTTON_RELEASED);
    auto w2 = button_released_texture->get_width();
    auto h2 = button_released_texture->get_height();  

    if ( w1 != w2 || h1 != h2 )
        throw std::runtime_error(" pressed & released button textures valid siizes ");

    button_rect_ = {
        {0, 0}, w1, h1
    };

    slide_length_ = platform_rect_.get_h() - platform_rect_.get_w();

    offset_ = { 
        platform_rect_.get_x() + platform_rect_.get_w() - button_rect_.get_w(),
        platform_rect_.get_y() + platform_rect_.get_w() - button_rect_.get_w() 
    };

    button_rect_.set_x( offset_.x );
    button_rect_.set_y( offset_.y + slide_length_ -  static_cast<int>( ( (cur_val_ - min_val) / (max_val - min_val) ) * slide_length_ ) );
}

void BasicSlider::update( const EventManager& event_manager )
{
    auto mouse_pos = event_manager.mouse_pos();

    if ( event_manager.left_got_clicked() &&
         button_rect_.collide_point( mouse_pos ) )
    {
        pressed_ = true;
        cur_button_texture_ = textures_.at(SliderTextureKeys::BUTTON_PRESSED);
    }

    else if ( event_manager.left_is_clicked() &&
              event_manager.mouse_motion() )
    {
        if ( pressed_ )
        {
            if ( func_ != nullptr )
            {
                auto val = max_val_ - ( (  button_rect_.get_y() - offset_.y ) * ( max_val_ - min_val_ ) ) / slide_length_ ;
                func_(val);
            }
        }
    }

    else if ( event_manager.left_got_unclicked() )
    {
        pressed_ = false;
        cur_button_texture_ = textures_.at(SliderTextureKeys::BUTTON_RELEASED);
    }

}

void BasicSlider::render( SDL_Renderer* renderer )
{   
    platform_texture_->render( platform_rect_.get_pos() );
    cur_button_texture_->render( button_rect_.get_pos() );
}



