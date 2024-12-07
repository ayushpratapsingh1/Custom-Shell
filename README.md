
# Custom Shell in C++

## Overview  
This project is a **Custom Shell** implemented in **C++**, designed to emulate the core functionalities of a Unix-like shell. It handles **command parsing, execution, directory navigation, and built-in commands** such as `cd`, `pwd`, and `echo`, with proper handling of special characters, quotes, and escape sequences.  

## Features  
- **Command Parsing**  
  - Supports single (`'`) and double (`"`) quotes.  
  - Handles escaped characters like `\` for special symbols and spaces.  
  - Splits input commands into arguments based on spaces while respecting quoted and escaped inputs.  

- **Built-in Commands**  
  - `cd <path>`: Change the current working directory.  
  - `pwd`: Display the current working directory.  
  - `echo <args>`: Print arguments to the shell output.  
  - `type <command>`: Identify whether a command is built-in or executable and locate its file path.  

- **Command Execution**  
  - Searches executable files in system `PATH`.  
  - Executes external commands using `std::system`.  
  - Gracefully handles errors like "command not found" or invalid paths.  

## How It Works  
1. **Input Parsing**  
   The shell uses a robust parser to tokenize the input command, accounting for special characters, quotes, and escape sequences.  
   
2. **Execution Flow**  
   - For recognized built-in commands, appropriate functions are called.  
   - For external commands, the shell searches directories in the `PATH` environment variable and executes the command if found.  

3. **Directory Management**  
   - Implements directory navigation using the `std::filesystem` library.  
   - Supports relative paths, absolute paths, and user home directory shortcuts (`~`).  

## Example Usage  
    ```
      $ pwd
      /home/user/projects/shell
      $ cd ..
      $ pwd
      /home/user/projects
      $ echo "Hello, World!"
      Hello, World!
      $ type echo
      echo is a shell builtin
      $ type ls
      ls is /usr/bin/ls

## Requirements
- C++17 or later
- Compiler with support for the std::filesystem library

## Compilation
To compile the shell:
    ```
    g++ -std=c++17 main.cpp -o shell
    ```

## Running the Shell
Run the compiled binary:
    ```
     ./shell
    ```

## Future Enhancements
- Add support for command piping (|) and redirection (>/<).
- Implement job control (bg, fg, jobs commands).
- Expand support for environment variable management.
