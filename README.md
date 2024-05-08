# Shafa

**Shafa** is a C++ project that provides a set of tools for configuring, building, and running C++ projects. It uses TOML files for configuration and supports different C++ compilers.

## Features
-	Configuring C++ projects using TOML files.
-	Building C++ projects with different compilers and versions (***Only clang++ is currently supported***).
-	Building C++ projects.
-	OpenSSL library for file hashing.
-	TOML++ library for parsing TOML files.

## How to Use
1.	Clone the repository.
2.	Open the project in your preferred IDE (e.g., Visual Studio).
3.	Build the project.
4.	Run the project with the desired arguments.

## Command Line Arguments
**Shafa** supports the following command line arguments:
-	none: Default argument.
-	help: Displays help information.
-	configure: Configures the project based on the provided TOML file.
-	build: Builds the project.
-	run: Runs the project (***not currently supported***)

## Configuration
**Shafa** uses TOML files for configuration. Here is an example of a configuration file:
``` Toml
[settings] # Obligatory key
projectName = 'New Project'  # Obligatory value 
projectVersion = '0.0.1'  # Obligatory value 
projectType = 'application'  # Obligatory value 

[compilation] # Obligatory key
cppVersion = 'c++23'

[configure] # Obligatory key
```

## Configuration Keys
***documentation for "Configuration Keys" is currently not finished!!***

## Building
To build a project, use firstly use the configure and then build command line arguments, respectively. For example:

``` powershell
./Shafa.exe configure
./Shafa.exe build
```

## Limitation
This project is currently in early alpha, so in what limits are we in?
- Supported c++ compilres: **clang++**.
- Supported project types: **Static library**, **Dynamic library**, **Application**.
- Currently, **Shafa** is supported only in Windows, there is no plan for other OSes.
- Currently, there is no additional **clang++** flags.
- There is currently no default LLVM compiler (out of the box) yet.

## Bugs
It is useful to note that due to the early alpha of the project, bug reports are appreciated!!