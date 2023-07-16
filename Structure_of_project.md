# structure of the project (by Chloé)

main -> app

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
