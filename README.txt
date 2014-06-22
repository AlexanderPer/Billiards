Billiards
=========

The MIT License
Copyright (c) 2014 Alexander Perepelitsa

Dynamics of a billiard ball and ray tracing.
Scene contains a room, a pool table and a rolling billiard ball.
Developed using OpenGL, GLSL and ray marching technique.
See shaders\frag.glsl (fragment shader) file.

Dependencies
------------
freeglut, glew, glm (include and lib folders must be extracted from Lib.zip to Billiards folder)


Application launch
------------------
Billiards.exe - default mode
Billiards.exe a - antialiasing mode


Application control
-------------------
Camera control keys: 
'a' - move left,
'd' - move right,
'w' - to move forward,
's' - going backwards,
'PgUp' - upward movement,
'PgDn' - a downward movement.

Clicking the left mouse button - the strike.


System Requirements
-------------------

PC test configuration

Laptop: ASUS U41SV 
Processor: Intel i5-2430M 
Video Card: NVIDIA GeForce 540M 
Memory: 8GB
OS: Windows 7 64-bit build 7601 [Service Pack 1]
IDE: Microsoft Visual Studio 2012

OpenGL configuration

Driver - 9.17.10.3347 (1-29-2014) - GL:ig4icd64.dll
OpenGL - OpenGL 4.3 (GeForce GT 540M/PCIe/SSE2 with 325 ext.)
GL_RENDERER - GeForce GT 540M/PCIe/SSE2
GL_VERSION - 4.3.0
GLSL version - 4.30 NVIDIA via Cg compiler
