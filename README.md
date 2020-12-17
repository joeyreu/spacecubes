# Final Project: CS488 - Graphics
This repository contains code for compiling and running the final project I worked on for the class **CS 488/688: Introduction to Computer Graphics**. This is a 3D spaceship-asteroid game which is unfinished due to poor time management while I was taking the course. View the **report.pdf** file for more details. 

---

## Dependencies
* OpenGL 3.2+
* GLFW
    * http://www.glfw.org/
* Lua
    * http://www.lua.org/
* Premake4
    * https://github.com/premake/premake-4.x/wiki
    * http://premake.github.io/download.html
* GLM
    * http://glm.g-truc.net/0.9.7/index.html
* ImGui
    * https://github.com/ocornut/imgui


---

## Building The Dependencies
We used **premake4** as our cross-platform build system. First you will need to build all
the static libraries that the projects depend on. To build the libraries, open up a
terminal, and **cd** to the top level of the root project directory and then run the
following:

    $ premake4 gmake
    $ make

This will build the following static libraries, and place them in the top level **lib**
folder the project directory.
* libcs488-framework.a
* libglfw3.a
* libimgui.a


--- 

## Building The Project
Next we can build the project

    $ cd Project_Jrreusin/
    $ premake4 gmake
    $ make

To Run the project

    $ ./A5 Assets/spaceship.lua