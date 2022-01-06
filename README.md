# Navigation Mesh Generation
Gameplay programming research topic

## Abstract


## Introduction/Description
For most game environments it's easily predictable where objects can and can't move[2]. With this information we can create a set of geometry to describe the walkable surface of a 3D environment, called a navigation mesh (nav mesh). The geometry of a nav mesh is build out of a set of convex polygons, this can be triangles or quadrilaterals, the latter giving a more compact and convenient representation [3]. The polygons of the nav mesh need to be convex because this is the only way to guarantee that an agent in this polygon or otherwise called a cell can reach any other point in that cell in a straight line without colliding with the environment. This giving the advantage of fewer collision checks.

* *For the generation of Navigation meshes there are different approaches.
First of all we have the fully manual creation, this is a mesh created by the level designer that gets loaded into the game.
Secondly we have the automatic approach which we will discuss in further detail in this research paper.
And to have the best of both worlds there are approaches that have a combination of the manual and automatic approach.

* *To generate a navigation mesh automatically we have a few steps we have to go through.
* *1. Walkable surface
  For 2D environments this can be done quite differently than for 3D environments.
  With a 2D environment we can give the navigation mesh generator a contour surface and all the static obstacles in the environment. The generator will then create a walkable surface by substracting the static obstacles from the contour.
  Highly likely we will get raw geometry of the game environment where we have to decide which parts are walkable and which parts aren't.
  This we can either do more manually by giving a contour surface of the world and substract every static obstacle from this contour; as is done in elite engine, or for 3D     environments we can check this by using the normal vector of the polygons in the environment. 

## Design/Implementation

## Result

## Conclusion/Future work

##  Bibliography
1. https://en.wikipedia.org/wiki/Navigation_mesh
2. Greg Snook, Simplified 3D movement and pathfinding using navigation meshes - Game programming gems p288-p291
3. Paul Tozour, Building a near-optimal navigation mesh - Game programming wisdom - p171-p185
4. https://www.gamedev.net/tutorials/programming/artificial-intelligence/navigation-meshes-and-pathfinding-r4880/
