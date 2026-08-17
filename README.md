# NTHU Graphics Programming and Applications

A collection of real-time rendering assignments and a final project developed in C++/OpenGL. The work moves from hierarchical character animation to screen-space post-processing, GPU-driven scene rendering, and a multi-effect deferred-rendering showcase.

## Results

![HW1 grade](https://img.shields.io/badge/HW1-100-brightgreen)
![HW2 grade](https://img.shields.io/badge/HW2-90-green)
![HW3 grade](https://img.shields.io/badge/HW3-100-brightgreen)
![Final grade](https://img.shields.io/badge/Final-92-green)

## Projects

### HW1 — Hierarchical robot

An articulated robot assembled with hierarchical transformations.

- Parent/child transformation stack for connected body parts.
- Mouse-driven camera rotation and zoom.
- WASD movement and a right-click menu for animation playback/pause.
- Built with Visual Studio 2019, OpenGL, FreeGLUT, GLEW, and GLM.

[Report and screenshots](HW1/110062222_AS1_Report.pdf)

### HW2 — Image-space effects

An interactive scene demonstrating fragment-shader and post-processing effects:

- normal textured rendering;
- image abstraction and watercolor stylization;
- magnifier and movable comparison bar;
- bloom, pixelization, and sine-wave distortion.

Use WASD and `Z`/`X` to move, drag the mouse to look around, and right-click to choose an effect.

[Report and screenshots](HW2/110062222_AS2_Report.pdf)

### HW3 — GPU-driven grassland

A large-scene rendering exercise using compute and graphics shaders.

- Grass and bush rendering with dedicated shader programs.
- Compute-shader reset/culling stages.
- View-frustum representation, spatial sampling, and trajectory support.
- Player-view navigation with `W`/`S` and turning with `A`/`D`.

[Report and screenshots](HW3/110062222_AS3_Report.pdf)

### Final — Rendering showcase

The final directory contains indoor and outdoor renderer applications. Together they demonstrate a broader real-time pipeline:

- deferred shading and G-buffer construction;
- shadow mapping, SSAO, FXAA, bloom, NPR, and volumetric lighting;
- model loading and camera control;
- an outdoor terrain scene with instanced vegetation, buildings, an airplane, and a magic-stone effect.

The final project was collaborative; the two scene directories preserve the renderer code and assets used by the team.

## Repository structure

```text
HW1/            Hierarchical robot and animation
HW2/            Post-processing shader experiments
HW3/            GPU-driven grassland scene
Final/Indoor/   Indoor deferred-rendering showcase
Final/Outdoor/  Outdoor terrain and scene renderer
```

## Build and run

The checked-in projects target Windows and Visual Studio. Dependencies such as GLFW/GLAD, GLM, Assimp, ImGui, and model-loading helpers are bundled with the corresponding frameworks.

1. Open the `.sln` file for the desired assignment or final scene.
2. Retarget the Windows SDK if Visual Studio requests it.
3. Build the main application project.
4. Run with the working directory set to the folder containing that project's shaders/assets so relative paths resolve.

Key solutions include:

- `HW1/110062222_AS1/110062222_Robot/110062222_Robot.sln`
- `HW2/110062222_AS2/AS2_Framework/VC14/AS2_Framework.sln`
- `HW3/GPA_Assignment3_Framework/Rendering_Framework.sln`
- `Final/Indoor/Rendering_Framework.sln`
- `Final/Outdoor/Rendering_Framework.sln`

## Notes

- This is a course-project archive. Some large executables or generated build artifacts referenced by the reports may not be present.
- Models, textures, framework code, and libraries from third parties remain subject to their respective licenses.
