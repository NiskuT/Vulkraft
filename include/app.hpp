#pragma once

#include "gameWindow.hpp"
#include "engineDevice.hpp"
#include "gameObject.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"
#include "engineTexture.hpp"
#include "visibleWorld.hpp"


#include <memory>
#include <vector>

namespace engine 
{
    class app
    {
        public:
            // window's size 
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            app(); // constructor 
            ~app(); // destructor 

            app(const app&) = delete;
            void operator=(const app&) = delete;

            void run();

        private:
            void loadGameObjects();
            void loadTextures();

            gameWindow window{WIDTH, HEIGHT, "Vulkraft"};
            engineDevice device{window};
            renderer engineRenderer{window, device};

            std::shared_ptr<visibleWorld> myWorld;

            // Order of declaration is important here
            std::unique_ptr<engineDescriptorPool> globalPool{};
            std::unique_ptr<engineDescriptorPool> blockPool{};
            gameObject::Map gameObjects;
            std::unique_ptr<engineTexture> textures;
            std::unique_ptr<engineTexture> blockTexture;      

    };
}