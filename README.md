# Overview

This was my early practical project with **OpenGL** to create practical software application, written from ground up using raw C++, including some of **container objects**.

Many **bugs** are not fixed, **memory leak** exists. It's also tested only on **Windows 8** and **Windows 10** with **my own Lenovo Y480 laptop** and **i7 4970, GTX 970, Asus Z97-A PC** environment. Some advanced rendering techniques like **Shadow Mapping** and **Parallel Mapping** are implemented but in terms of **OpenGL API**, advanced extensions are not used. I think this project in its present state should only be considered as a more **complete OpenGL example**, showcasing how it can be used to **build an actual application**, rather than a **fully-fledged application** in itself.

## Dependencies

1. **DevIL**: Image loading
2. **AssImp**: 3D object import
3. **Glew**: OpenGL extension loading
4. **Glfw**: Window creation and management

# Installation/Compilation

For executables, check out [release](https://github.com/szinubuntu/NodeEditor/releases) page.

For compilation:

1. Install **VS2013** or above
2. Set **DreamEditor** as *startup project*
3. Compile and run, no extra configuration is needed

# Features (as a "quality" rather than "marketing value")

By and large, this is an **experimental** **personal project** intended to be just **functional** - nothing **production-level**. However for the **scope** of the project, I think it's rather complete in terms of presentation, with its **user manual** and everything.

1. **OpenGL** based, thus possible to run on different OS, although system specific functions need reimplementation
2. **Native Unicode** support, tested on windows only
3. **Phong shader** with **parallax mapping** support; **Dynamic scene streaming**
4. Application-Independant Reusable components: **BMFont** loader, Simple-to-use C Style **Specification file** parser