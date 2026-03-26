### Introduction

This utility uses `qCompress()` to create a compressed version of distro.smime for use in custom Berryboot repositories:

https://github.com/maxnet/berryboot/issues/66

### Usage

1. Prepare distro.txt
2. Sign into distro.smime
3. Compress into distro.zsmime

````
$ time ./qcompress
````

### Signing

````
openssl genrsa -des3 -out berryboot.key 1024
openssl req -new -key berryboot.key -out berryboot.csr
openssl x509 -req -days 30000 -in berryboot.csr -signkey berryboot.key -out berryboot.crt
openssl smime -sign -in distro.txt -text -out distro.smime -signer berryboot.crt -inkey berryboot.key
````

### Compiling

The instructions below are generic for compiling a Qt project.

````
$ time sudo apt install qt5-default
````

---

To compile a basic Qt application from the command line, follow these steps:

#### Create a minimal Qt program

Save the following code in a file named main.cpp:

````
#include <QApplication>
#include <QLabel>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QLabel label("Hello, World!");
    label.show();
    return app.exec();
}
````

#### Generate a project file

Run `qmake -project` in the directory containing main.cpp. This creates a .pro file. 

#### Generate a Makefile

Run `qmake` to generate a platform-specific Makefile based on the .pro file. 

#### Compile the project

Execute `make` (on Linux/macOS) or nmake (on Windows with MSVC) to compile the application. 

#### Run the executable
After successful compilation, run the generated binary (e.g., `./main` on Linux/macOS, `main.exe` on Windows).
