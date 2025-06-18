# OpenGL Programming - a small artificial solar system

[🎬 Watch the demo video](https://github.com/llada60/Artificial-Solar-System/raw/refs/heads/main/media/result.mp4)

![OpenGL Result](media/opengl_result.png)

---

**CSC_4IG01_TP** Interactive 3D Application Development, Fall 2024

---

Implement a solar system with OpenGL, using the following libraries:
- **OpenGL**: accessing the graphics processor.
- **GLM**: interface OpenGL and the window system of the operating system.
- **GLFW**: basic mathematical tools (vectors, matrices, etc.).

## File Structure

```
.
├── dep
├── glm
├── media                                       # Textures for different spheres
├── shader/                                     # Contains different shading implementations
│   └── Blinn-Phong/
│       ├── Blinn-PhongFragmentShader.glsl               
│       └── Blinn-PhongVertexShader.glsl
│   └── Phong/
│       ├── PhongFragmentShader.glsl             
│       └── PhongVertexShader.glsl
│   └── Gouraud/
│       ├── GouraudShadingFragmentShader.glsl
│       └── GouraudShadingVertexShader.glsl
├── CMakeLists.txt
├── main.cpp
└── README.md                                   # This file
```

## Running the code

The code uses `cmake` as a build system. Run the following commands in the terminal to build and run the project:

```
> cmake -B build    # under the source directory where the CMakeLists.txt is located
> make -C build
> ./tpOpenGL        # it will try to load ./fragmentShader.glsl, ./vertexShader.glsl and ./media/*.jpg
```

This code implement 3 shading methods, including Phong Shading, Gouraud shading and Blinn-Phong model. You can change the following [two lines](https://github.com/llada60/Artificial-Solar-System/blob/0c3b34b05653cdca0f1230ef82a5e01a0471d1c8/main.cpp#L500) with corresponding shading `glsl` files.
