# Disk File System

Author: Mahmoud Dawood

## Background

The Disk File System is a simulation that emulates the functionality of a disk using the indexed allocation method. It provides a virtual representation of how files are organized and stored on a hard disk.

## Usage

The simulation includes several classes and functions that facilitate file operations on the disk. Here are the key functions available:

- `CreateFile(filename)`: Creates and opens a file.
- `WriteToFile(file_descriptor, data, length)`: Writes data to a file.
- `ReadFromFile(file_descriptor, buffer, length)`: Reads data from a file to a buffer.
- `OpenFile(filename)`: Opens a file.
- `CloseFile(file_descriptor)`: Closes a file.
- `fsFormat(block_size)`: Formats the disk.
- `listAll()`: Prints the disk contents.
- `DelFile(filename)`: Deletes a file.
- `decToBinary(integer, char)`: Converts a decimal value to binary and prints it into a character.

To compile and run the simulation, follow these steps:

1. Compile the code using the provided command: `g++ ex7.cpp -o ex7`.
2. Run the compiled program: `./ex7`.

When running the program, you can choose an option from 0 to 8, where each number represents a specific function in the simulation. The program will display the results or failure reasons for each executed function.

