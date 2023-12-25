<h1 align="center"> 
   <span>Simple Graphics Library</span>
</h1>

<div align="center">

<a href="https://github.com/BoyManWamen/SimpleGraphicsLibrary/stargazers">![GitHub Repo stars](https://img.shields.io/github/stars/BoyManWamen/TrainingExercises?style=social)</a>
<a href="https://github-com.translate.goog/BoyManWamen/SimpleGraphicsLibrary/blob/main/README.md?_x_tr_sl=auto&_x_tr_tl=en&_x_tr_hl=en&_x_tr_pto=wapp">![Translate](https://img.shields.io/badge/Translate-blue)</a>
</div>

<div align="center">
<img src="https://i.imgur.com/Sm2jGxq.png"/>
</div>

## Table of Contents

* [Description](#description)
* [Installing](#installing)
* [Version History](#version-history)
* [Credits](#credits)

## Description

Embark on an exciting journey into the realm of 3D graphics with SimpleGraphicsLibrary, a project designed for aspiring programmers eager to explore the fundamentals of computer graphics using C++. This OpenGL-inspired endeavor is perfect for amateurs seeking hands-on experience in rendering simple 3D scenes.

## Getting Started

### Installing

Make sure to download cmake and ninja before compiling the program.

[Cmake](https://cmake.org/download/)

[Ninja](https://ninja-build.org/)

First, navigate to the directory you want to have your project.

```sh
cd path/to/your/directory
```

Next, clone the repository.

```sh
git clone https://github.com/BoyManWamen/SimpleGraphicsLibrary.git
```

Once you have cloned the repository, you can now move into the project directory.

```sh
cd SimpleGraphicsLibrary
```

Make the build directory.

```sh
mkdir build
```

Change your directory to build.

```sh
cd build
```

You can create the executable with cmake.

```sh
cmake ../src/
```

```sh
cmake --build . -j
```

TGA files can be used as input for the executable in the obj directory. 

```sh
./simplegraphicslibrary ../obj/{object_name}/{object_name.obj}
```

You can create the obj directory doing a terminal command.

```sh
mkdir obj
```

## Version History

* 1.0.0
    * Initial Release

## Credits

Dmitry V. Sokolov

[tinyrenderer Repo](https://github.com/ssloy/tinyrenderer)

