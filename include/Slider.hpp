
#ifndef SLIDER_HPP_
#define SLIDER_HPP_

#include "Vector2D.hpp"
#include "Rect.hpp"
#include "EventManager.hpp"
#include "Texture.hpp"

#include <memory>
#include <functional>


enum class SliderTextureKeys
{
    BUTTON_PRESSED,
    BUTTON_RELEASED,
    PLATFORM
};

using slider_textures_t = std::unordered_map< SliderTextureKeys, std::shared_ptr<Texture> >;

class Slider
{
    public:
        Slider( Vector2D<int> pos ):
            rect_{pos}
        {}
        
        virtual ~Slider() = default;

        virtual void update( const EventManager& event_manager) = 0;
        virtual void render( SDL_Renderer* renderer ) = 0;

        Vector2D<int> get_pos() { return rect_.get_pos(); }
        void set_pos( Vector2D<int> pos ) { rect_.set_pos(pos); }

    protected:
        Rect rect_;
};


class Sliders
{
    public:
        Sliders() = default;
        virtual ~Sliders() = default;

        void update( const EventManager& event_manager );
        void render( SDL_Renderer* renderer );

        void add( std::unique_ptr<Slider> slider );

    private:
        std::vector<std::unique_ptr<Slider>> sliders_;

};


class BasicSlider : public Slider
{
    public:
        BasicSlider( 
            Vector2D<int> pos, 
            slider_textures_t  textures, 
            double max_val, 
            double min_val, 
            double init_val, 
            std::function<void(double)> func = nullptr 
        );

        void update( const EventManager& event_manager) override;
        void render( SDL_Renderer* renderer ) override;

    private:   
        slider_textures_t textures_;
        double min_val_;
        double max_val_;
        double cur_val_;
        std::function<void(double)> func_; 

        Rect platform_rect_;
        Rect button_rect_;
        std::shared_ptr<Texture> cur_button_texture_;
        std::shared_ptr<Texture> platform_texture_;

        Vector2D<int> offset_;
        double slide_length_;
        bool pressed_;
};


#endif
