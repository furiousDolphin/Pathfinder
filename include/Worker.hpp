
#ifndef WORKER_HPP_
#define WORKER_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <unordered_map>
#include <set>

#include "Settings.hpp"
#include "MatLab.hpp"
#include "Vector2D.hpp"
#include "Graphs.hpp"



class Worker
{
    public:
        Worker(std::size_t n, std::size_t m, Vector2D<int> grid_pos, std::function<double(double)> func = nullptr);

        void update();
        void render(SDL_Renderer* renderer, const Vector2D<int>& origin_ ) const;
        Rect get_rect() const
        { return rect_; }
        const MatLab::Matrix<double>& get_Z_matrix() const 
        { return Z; }

    private:
        MatLab::Matrix<int> X;
        MatLab::Matrix<int> Y;
        MatLab::Matrix<double> Z;
        std::size_t k_;
        Rect rect_;
};


class Platform
{
    public:
        Platform(std::size_t n, std::size_t m, Vector2D<int> grid_pos);

        void update();
        void render(SDL_Renderer* renderer, const Vector2D<int>& origin_ ) const;
        void add_worker(const Worker& worker) const;
        const MatLab::Matrix<double>& get_Z_matrix() const 
        { return Z; }
        Graphs::DijkstraAdjList get_adjlist();

    private:
        MatLab::Matrix<int> X;
        MatLab::Matrix<int> Y;
        MatLab::Matrix<double> Z;
        std::size_t k_;
        Rect rect_;

        static inline std::array<Vector2D<int>, 8> neighbors_ = 
        {{ 
            {-1, -1}, { 0, -1}, { 1, -1},
            {-1,  0},           { 1,  0},
            {-1,  1}, { 0,  1}, { 1,  1}
        }};
};


MatLab::Matrix<double> rect_rect_intersection_matrix_view
(
    const MatLab::Matrix<double>& matrix, 
    const Rect& rect, const Rect& 
    intersection_rect
);

inline constexpr double SQRT2 = 1.414213562373095;

Vector2D<double> get_spline_point(double t);

#endif