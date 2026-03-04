

#include "Worker.hpp"
#include <algorithm>
#include <numeric>


Worker::Worker(std::size_t n, std::size_t m, Vector2D<int> grid_pos, std::function<double(double)> func):
    k_{n*m},
    rect_{grid_pos*TILE_SIZE, TILE_SIZE*static_cast<int>(n), TILE_SIZE*static_cast<int>(n)}
{
    double r = 7.0;
    double start = 2.0*r/(k_-1) - r;
    double stop = -start;

    auto [ mXd, mYd ] = MatLab::meshgrid<double>(k_, k_, start, stop);

    double a = 3.5;
    double b = 10.0;
    double max = 0.0;

    const auto& system_exp = 
    [&a, &b, &max, &r](double r2) 
    { 
        if ( r2 > r*r/2 )
        { return 0.0; }

        auto res = a*std::exp(-r2/b); 
        max = std::max(res, max); 
        return res; 
    };

    Z = (mXd*mXd+mYd*mYd);
    Z.apply_inplace(system_exp);

    auto [ mXi, mYi ] = MatLab::meshgrid<int>(k_, k_, 0, static_cast<int>(k_) - 1);
    X = std::move(mXi);
    Y = std::move(mYi);

    X += grid_pos.x*m;
    Y += grid_pos.y*m;
}

void Worker::update()
{}

void Worker::render(SDL_Renderer* renderer, const Vector2D<int>& origin_) const
{
    double step_w = static_cast<double>(rect_.get_w()) / k_;
    double step_h = static_cast<double>(rect_.get_h()) / k_;
    SDL_Rect sdl_rect;
    sdl_rect.w = static_cast<int>(std::ceil(step_w)); 
    sdl_rect.h = static_cast<int>(std::ceil(step_h));

    for (std::size_t i = 0; i < k_; i++) {
        for (std::size_t j = 0; j < k_; j++) {
            
            sdl_rect.x = static_cast<int>(rect_.get_x() + j * step_w + origin_.x);
            sdl_rect.y = static_cast<int>(rect_.get_y() + i * step_h + origin_.y);

            double val = Z(i, j);
            Uint8 color = static_cast<Uint8>(std::clamp((1.0 - val) * 255.0, 0.0, 255.0));

            SDL_SetRenderDrawColor(renderer, color, color, color, 0xff);
            SDL_RenderFillRect(renderer, &sdl_rect);
        }
    }
}



Platform::Platform(std::size_t n, std::size_t m, Vector2D<int> grid_pos):
    k_{n*m},
    rect_{grid_pos*TILE_SIZE, TILE_SIZE*static_cast<int>(n), TILE_SIZE*static_cast<int>(n)},
    Z{n*m, n*m}
{
    Z.apply_inplace( []( double elem ){ return 1.0; } );
}

void Platform::update()
{}

void Platform::render(SDL_Renderer* renderer, const Vector2D<int>& origin_) const {
    double step_w = static_cast<double>(rect_.get_w()) / k_;
    double step_h = static_cast<double>(rect_.get_h()) / k_;
    SDL_Rect sdl_rect;
    sdl_rect.w = static_cast<int>(std::ceil(step_w)); 
    sdl_rect.h = static_cast<int>(std::ceil(step_h));

    for (std::size_t i = 0; i < k_; i++) {
        for (std::size_t j = 0; j < k_; j++) {
            
            sdl_rect.x = static_cast<int>(rect_.get_x() + j * step_w + origin_.x);
            sdl_rect.y = static_cast<int>(rect_.get_y() + i * step_h + origin_.y);

            double max_val = 5.0;
            double val = Z(i, j);

            Uint8 color = static_cast<Uint8>(std::clamp(255.0*(max_val - val)/max_val, 0.0, 255.0)); 

            SDL_SetRenderDrawColor(renderer, color, color, color, 0xff);
            SDL_RenderFillRect(renderer, &sdl_rect);
        }
    }
}

void Platform::add_worker(const Worker& worker) const
{
    auto worker_rect = worker.get_rect();
    const auto& worker_Z = worker.get_Z_matrix();
    const auto& platform_rect = rect_;
    const auto& platform_Z = Z;

    Rect intersection_rect{};

    if ( platform_rect.get_rect_intersection(worker_rect, intersection_rect) )
    {
        auto platform_view = rect_rect_intersection_matrix_view( platform_Z, platform_rect, intersection_rect );
        auto worker_view   = rect_rect_intersection_matrix_view( worker_Z,   worker_rect,   intersection_rect );

        platform_view+=worker_view;
    }
}


Graphs::DijkstraAdjList Platform::get_adjlist()
{
    Graphs::DijkstraAdjList adj;
    adj.reserve(k_*k_);

    for ( int i = 0; i < static_cast<int>(k_); i++ )
    {
        for ( int j = 0; j < static_cast<int>(k_); j++ )
        {
            std::vector<std::pair<Graphs::VertexID, double>> v;
            v.reserve(neighbors_.size());

            for (const auto& neighbor : neighbors_) 
            {
                int ni = i + neighbor.y;
                int nj = j + neighbor.x;

                if (ni >= 0 && ni < k_ && nj >= 0 && nj < k_) 
                {
                    double cost   = Z(i, j);   

                    if (neighbor.x != 0 && neighbor.y != 0) 
                    { cost *= SQRT2; }
                    v.emplace_back(Graphs::VertexID{nj, ni}, cost);
                }
            }

            adj.emplace(Graphs::VertexID{j, i}, std::move(v));
        }
    }

    return adj;
}

MatLab::Matrix<double> rect_rect_intersection_matrix_view
(
    const MatLab::Matrix<double>& matrix, 
    const Rect& rect, 
    const Rect& intersection_rect
)
{
    auto [x1, y1] = rect.get_pos();
    auto [w1, h1] = rect.get_size();
    auto [x2, y2] = intersection_rect.get_pos();
    auto [w2, h2] = intersection_rect.get_size();

    auto [rows1, cols1] = matrix.size();

    std::size_t start_row = static_cast<std::size_t>(std::round(static_cast<double>(rows1) * (y2 - y1) / h1));
    std::size_t start_col = static_cast<std::size_t>(std::round(static_cast<double>(cols1) * (x2 - x1) / w1));
    
    std::size_t rows2 = static_cast<std::size_t>(std::round(static_cast<double>(rows1) * h2 / h1));
    std::size_t cols2 = static_cast<std::size_t>(std::round(static_cast<double>(cols1) * w2 / w1));


    if (start_row + rows2 > rows1) rows2 = rows1 - start_row;
    if (start_col + cols2 > cols1) cols2 = cols1 - start_col;

    return matrix.slice(start_row, rows2, start_col, cols2);
}

