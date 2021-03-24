# Graphics Assignment 0
---
# Instructions
* Required libraries: 
    * glad 
    * glfw 
    * glm 
* Make a separate directory named 'libraries' outside the source directory and put glad and glfw inside it. Put glm in the includes (usr/includes) folder
* To run the code:
    ```
    mkdir build
    cd build
    cmake ..
    make
    ./window
    ```
    Select any of the following three shapes:
    1. Elongated Square Pyramid
    2. Decagonal Prism
    3. Hexagonal Dipyramid
* Code to calculate the coordinates of vertices of different shapes is present in [shapes](shapes).
# Controls
|          Effect                         |   Keyboard input    |          
|:---------------------------------------:|:-------------------:|
| Move camera around                      | W/S/A/D/Q/E         |
| Move object around                      | I/K/J/L/U/O         |
| Make camera spin around object          | R                   |
| Make object spin about its local axis   | Z/X/C               |
| Move camera to fixed positions          | B/N/M               |
| Exit                                    | Esc                 |

Note: 
- On pressing R, camera spins aroung the object in a circle of radius 3. If camera is far away, then it is first moved closer to the object to a distance of 3.
- W/S moves camera along z axis. A/D moves camera along x axis. Q/E moves camera along y axis.
- I/K moves object along z axis. J/L moves object along x axis. U/O moves object along y axis.

