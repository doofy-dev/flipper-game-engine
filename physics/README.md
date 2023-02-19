<p align="center"><img src="https://raw.githubusercontent.com/kadir014/kadir014.github.io/master/assets/novaphysics.png" width=340></p>

<p align="center">
Slightly modified version of Nova physics.
</p>

[Original repo](https://github.com/kadir014/nova-physics)


## Features
- Simple and user-friendly interface
- Rigid body dynamics
- Convex polygon and circle collisions
- Friction and restitution
- Collision callbacks
- Sleeping
- Attractive forces
- Collision callbacks
- Portable codebase with no dependencies

## Future
#### Faster broad-phase with Bounding Volume Hierarchy
Current broad-phase strategy is to check AABB collisions between bodies to narrow down expensive polygon checks. But this aproach is still far away from efficient. That's why I'll be working on a BVH based tree system for broad-phase.

#### Stability & Stacking
Other than a faster broad-phase, I will be working on simulation stability and general optimization. Methods like accumulating impulses accross frames, warm-starting, coherence... ([Erin Catto has amazing slides about these topics from GDC](https://box2d.org/files/ErinCatto_SequentialImpulses_GDC2006.pdf))

#### Codebase & Structure
Improve the codebase, there are a few duplicates of array structures that can be merged into one generic array type. Improving the codebase with better docs will help contributors as well.

## Resources & References
- **Erin Catto**, GDC Slides (https://box2d.org/publications/)
- **Chris Hecker**, Rigid Body Dynamics (https://chrishecker.com/Rigid_Body_Dynamics)
- **Randy Gaul**, Game Physics Articles (https://randygaul.github.io/)

## License
[MIT](LICENSE) © Kadir Aksoy
