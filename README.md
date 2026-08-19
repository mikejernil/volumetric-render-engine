# Volumetric Render Engine

A Volumetric Render Engine 🧊 for Windows (Native). It's being built with OpenGL & C++ 💻 and you can try it out and integrate it into your own applications.

> [!NOTE]
> This repo is at it's very early stages of development and its an OpenGL Volumetric Demo, it is not a fully-featured Engine yet. We are planning to build more features like custom file loading, more volumetric FX & additional formats support (like DICOM, VDB etc) soon!


>[!Input Volume Data]
> Current output is of Engine256.raw which is a sample volume dataset of scalar values (unsigned byte) in voxel grid of 256*256*256.It has no header.Dataset is acquired by CT scan of small engine block.Commonly used in simple volume rendering tutorials. You can try other datasets from the /resources/models/ directory.

<img width="600" height="380" alt="volume_engine_day2_post (1)" src="https://github.com/user-attachments/assets/74f362ae-9ff0-4ba9-8ad8-d7cbb7f8eda0" />

Currently it has -
- Volumetric RAW format support 
- Different types of rendering (Colormap, Iso-surface etc.)
- Rotate & Zoom Controls (for easy navigation) 
- 6 slicing planes to visualize cross-sections


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

**Marching Tetrahedra Method**:
This is an algorithm in which we  can Render our Volume data as a set of Polygon meshes by extracting  'iso surface'.
It goes through whole dataset and tries to fit a polygon based on data values to calculate a polygonal mesh from the volume dataset.
It is called 'Marching Tetrahedra' because the cube is split in half three times, along diagonals which forms six irregular tetrahedra shapes. Here the intersection of the edges along isosurface is used for further calculation of grid points
 

**Applications** 
1. Medical imaging
2. Industrial machinery testing
3. Scientific visualization of data
