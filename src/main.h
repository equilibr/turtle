#ifndef MAINBRAIN_H
#define MAINBRAIN_H

#include "ThreadedBrain.h"

void Main(ThreadedBrain &brain);

void Draw(ThreadedBrain &brain);
void Follow(ThreadedBrain &brain);
void Adder(ThreadedBrain &brain);

void MazeSolverWall(ThreadedBrain &brain);

#endif // MAINBRAIN_H
