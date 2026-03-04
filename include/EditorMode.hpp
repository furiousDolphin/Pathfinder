
#ifndef EDITOR_MODE_HPP_
#define EDITOR_MODE_HPP_

#include "Worker.hpp"

#include "Button.hpp"
#include "Mode.hpp"
#include "Spline.hpp"

class EditorMode : public Mode
{
    public:
        EditorMode( AppContext& app_context );
        void run( float dt ) override;
        void pan_input();
        void update_grid_pos();
        void draw_grid() const;
        void create_buttons();

        void update( float dt );
        void render();
    
    private:
        Buttons buttons_;
        Vector2D<int> origin_;
        Vector2D<int> mouse_origin_vector_;
        Vector2D<int> grid_pos_;
        Platform platform_;
        std::vector<Worker> workers_;
        Graphs::Collection path_;
        InteractiveSpline spline_;
        Spline path_finder_spline_;

};

#endif