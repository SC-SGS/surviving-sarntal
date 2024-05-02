# Discussion about the basic architecture of our project, decision for OOP

## Context and Problem Statement

The current project is structured using Flecs and the Entity-Component-Statement. As a team, we are unfamiliar with the ECS-architecture,
since we have only worked with OOP so far. Therefore, we are considering removing the ECS, and replacing it with a new structure.

## Problem Statement

We have to decide how to structure our project on a fundamental level. We need to make the decision if we want to keep
the existing ECS structure or change it to OOP.

## Considered Options

* Keeping the Entity-Component-System
* Change ECS to OOP

## Pros and Cons of the Options

### {Keeping the Entity-Component-System}

Keep the structure of the project as it currently is. While refactoring or adding new functionality, we have to keep adhering
to the principles of the ECS.

+ Good, because ECS is the better option for performance-critical applications
+ Good, because ECS provides flexibility and reusability, since components can be easily added
+ Good, because it would make the software engineering part of the project less complex/ work-intense for us
- Bad, because the ECS makes it more difficult for us to understand the structure and functionality of the existing codebase
- Bad, because the existing documentation for Flecs is difficult to understand

### {Change ECS to OOP}

Renew the whole structure of the project and design a new structure based on OOP. Transform the ECS design into the OOP design.

+ Good, because OOP design is more intuitive and familiar for the team
+ Good, because OOP makes code reuse easier through inheritance and polymorphism
+ Good, because the design transformation would force the team to throughly understand the code
+ Good, because previous experience with OPP would make the refactoring of code, adding/ improving functionality easier for the team
- Bad, because it requires extensive refactoring of the codebase

## Decision Outcome

Chosen option: "Change ECS to OOP", because it makes the physics as well as the game engineering part of the project more 
approachable for us, since it is the project architecture/design we are most familiar with.
Changing the structure from ECS to OOP will give us the opportunity to create a basic code structure
of the project, that can easily be understood, extended and maintained by our team.
