Nerve wrecking, ball splitting action!

Nerve Ball -- "Do my balls get on your nerves?"

An addicting ballgame with a neural network guiding system modulating the trajectories of the balls.
- click on balls to divide them to smaller balls.
- the smaller the ball, the bigger the score.
- the smallest ball does not divide, but disappears and player gains one minute play time.
- the game ends when the time runs out or the balls all disappear.
- the speed of the balls is controlled by the mouse wheel.
- the faster the ball, the bigger the score.
- misses cause a time penalty.

The audio in the game is generated by a real time FM syntheziser.

To build them balls, you need CMAKE.

```
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build
```
Building tested on Debian and macos.

As a sidenote, the movements of the balls is a reminder of the wave/particle-dualism of
photons. The balls move in two distinct manners: in a straight line and around a circle.
The straight movement or the speed the ball and the circular movement or the direction of the ball are both modulated by the neural network.

