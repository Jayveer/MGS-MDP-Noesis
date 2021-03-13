# Metal Gear Solid: Peace Walker Noesis Plugin


This is a plugin for [Noesis](https://richwhitehouse.com/index.php?content=inc_projects.php&showproject=91) which allows the user to view textured 3D Models and animations from the game Metal Gear Solid: Peace Walker. There is currently a specific type of model format without a vertex buffer which still requires research. An error will be shown when you encounter a file of this type.


![picture](https://github.com/Jayveer/MGS-MDP-Noesis/blob/master/model.png?raw=true)

### Latest Changes
 - Fixed a problem where textures weren't auto applying on export
 - Fixed winding order

### To Do
 - Reasearch how model files with no vertex buffer are read.

##  Usage.

Drag the dll file into the plugins folder of your Noesis folder, run noesis and find and locate the MDP file you wish to view. Textures will be applied if the associated TXP file can be found. If you wish to view animations In the menu select 'Prompt for Motion Archive' which will allow you to choose an Mtar file after the model has loaded.

![picture](https://github.com/Jayveer/MGS-MDP-Noesis/blob/master/open_anims.png?raw=true)
