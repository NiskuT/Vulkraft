#pragma once

#include "gameWindow.hpp"
#include "engineDevice.hpp"
#include "gameObject.hpp"
#include "renderer.hpp"


#include <memory>
#include <vector>

namespace engine 
{
    class app
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            app();
            ~app();

            app(const app&) = delete;
            void operator=(const app&) = delete;

            void run();

        private:
            void loadGameObjects();

            gameWindow window{WIDTH, HEIGHT, "Vulkraft"};
            engineDevice device{window};
            renderer engineRenderer{window, device};
            std::vector<gameObject> gameObjects;          
    };
}