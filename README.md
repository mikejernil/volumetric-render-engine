# Volumetric Render Engine

A Volumetric Render Engine 🧊 for Windows (Native). It's being built with OpenGL & C++ 💻 and you can try it out and integrate it into your own applications. ✨

Currently it has -
- Volumetric RAW format support 📺
- Different types of rendering (Colormap, Iso-surface etc.)
- Rotate & Zoom Controls (for easy navigation) 
- 6 slicing planes to visualize cross-sections

We are planning to build more features and add support for more volumetric formats (like DICOM, VDB etc) soon!

### How to Run
- Copy glew32.dll (OpenGL Prerequisite extension) from Project folder `dependencies\glew\bin\Release\x64\glew32.dll` to your `Windows/System32/` folder 
- Start Visual Studio 2022 x64 developer command prompt (Install VS 2022 or higher if not installed already)
- Go to Project directory, and run `build.bat`


<img width="2220" height="2550" alt="NewCode (3) (1)" src="https://github.com/user-attachments/assets/60c84719-1027-407e-a8b8-00c2193aa2ba" />

### Rendering Effects ##


**Colormap Classification** (Our Default):
Here we can see the volume data with color values mapped to its density.
As per our current Colormap we can see the Red is Higher density whereas blue is Low density Noise.

**Basic Method (3D Texture Slicing)**:
It is one of the simplest method in which we can generate a 3D Texture from Volume Data.
It uses volume data values to approximate slices of data and make 3D Texture of it.

**Ray Casting Method**:
Ray Casting is using a virtual light ray which marches through volume data to find and map the scalar value we need to visualize.
We are using single pass GPU Ray casting.

**Pseudo Iso Surface Rendering using Ray Casting**:
This gives our volumetric data an approximate Surface without having to extract polygonal surface .
Its done using Ray Casting method which visualizes our objects/volumes  by traversing rays through data.
Iso Surface value helps us set filter as per Density and remove Noise.

**Applications** 
1. Medical imaging
2. Industrial machinery testing
3. Scientific visualization of data
