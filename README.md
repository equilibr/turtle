# Turtle
A simple turtle-graphics based framework to aid in teaching programming.

It requieres the OpenSceneGraph library to compile and work.
On Windows the library should be installed, relative to the project file, at ../../libraries/OpenSceneGraph/install
In case it is located at a different place the appropriate lines in the project file should be modified.

The program is build in a way as to allow the student modifiable code, normally located in the file main.cpp, in a function named "MainBrain" to be executed sequentially, with all I/O calls blocking the functions' execution, while allowing the main program to keep running in its own thread.
This allows the main program to remain responsive and to show live data while not requiering the student to handle, or even being aware of, the underying synchronization machinery.