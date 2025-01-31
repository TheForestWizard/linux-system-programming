# Linux Inter Process Communication (ICP) types

## 1. Pipe

A pipe represents one directional data flow between two processes. One process writes data via the
input of the pipe, the other process reads data via the output of the pipe. One process needs to know
another process to create a pipe between them, therefore this method is most common in parent-child
processes in the context of one program.

See the playground `pipe/`. One process reads a .WAV file and writes it via a pipe to another 
process, which reads it and plays it.

## 2. FIFO

FIFO (or named pipe) represent one directional data flow between two processes. It's like a regular
pipe, but its file name can be defined beforehand in different independant processes, which means
two different programs could communicate with one another.

See the playground `fifo/`. It's a singleplayer "game", where the player can walk around a grid. The 
program `input` gets player input and writes it via a named pipe to the program `game`, which reads
it and displays the updated gameplay.

## 3. Shared memory

## 4. Message queue

## 5. Socket
