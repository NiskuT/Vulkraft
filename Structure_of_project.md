# structure of the project

## general notes :
* ```#pragma once ``` ensures the header file is included only once during compilation to prevent duplicate definitions.

* Swap chain in Vulkan : to manage the presentation of images onto the screen 
Rendering involves creating a series of images=frames to present the on the screen -> directly render them can be inefficient -> need of swap chain as intermediary between the application and the device. 
The swap chain helps synchronizing the rendering process with the display refresh rate.

* Descriptor in Vulkan : resource accessed by the shaders during rendering process to connect shaders variables with corresponding resources (lights, textures...)
Descriptors are in sets, containing >=1 bindings, specifying a specific resource -> unique binding index with particular type of resource 
A descriptor pool manages the allocation and deallocation of the sets.
During rendering, shaders can access descriptors and allow them to read from or write to resources like buffers or textures during rendering pipeline
## general use of libraries : 

- <tiny_obj_loader.h> : allows to load 3D models from .obj
- <glm/gtx/hash.hpp> : to include hashing functions -> used for example in the engine Model, to create a seed for the generation of the world 
- <cassert> : allows to use the 'assert' function to display error messages after a test 


## main 
-> app

## app : 

-> gameWindow : create the window 
-> engineDevice : ?
-> renderer : ? 
-> descriptors -> engineDescriptorPool : ? 
-> gameObjects
-> pointLightSystem 
-> camera 
-> keyboardController

- run()
- loadGameObjects()

## gameObject : 

--> makePointLight ()


## pointLightSystem :

- constructor & destructor
- creation of pipeline & layout : call of shaders of point light
- update of the pointLightSystem
    - rotate 
    - translate
    - color 
- render of the pointLightSystem
    - sorting of lights (?) 
    - position
    - color
    - radius

## camera : 

- proj matrix 
- view matrix : updated from the hpp with the set view ()
- view inverse matrix

- set orthographic projection : parallel proj
- set perspective projection : perspective proj
- set view direction : view matrix 'look in direction' 
- set view target: view matrix 'look at'
- set view yxz : transformations (translation and rotation) on the camera to move it in the scene
- get proj matrix 
- get view matrix 
- get view inverse matrix 
- get position 


## keyboardController

Defines the key mappings and the associated transformations :
- used for 
    - the camera 
    - the player 

- moveInPlaneXZ () : move the player in the plane
- computePosCam () : move the camera depending on the player position and first/third person view 
## frame info 

- struct PointLight : position & color 
- struct GlobalUbo : mat projection, mat view, mat viewInverse, vec ambientLightColor, pointLights[], numLights
- struct FrameInfo : frameIndex, frameTime, commandeBuffer, camera, globalDescriptorSet, gameObjects


struct texture line 207
enum 271
struct line 273
base project -> class texture ine 300
textures in pool line 324
createIae line 1153 
textures inpool line 1457
create texture iage () line 2281
createtextureimageview() line 2349
createtexturesampler() line 2358
texture init line 2387
texture init cubic line 2397


## engineModel 

