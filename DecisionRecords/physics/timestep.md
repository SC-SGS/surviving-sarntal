# How and when should physics be updated?

## Considered Options

* variable time step
* semi-fixed time step
* fixed time step
* parallel game loop

## Pros and Cons of the Options

See
also [Fix your timestep](https://web.archive.org/web/20130328024036/http://gafferongames.com/game-physics/fix-your-timestep/).

### Variable Time Step ("classic" serial game loop)

In each step of the game loop, we receive input, update the world state and render the scene.
I.e. Renderer and PhysicsEngine are closely coupled and updated equally often.

+ Good, because that is how it is right now and there are no performance issues.

- Bad, because performance issues might arise and decoupling physics and renderer is part of our task.
- Dynamic Timestep will lead to inconsistent physics calculations.
- Physics cannot handle *any* value as deltaT.

### Semi-fixed Time Step

The time step size is derived from the frame rate, but if the frames are too slow, we divide the frame step into
multiple constant physics substeps.

+ physics deltaT will not get too big (-> no glitching)
+ less inconsistent

- spiral of death if physics falls behind

### Fixed Time Step (serial game loop with decoupled physics and potentially parallel tasks)

The game loop will be left as is.
However, during the update step, multiple physics simulations are executed with constant time steps.
The number of steps depends on the frame time.
Parallelism can be exploited by dividing jobs in the physics simulation (and maybe also in the input and renderer) on
different threads.

+ constant physics step size that can be analytically or empirically tested and adjusted to taste
+ no glitching
+ as accurate as necessary
+ consistency across platforms
+ improved performance with parallelism on sub-game-loop level

### parallel game loop

Physics and Input/Renderer are running on two different threads, communicating via a channel.
When the renderer needs to draw the scene, he polls the current physics state ()

+ physics is decoupled and has a fixed time step (see above)

- more difficult to parallelize and synchronize
- performance might not be increased as much as with parallel jobs due to synchronization and dependencies

## Decision Outcome

Chosen option: "Fixed Time Step" (serial game loop with decoupled physics and potentially parallel tasks), because
it is the most common approach in game engineering, relatively easy to implement and has all the aforementioned
benefits.