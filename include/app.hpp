#pragma once

#include "gameWindow.hpp"

namespace engine 
{
    class app
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();

        private:
            gameWindow window{WIDTH, HEIGHT, "Vulkraft"};
    };
}