#include "GameMode.hpp"

GameMode::GameMode( AppContext& app_context ):
    Mode{ app_context }
{
    this->create_buttons();
}

void GameMode::create_buttons()
{
    buttons_.add( 
        std::make_unique<TextButton>( 
            TextButton{ 
                Vector2D<int>{ 20, 20 },
                app_context_.renderer,
                "EDITOR", 
                { 0x00, 0x00, 0xff }, 
                { 0x00, 0xff, 0x00 }, 
                app_context_.fonts.at( FontTypes::MINECRAFT_24 ).get_font(), 
                [this]() { app_context_.mode_type = ModeType::EDITOR; } 
            } 
        ) 
    );
}

void GameMode::update()
{
    buttons_.update( app_context_.event_manager );
}

void GameMode::render()
{
    SDL_SetRenderDrawColor( app_context_.renderer, 0x00, 0x00, 0x00, 0xff );
    SDL_RenderClear( app_context_.renderer );

    buttons_.render( app_context_.renderer );

    SDL_RenderPresent( app_context_.renderer );
}

void GameMode::run( float dt )
{
    this->update();
    this->render();
}