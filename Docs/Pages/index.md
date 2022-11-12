# Catalyst

## Technical Specification
- Platform Information
    - Developed in C++
    - using Visual Studio 2022
    - Targets Windows (x86, x64)
- Project Information
    - Game Project
        - Contains game content
    - Engine Project
        - Contains the core logic for systems such as rendering or entity management. 
- Build Pipeline
    - Configured and built using CMake
        - Portable and easy to build 
            - CMake 3.14 or later required
            - install the latest version from https://cmake.org
        - Automatically sets up project dependencies, trivializing setup
    - To Build, Checkout the repository, and then run `Build.sh`
        - Checkout with 
        ```
        git clone https://github.com/ewanburnett/catalyst
        cd Catalyst
        ```
        - Then run CMake with the commands
        ```
        mkdir build && cd build
        cmake ..
        ```
- Dependencies
    - DirectX 11
    - TinyXML 2
    - ASSIMP

[Game Design Documentation](GDD.md)