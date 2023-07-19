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

            gameWindow window{WIDTH, HEIGHT, "Vulkraft"}; // we create the window here 
            engineDevice device{window}; // and we initialize the environment for the present device (swap chain for example)
            renderer engineRenderer{window, device}; // initializes the renderer with the window and present device 

            std::shared_ptr<visibleWorld> myWorld;

            // Order of declaration is important here
            std::unique_ptr<engineDescriptorPool> globalPool{};
            std::unique_ptr<engineDescriptorPool> blockPool{};
            gameObject::Map gameObjects; // map of all the gameObjects present in the scene 
            std::unique_ptr<engineTexture> textures; // unique pointer to steve textures    
            std::unique_ptr<engineTexture> blockTexture;  // unique pointer to the block textures 

    };
}