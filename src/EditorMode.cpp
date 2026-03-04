
#include "EditorMode.hpp"
#include <algorithm>


EditorMode::EditorMode( AppContext& app_context ):
    Mode            { app_context                      },
    buttons_        {                                  },
    platform_{ 9, 8, Vector2D<int>{0, 0} },
    workers_{},
    spline_{{{100, 100}, {200, 200}, {40, 120}, {90, 230}, {170, 300}, {190, 100}}, 20, 20},
    path_finder_spline_{}
{
    this->create_buttons();



    const std::size_t n = 6;

    std::array<Vector2D<int>, n> workers_coords =
    {{ {1, -1}, {4, 0}, {4, 3}, {3, 4}, {5, 6}, {1, 3}  }};
    
    workers_.reserve(n);
    for ( const auto& worker_coords : workers_coords )
    { workers_.emplace_back(3, 8, worker_coords); }

    for ( const auto& worker : workers_ )
    { platform_.add_worker(worker); }


    Graphs::DijkstraAdjList adj = platform_.get_adjlist();

    Graphs::VertexID start_node{0, 0};
    Graphs::DijkstraResult res = Graphs::dijkstra(start_node, adj);

    

    int k = 9*8;
    Graphs::VertexID target{k-1, k-1};
    std::cout << res.parents.at(target);
    Graphs::Collection path = Graphs::get_path(start_node, target, res.parents);

    path_ = std::move(path);

    // for ( auto& step : path_ )
    // { std::cout << step << "\n"; }

    // std::cout << res.distances.at(target);

    path_finder_spline_.reserve(path_.size());
    for ( const auto& point : path_ )
    { path_finder_spline_.add(Vector2D<double>{point.x*8.0, point.y*8.0}); }
}


void EditorMode::draw_grid() const
{
    int tile_size = 64;

    int x_offset = origin_.x % tile_size;
    int y_offset = origin_.y % tile_size;

    SDL_SetRenderDrawColor( app_context_.renderer, 0x00, 0x00, 0x00, 0xff);

    for (int x = x_offset; x <= WIDTH; x += tile_size) 
    {
        SDL_RenderDrawLine( app_context_.renderer, x, 0, x, HEIGHT);
    }
  
    for (int y = y_offset; y <= HEIGHT; y += tile_size) 
    {
        SDL_RenderDrawLine( app_context_.renderer, 0, y, WIDTH, y);
    }
}

void EditorMode::pan_input()
{
    if( app_context_.event_manager.middle_got_clicked() )
    {
        mouse_origin_vector_ = app_context_.event_manager.mouse_pos() - origin_;
    }

    if( app_context_.event_manager.middle_is_clicked() &&
        app_context_.event_manager.mouse_motion() )
    {
        origin_ = app_context_.event_manager.mouse_pos() - mouse_origin_vector_;
    }
}

void EditorMode::create_buttons()
{
    buttons_.add( 
        std::make_unique<TextButton>( 
            TextButton{ Vector2D<int>{ 20, 20 },
            app_context_.renderer,
            "GAME", 
            { 0x00, 0x00, 0xff }, 
            { 0x00, 0xff, 0x00 }, 
            app_context_.fonts.at( FontTypes::MINECRAFT_24 ).get_font(), 
            [this]() { app_context_.mode_type = ModeType::GAME; } } 
        ) 
    );
}

void EditorMode::update_grid_pos()
{
    auto [ x, y ] = app_context_.event_manager.mouse_pos() - origin_;
      
    grid_pos_.x = static_cast<int>( x/TILE_SIZE ) - ( ( x < 0 ) ? 1 : 0);
    grid_pos_.y = static_cast<int>( y/TILE_SIZE ) - ( ( y < 0 ) ? 1 : 0);
}

void EditorMode::update( float dt )
{
    const auto& mouse_pos  = app_context_.event_manager.mouse_pos();

/*-----------------------------------------------------------------------------------*/

    if( !buttons_.update( app_context_.event_manager ) )
    {}

/*-----------------------------------------------------------------------------------*/

    this->pan_input();
    this->update_grid_pos();  

    spline_.update(app_context_.event_manager, origin_);
}

void EditorMode::render()
{
    SDL_SetRenderDrawColor( app_context_.renderer, 0xff, 0xff, 0xff, 0xff );
    SDL_RenderClear( app_context_.renderer );

    this->draw_grid();
    
    buttons_.render( app_context_.renderer );

    //------------------------------------------------------------------------

    platform_.render(app_context_.renderer, origin_);

    auto w = TILE_SIZE/8;
    auto h = TILE_SIZE/8;
    Rect rect_{ 0, 0, w, h };

    SDL_SetRenderDrawColor(app_context_.renderer, 0xff, 0x00, 0x00, 0xff);
    for ( const auto& step : path_ )
    {
        auto x = step.x*w + origin_.x;
        auto y = step.y*h + origin_.y;

        rect_.set_x(x);
        rect_.set_y(y);


        SDL_RenderFillRect(app_context_.renderer, rect_.sdl_rect_ptr());
    }

    //------------------------------------------------------------------------./cmake-build-debug/main.exe

    path_finder_spline_.render(app_context_.renderer, origin_);
    spline_.render(app_context_.renderer, origin_);

    //------------------------------------------------------------------------

    SDL_RenderPresent( app_context_.renderer );
}

void EditorMode::run( float dt )
{
    this->update( dt );
    this->render();
}


