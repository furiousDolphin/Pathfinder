

#include "Spline.hpp"

Vector2D<double> Spline::get_spline_point(double t, bool loop) const
{

    int p0, p1, p2, p3;
    std::size_t n = points_.size();

    if (!loop)
    {
        p1 = (int)t + 1;
        p2 = p1 + 1;
        p3 = p2 + 1;
        p0 = p1 - 1;
    }
    else
    {
        t = std::fmod(t, (double)n);
        if (t < 0) 
        {t += (double)n;}

        p1 = (int)t;
        p2 = (p1 + 1)%n;
        p3 = (p2 + 1)%n;
        p0 = (p1 >= 1) ? (p1 - 1) : (n-1);    
    }

    t = t - (int)t;

    double tt = t*t;
    double ttt = tt*t;

    double q1 = -ttt + 2.0*tt - t;
    double q2 =  3.0*ttt - 5.0*tt     + 2.0;
    double q3 = -3.0*ttt + 4.0*tt + t;
    double q4 =      ttt -     tt;

    double tx = 0.5 * (points_[p0].x * q1 + points_[p1].x * q2 + 
                       points_[p2].x * q3 + points_[p3].x * q4);

    double ty = 0.5 * (points_[p0].y * q1 + points_[p1].y * q2 + 
                       points_[p2].y * q3 + points_[p3].y * q4);

    return Vector2D<double>{tx, ty};
}

Vector2D<double> Spline::get_spline_gradient(double t, bool loop) const
{
    int p0, p1, p2, p3;
    std::size_t n = points_.size();

    if (!loop)
    {
        p1 = (int)t + 1;
        p2 = p1 + 1;
        p3 = p2 + 1;
        p0 = p1 - 1;
    }
    else
    {
        t = std::fmod(t, (double)n);
        if (t < 0) 
        {t += (double)n;}

        p1 = (int)t;
        p2 = (p1 + 1)%n;
        p3 = (p2 + 1)%n;
        p0 = (p1 >= 1) ? (p1 - 1) : (n-1);    
    }

    t = t - (int)t;

    double tt = t*t;
    double ttt = tt*t;

    double q1 = -3.0*tt +  4.0*t - 1.0;
    double q2 =  9.0*tt - 10.0*t;
    double q3 = -9.0*tt +  8.0*t + 1.0;
    double q4 =  3.0*tt -  2.0*t;

    double tx = 0.5 * (points_[p0].x * q1 + points_[p1].x * q2 + 
                       points_[p2].x * q3 + points_[p3].x * q4);

    double ty = 0.5 * (points_[p0].y * q1 + points_[p1].y * q2 + 
                       points_[p2].y * q3 + points_[p3].y * q4);

    return Vector2D<double>{tx, ty};
}

void Spline::update(const EventManager& event_manager, Vector2D<int> origin)
{}

void Spline::render(SDL_Renderer* renderer, Vector2D<int> origin) const
{
    bool loop = false;
    double dt = 0.02;
    //double limit = loop ? (double)points_.size() : (double)points_.size() - 3.0;
    double limit=(double)points_.size()-(loop ? 0 : 3);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    for ( double t = 0.0; t < limit; t+=dt )
    {
        
        Vector2D<double> p1 = this->get_spline_point(t, loop);
        Vector2D<double> p2 = this->get_spline_point(t+dt, loop);

        p1+=(Vector2D<double>)origin;
        p2+=(Vector2D<double>)origin;

        SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    }  
}


InteractiveSpline::InteractiveSpline(std::initializer_list<Vector2D<double>> init_points, int rect_w, int rect_h):
    Spline(init_points),
    grabbed_{0, false, {0, 0}},
    rect_w_{rect_w},
    rect_h_{rect_h},
    big_rect_{0, 0, 0, 0}
{
    std::size_t n = init_points.size();
    rects_.reserve(n);


    for ( std::size_t i = 0; i < n; i++ )
    {
        const auto& point = points_[i];
        int x = point.x-rect_w/2.0;
        int y = point.y-rect_h/2.0;

        //std::unique_ptr<InteractiveRect> rect = std::make_unique<InteractiveRect>(InteractiveRect{std::move(rect), false });

        rects_.emplace_back(static_cast<int>(x), static_cast<int>(y), rect_w_, rect_h_);
        this->update_big_rect();
    }
}

void InteractiveSpline::update(const EventManager& event_manager, Vector2D<int> origin)
{
    const auto global_mouse_pos = event_manager.mouse_pos()-origin;
    
    if (grabbed_)
    {
        if ( event_manager.left_got_unclicked() )
        {
            this->update_big_rect();
            grabbed_.clear();        
        }
        else if ( event_manager.left_is_clicked() && event_manager.mouse_motion() )
        {
            Vector2D<int> new_rect_pos = global_mouse_pos - grabbed_.offset;
            auto& rect = rects_[grabbed_.index];
            auto& point = points_[grabbed_.index];

            rect.set_pos(new_rect_pos);
            point.x = new_rect_pos.x + rect_w_/2.0;
            point.y = new_rect_pos.y + rect_h_/2.0;
             
        }
    }
    else
    {
        if ( event_manager.left_got_clicked() )
        {
            if ( big_rect_.collide_point(global_mouse_pos) )
            {
                for ( std::size_t i = 0; i < rects_.size(); i++ )
                {
                    if ( rects_[i].collide_point(global_mouse_pos) )
                    {
                        auto offset = global_mouse_pos-rects_[i].get_pos();
                        grabbed_.set_new(i, offset );
                        break;
                    }
                }
            }
        }
    }
}

void InteractiveSpline::render(SDL_Renderer* renderer, Vector2D<int> origin) const
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    for ( const auto& rect : rects_ )
    { 
        SDL_Rect sdl_rect{rect.get_x()+origin.x, rect.get_y()+origin.y, rect_w_, rect_h_};
        SDL_RenderFillRect(renderer, &sdl_rect); 
    }

    bool loop = true;
    double dt = 0.02;
    //double limit = loop ? (double)points_.size() : (double)points_.size() - 3.0;
    double limit=(double)points_.size()-(loop ? 0 : 3);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    for ( double t = 0.0; t < limit; t+=dt )
    {
        
        Vector2D<double> p1 = this->get_spline_point(t, loop);
        Vector2D<double> p2 = this->get_spline_point(t+dt, loop);

        p1+=(Vector2D<double>)origin;
        p2+=(Vector2D<double>)origin;

        SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    }
}

void InteractiveSpline::update_big_rect()
{
    if (rects_.empty()) {
        big_rect_ = Rect{0, 0, 0, 0};
        return;
    }

    int min_x = rects_[0].get_x();
    int min_y = rects_[0].get_y();
    int max_x = min_x + rects_[0].get_w();
    int max_y = min_y + rects_[0].get_h();

    for (const auto& rect : rects_) {
        min_x = std::min(min_x, rect.get_x());
        min_y = std::min(min_y, rect.get_y());
        max_x = std::max(max_x, rect.get_x() + rect.get_w());
        max_y = std::max(max_y, rect.get_y() + rect.get_h());
    }

    big_rect_.set_x(min_x);
    big_rect_.set_y(min_y);
    big_rect_.set_w(max_x - min_x);
    big_rect_.set_h(max_y - min_y);
}

void InteractiveSpline::add(Vector2D<double> point)
{
    points_.push_back(point);

    int x = static_cast<int>(point.x-rect_w_/2.0);
    int y = static_cast<int>(point.y-rect_h_/2.0);
    rects_.emplace_back(x, y, rect_w_, rect_h_);

    if ( rects_.size() == 1 )
    { big_rect_ = rects_.back(); }
    else
    {
        int min_x = std::min(big_rect_.get_x(), x);
        int min_y = std::min(big_rect_.get_y(), y);
        int max_x = std::max(big_rect_.get_x() + big_rect_.get_w(), x + rect_w_);
        int max_y = std::max(big_rect_.get_y() + big_rect_.get_h(), y + rect_h_);

        big_rect_.set_x(min_x);
        big_rect_.set_y(min_y);
        big_rect_.set_w(max_x - min_x);
        big_rect_.set_h(max_y - min_y);       
    }
}

void InteractiveSpline::reserve(std::size_t n)
{
    points_.reserve(n);
    rects_.reserve(n);
}

void InteractiveSpline::clear()
{
    points_.clear();
    rects_.clear();
    big_rect_ = Rect{0, 0, 0, 0};
}

void Splines::update(const EventManager& event_manager, Vector2D<int> origin)
{
    if ( event_manager.left_got_clicked() || event_manager.left_got_unclicked() ||
         event_manager.left_got_clicked() || event_manager.left_got_unclicked() ||
         event_manager.mouse_motion()
        )

        {
            for ( auto& spline : splines_ )
            { spline->update(event_manager, origin); }
        }
}

void Splines::render(SDL_Renderer* renderer, Vector2D<int> origin) const
{}