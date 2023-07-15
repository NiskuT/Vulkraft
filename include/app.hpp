#pragma once

#include "gameWindow.hpp"
#include "engineDevice.hpp"
#include "gameObject.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"
#include "engineTexture.hpp"


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
            void loadTextures();

            gameWindow window{WIDTH, HEIGHT, "Vulkraft"};
            engineDevice device{window};
            renderer engineRenderer{window, device};

            // Order of declaration is important here
            std::unique_ptr<engineDescriptorPool> globalPool{};
            gameObject::Map gameObjects;
            std::unique_ptr<engineTexture> textures;      

    };
}