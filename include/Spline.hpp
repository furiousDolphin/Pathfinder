#ifndef SPLINE_HPP_
#define SPLINE_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <vector>
#include <initializer_list>
#include <memory>

#include "Vector2D.hpp"
#include "Rect.hpp"
#include "EventManager.hpp"





class ISpline
{
    public:    
        virtual Vector2D<double> get_spline_point(double t, bool loop = false) const = 0;
        virtual Vector2D<double> get_spline_gradient(double t, bool loop = false) const = 0;
        virtual void update(const EventManager& event_manager, Vector2D<int> origin) = 0;
        virtual void render(SDL_Renderer* renderer, Vector2D<int> origin) const = 0;
        virtual void add(Vector2D<double> point) = 0;
        virtual void reserve(std::size_t n) = 0;
        virtual void clear() = 0;
};

class Spline : public ISpline
{
    public:
        Spline() = default;
        virtual ~Spline() = default;

        Spline(std::initializer_list<Vector2D<double>> init_points):
            points_{init_points}
        {}

        Spline(const Spline& other) = default;
        Spline(Spline&& other) = default;

        Spline& operator=(const Spline& other) =default;
        Spline& operator=(Spline&& other) =default;

        Vector2D<double> get_spline_point(double t, bool loop = false) const override;
        Vector2D<double> get_spline_gradient(double t, bool loop = false) const override;

        void update(const EventManager& event_manager, Vector2D<int> origin) override;
        void render(SDL_Renderer* renderer, Vector2D<int> origin) const override;
        void add(Vector2D<double> point) override { points_.push_back(point); };
        void reserve(std::size_t n) override { points_.reserve(n); };
        void clear() override { points_.clear(); };

    protected:
        std::vector<Vector2D<double>> points_;
};

class InteractiveSpline : public Spline
{
    public:
        InteractiveSpline(std::initializer_list<Vector2D<double>> init_points, int rect_w, int rect_h);

        InteractiveSpline(const InteractiveSpline& other) = default;
        InteractiveSpline(InteractiveSpline&& other) = default;

        InteractiveSpline& operator=(const InteractiveSpline& other) =default;
        InteractiveSpline& operator=(InteractiveSpline&& other) =default;

        void update(const EventManager& event_manager, Vector2D<int> origin);
        void render(SDL_Renderer* renderer, Vector2D<int> origin) const;
        void add(Vector2D<double> point);
        void reserve(std::size_t n);
        void clear();

        void update_big_rect();

    private:

        struct GrabbedState
        {
            std::size_t index = 0;
            bool active = false;
            Vector2D<int> offset;
            explicit operator bool() const { return active; }
            void clear() {active = false;}
            void set_new(std::size_t new_index, Vector2D<int> new_offset) 
            { index = new_index; active=true; offset = new_offset; }

        } grabbed_;

        std::vector<Rect> rects_;
        Rect big_rect_;
        int rect_w_;
        int rect_h_;
};


class Splines
{
    public:
        Splines() = default;
        ~Splines() = default;

        void update(const EventManager& event_manager, Vector2D<int> origin);
        void render(SDL_Renderer* renderer, Vector2D<int> origin) const; 

    private:
        std::vector<std::unique_ptr<ISpline>> splines_;
};

#endif