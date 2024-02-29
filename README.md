# CPP-Monkey-Interpreter

CPP-Monkey-Interpreter is a C++ implementation of the Monkey programming language, inspired by Thorsten Ball's "Writing an Interpreter in Go." Monkey is a simple, dynamically typed programming language designed for educational purposes. This project aims to replicate the functionality of the Monkey interpreter as described in the book, in C++ instead of Go as a learning exercise.

## Features

The current version of CPP-Monkey-Interpreter includes the following features:

- **Lexer**: Breaks down the source code into a series of tokens, facilitating the parsing process.
- **Parser**: Analyzes the token sequence to construct an Abstract Syntax Tree (AST), representing the program's structure.
- **AST**: A tree representation of the syntactic structure of the source code, enabling easy manipulation and evaluation.
- **Tree-Walking Evaluation**: Traverses the AST to interpret and execute the Monkey code directly, evaluating expressions and executing statements.
- **REPL (Read-Eval-Print Loop)**: An interactive shell that allows users to enter and evaluate Monkey expressions on the fly, providing immediate feedback.
- **Basic Data Types**: Support for integers, booleans, strings, arrays, and hash maps.
- **Functions**: First-class citizens with the ability to define and invoke functions, including closures.
- **Control Structures**: Implements control flow with if-else statements and loops.

## Roadmap

While the interpreter is functional and covers the core aspects of the Monkey language, there are several enhancements and features planned for future development:

- **Complete Garbage Collector**: Implementing a more sophisticated garbage collection mechanism to efficiently manage memory usage and prevent leaks.
- **Tail Call Optimization**: Optimization for recursive function calls to allow deeper recursion without stack overflow risks.
- **More Data Types**: Introduction of additional data types (e.g., floating-point numbers, sets) to enrich the language's expressiveness.
- **Module System**: Support for importing and organizing code across multiple files or modules.
- **Standard Library**: Development of a rudimentary standard library providing useful functions and utilities for common tasks.
- **Error Handling**: Enhanced error reporting and handling mechanisms for runtime errors and exceptions.
- **Concurrency Support**: Adding features to support concurrent execution, such as goroutines or async/await patterns.
- **JIT Compilation**: Exploring Just-In-Time compilation techniques to improve execution performance beyond tree-walking interpretation.
- **Server Integration**: Creating a sandbox environment over a webserver utilizing Monkey's REPL. 
