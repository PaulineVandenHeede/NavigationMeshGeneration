# Navigation Mesh Generation
Gameplay programming research topic

## Abstract


## Introduction/Description
For most game environments it's easily predictable where objects can and can't move[2]. With this information we can create a set of geometry to describe the walkable surface of a 3D environment, called a navigation mesh (nav mesh). The geometry of a nav mesh is build out of a set of convex polygons, this can be triangles or quadrilaterals, the latter giving a more compact and convenient representation [3]. 

There are different approaches to the generation of navigation meshes.[1]
1. Manually creating the navigation mesh.
  Manually creating a navigation mesh is done by a level designer in a level editor.  This mesh then gets loaded into the game and can directly be used.
2. Generating the navigation mesh.
  In this mehtod the raw game geometry is used to build a navigation mesh through code.
3. Combination of manually creating and generating the navigation mesh.

To get an optimal navigation mesh, our navigation mesh needs to meet some requirements.
The first requirement tells us that the polygons of which the navigation mesh is made need to be convex, as mentioned before. The reason why these need to be convex is simple. This is the only way our navigation mesh can guarantee that an agent inside the polygon can reach any other point inside the polygon in a straight line without colliding with the environment. This gives us the big advantage of fewer expensive collision checks with the environment.
Secondly, as we only can go out from the given raw game geometry our navigation mesh generator needs to be robust in handling degeneracy[3]. We cannot expect that data is given in a certain format that aids the generator. This means that we need to have a way to handle incomplete data or data that is different than we expect. If we do not want to go this route we need to query all the elements in the game environment with a certain flag set; e.g. elite engine navigation mesh
Other requirements of the navigation mesh are simplicity, completeness and consistent. We want our navigation mesh to have as few polygons as possible that covers the whole area where an AI can go and that there is no randomness, any equal map should have an equal outcome. [3]


## Design
*To generate a navigation mesh automatically we have a few steps we have to go through.*
*1. Walkable surface For 2D environments this can be done quite differently than for 3D environments. With a 2D environment we can give the navigation mesh generator a contour surface and all the static obstacles in the environment. The generator will then create a walkable surface by substracting the static obstacles from the contour. Highly likely we will get raw geometry of the game environment where we have to decide which parts are walkable and which parts aren't. This we can either do more manually by giving a contour surface of the world and substract every static obstacle from this contour; as is done in elite engine, or for 3D environments we can check this by using the normal vector of the polygons in the environment.*


## Implementation

## Result

## Conclusion/Future work

##  Bibliography
1. https://en.wikipedia.org/wiki/Navigation_mesh
2. Greg Snook, Simplified 3D movement and pathfinding using navigation meshes - Game programming gems p288-p291
3. Paul Tozour, Building a near-optimal navigation mesh - Game programming wisdom - p171-p185
4. https://www.gamedev.net/tutorials/programming/artificial-intelligence/navigation-meshes-and-pathfinding-r4880/
