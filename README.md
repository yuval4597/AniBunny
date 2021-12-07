# AniBunny
*Ani Bunny* is a game I developed because I wanted to bring my girlfriend's favorite childhood stuffed animal to life. My goal was to create a game that will fit the theme of a cute pink bunny, so I decided to make collecting and planting flowers the focus of the game.

Gameplay as well as technical details can be found below.
![Ani Bunny Jumps Off Platform](../visuals/Gifs/PlatformerJump2.gif?raw=true)

## Introduction
![Base Map Well](../visuals/Screenshots/AniBunnyScenery1.png?raw=true)

*Ani Bunny* is a 3D platformer in which the player controls a pink stuffed bunny which came to life in order to collect and plant flowers, and beautify the world she calls home.
There are some flowers scattered in Ani Bunny's base world, but in order to find something truly special, she must travel to the platformer world and collect the rarest flowers of all.

![Gold Flower Collectible](../visuals/Gifs/GoldFlowerCollectible.gif?raw=true)

## Base Level
### Gameplay
*Ani Bunny* is filled with flower collectibles, so the first thing to do is collect them!
![Collecting Flower](../visuals/Gifs/CollectingFlower.gif?raw=true)

The base world allows Ani Bunny to manage her garden by picking and planting flowers in valid locations (land!). In order to plant flowers, the user must press **G** to activate **Gardening Mode**. Then, it is possible to cycle through flowers collected by using **E** and **Q**, and plant the flower by right-clicking after finding a valid spot.
![Planting Flowers](../visuals/Gifs/PlantingFlower.gif?raw=true)

A flower will take some time to grow after planted, depending on the type of flower this can range from 30 seconds to a couple of minutes. If Ani Bunny decides that she doesn't like where she previously planted a flower, all one must do to help her out is enter **Gardening Mode** once more and left-click the out-of-place flower to pick it.
![Picking Flowers](../visuals/Gifs/PickingFlower.gif?raw=true)

### Technical Details
Both the flower collectible and flower blueprints allow for easy game design and customization.

The flower collectible allows the game designer to easily set the type of flower (determined by the selected class), the amount of flowers which will be added to the player's inventory, whether or not this collectible should respawn, and how many seconds until it does respawn (assuming Should Respawn has been set to true).
![Flower Collectible Blueprint Settings](../visuals/Screenshots/CollectibleEditorTools.PNG?raw=true)

The flower blueprint inherits from BPlantable, a class of plantable objects in *Ani Bunny*. It can be set to either never grow or be spawned fully grown. In case it should grow normally, the number of seconds before it is fully grown can be set.
![Flower Blueprint Settings](../visuals/Screenshots/PlantBlueprintSettings.PNG?raw=true)

## Platformer Level

### Gameplay
The platformer level allows Ani Bunny to acquire the heavily sought after golden flower. In this world she can run faster, jump higher (maybe even twice?), and has much more control over her movement in the air.
![Platformer Spawn Scenery](../visuals/Screenshots/AniBunnyScenery4.png?raw=true)

In order to reach the golden flower, she must first navigate across the moving platforms safely, as if she falls into the water she will have to retry from the beginning of the level.
![First Platform Jump](../visuals/Gifs/PlatformerJump1.gif?raw=true)

### Technical Details
The platforms are setup to be easily placed in the level by using blueprints with several parameters to quickly adjust their behavior:
![Platform Blueprint Settings](../visuals/Gifs/PlatformerEditorTools.gif?raw=true)

As one might expect, **Period** determines the amount of time it takes the platform to travel from side to side, and **Move Right First** determines the direction in which the platform starts moving when it is spawned.
The settings for the rotating platforms are similar to the ones mentioned above.

## Credits
While Ani Bunny and her animations were created by myself, I have used assets from the following packs when designing the Base and Platformer worlds:

[Advanced Village Pack by *Advanced Asset Packs*](https://www.unrealengine.com/marketplace/en-US/product/advanced-village-pack)

[Platformer Starter Pack by *Platfunner*](https://www.unrealengine.com/marketplace/en-US/product/platformer-starter-pack)

## Extra Screenshots
![Base Map View](../visuals/Screenshots/AniBunnyScenery2.png?raw=true)
![Base Map Path](../visuals/Screenshots/AniBunnyScenery3.png?raw=true)
![Platformer Map Moving Platforms](../visuals/Screenshots/AniBunnyScenery5.png?raw=true)
![Platformer Map Rotating Platforms](../visuals/Screenshots/AniBunnyScenery6.png?raw=true)
