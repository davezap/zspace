
# zspace : Can you Z the Space?

This may be hard to believe but once upon a time 3D graphics engines rendered
everything on a single CPU core and a single thread. Dedicated graphics cards, 
where the "boring stuff" could be offloaded, did not see widespread adoption
until the early 2000's.

Click image to see video.
[![Watch the video](https://img.youtube.com/vi/HjbRNnHziek/maxresdefault.jpg)](https://youtu.be/HjbRNnHziek)

zspace is a software 3d engine that I worked on in 2003. It supports
- Wavefront OBJ models (previously .x)
- 24bit textures up to 256x256 as BMPs
- Z-buffered perspective corrected texturing with basic normal shading
- polygon backface and hidden object culling optimisations.
- 3D sprites with alpha transparency and animation.
- rudimentary particle effects



# Changes #

This code sat broken for many years due to DirectDraw related shenanigans. When
I decided to archive it to Github I needed to first get it working and fix some
of the most egregious coding mistakes of my past.

It has now been ported from DirectDraw to [SDL3](https://github.com/libsdl-org/SDL).
SDL is only providing us a surface to draw pixels onto, and keyboard interaction.

The old code used XLoader to read the Microsoft .X mesh files, so I replaced 
that with the [DirectXMesh](https://github.com/microsoft/DirectXMesh) library.

The winmm.lib library was removed as time is now provided by SDL.

With my now Ryzon 9 3900 having a single core performance 500% faster, than the Pentium D of the day, it hardly seems fair I spent so much time trying to optomize the code. Now I have to cap it at 60fps.

# What's next? #

Probably nothing! aside from a little tinkering.


# Download and Install #

Follow the link to download the working demo, includes X64 binary's and required
assets. Just extract files to a folder and run the executable.
[downloaded here](https://204am.com/downloads/zspace_2025.zip) md5:d5f0e7a57b889f0abad21af9b3c08676

This project requires the Microsoft Visual C++ Redistributable [downloaded here](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170) 

# Controls # 

keyboard

- Use arrow keys to move around.
- Space : Shoot rainbow pom poms
- C : Shoot rainbow pom poms in all directions
- S, A : Move camera in and out
- N, M : Change camera angle X
- G, H : Change camera angle y
- J, K : Change camera angle z


# Compiling #

The project is built with Visual Studio 2022, and requires you to build [SDL3](https://github.com/libsdl-org/SDL) and [DirectXMesh](https://github.com/microsoft/DirectXMesh). The project 

The project assumes you have put them both in C:\Libs\SDL\ and C:\Libs\DirectXMesh-main\ respectively.

Follow the SDL docs\README.visualc.md for build instructions.

DirectXMesh lib is simpler, just open the DirectXMesh_Desktop_2022.sln, and build the DirectXMesh branch.

Add C:\Libs\SDL\VisualC\x64\Release to your environment variables or copy the 
SDL3.dll to the project folder either works.

Assuming it can find both of those my project "should" build and run just fine.

# Release # 

After doing a release build you just need to ship the executable with the release
build of C:\Libs\SDL\VisualC\x64\Release\SDL3.dll and the assets folder.


# Contents #

Main.cpp is the entrypoint for SDL and the main game loop.

math.h is the vector math and lookup tables.

LX.cpp is my hacked replacement for XLoader.

engine.cpp is where the rendering happens.

Meshconvert - So turns out the only file DirectXMesh does not support is the .X file that I needed it to support. So what I did was convert them all to Wavefront OBJ's.
Then to get to the internals of Mechconvert I commented out private in Mesh.h to
make those members public.

## The assets folder ##
Contains the meshes and textures for the demo. I found these
online and have no idea where I got them now, if they are your's I'll take them
down on request.

## The docs folder ##
Contains the article explaining perspective corrected texture mapping by posterity. Tom Hammerslevs now works at EA (assuming it's the same person) - Thanks Tom :)

The original source code for this project is world.h and the creatively titled Project_One.cpp

# Contact # 

I can be contacted at davez@204am.com