# Physics Engine

## Context

Do we need separate classes for Hiker and Rock Physics Engine?

## Considered Options

* MovableEntities and PhysicsEngine
* HikerEngine and RockEngine
* MovableEntities and HikerEngine and RockEngine

## Pros and Cons of the Options

### MovableEntities and PhysicsEngine

Hiker and Rock inherit from MovableEntities and override calculateForces methods.
The PhysicsEngine is indifferent towards object type (generic) and has one move-function that only varies through different parameters.

+ simpler physics engine
- physics calculations in rock and hiker class

### HikerEngine and RockEngine

Hiker and Rock are classes with no common ancestors and separate engines that perform the calculations to move the objects.
Both engines inherit from one PhysicsEngine class defining the most important methods.

+ physics is actually performed in physics engines
- no common ancestor between hiker and rock, which would be convenient since they are both entities of the game

### MovableEntities and HikerEngine and RockEngine

Hiker and Rock inherit from MovableEntities, they have each their corresponding engine inheriting from an abstract PhysicsEngine class.

+ physics is actually performed in physics engines
+ hiker and rock have a common ancestor
+ adheres to the OOP approach
- might be unnecessary complex

## Decision Outcome

Chosen option: "MovableEntities and HikerEngine and RockEngine", because
it might be more complex, but it realizes our inner vision of the game world and is cleaner with regards to OOP standards.

