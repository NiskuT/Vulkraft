#pragma once

#include "gameWindow.hpp"
#include "pipeline.hpp"

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
            pipeline pipeline{"shaders/simple_shaders.vert.spv", "shaders/simple_shaders.frag.spv"};
    };
}