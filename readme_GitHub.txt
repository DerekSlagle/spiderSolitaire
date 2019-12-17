https://github.com/pfloyd123/spiderGame

readme:

spiderGame

Spider solitaire game similar to xp version is the main project. Particle effects are developed on extra levels to create the fireworks-on-win game finish. Generic card animation methods are developed on level 2 for integration into the specific game on level 1. Particle effects on levels 3 and 4 go far beyond the fireworks effect. Level 3: Localized particle animations, using central forces such as springs or inverse square forces + drag, gravity and a "curling" force acting perpendicular to the motion. A force map is made and the animation is "boxed" for use elsewhere.

Level 4 develops collision detection and response for particles vs. any 2d curve. Curves can be in motion. Velocity addition is (I think) correct. A graphFuncs class has function pointer members which can be assigned to graph any curve. Cartesian and polar functions are used. There are several working examples of both types in the current version. The function and its 1st derivative must be given so the surface normal may be found for collision handling. All graphFuncs can be rotated, scaled or translated. They can be dragged around by grabbbing near the curve center. Custom control classes are used throughout for user input.

Level 5 adds rotaional motion to some collider types (mvHit=base type) I've developed elsewhere. mvHit collisions with graphFunc objects also is developed here.