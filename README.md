### Introduction

This utility uses `qCompress()` to create a compressed version of distro.smime for use in custom Berryboot repositories:

[Creating Custom Repository #66](https://github.com/maxnet/berryboot/issues/66)

### Prep work

1. Generate private key and certificate:

````
openssl genrsa -des3 -out berryboot.key 1024
openssl req -new -key berryboot.key -out berryboot.csr
openssl x509 -req -days 30000 -in berryboot.csr -signkey berryboot.key -out berryboot.crt
````

2. Download the sample [distro.smime](http://dl.berryboot.com/distro.smime) and extract signed data:

````
openssl smime -verify -in distro.smime -noverify -out distro.ini
````

  * Prepare `distro.ini` by modifying entries in the file; each item requires the following parameters where the INI heading is unique and the SHA-1 sum and filesize match the image file:

````
[debian_dytest]
name=Dysproseum Berryboot Test v1.2
url=http://your-server.com/ez_wifi_broadcast_v1.6.img
memsplit=192
sha1=d3fed6d7fb6764f0142ea1e4fe8c01238823d628
description=EZ-Wifi Broadcast v1.6 2018-04-02
size=513015808
group=Popular
icon_b64="@ByteArray(...)"
````

Base-64 version of the icon should be a PNG image at 128x128.

### Signing

3. Use openssl to sign `distro.ini` as `distro.smime`:

````
openssl smime -sign -in distro.ini -text -out distro.smime -signer berryboot.crt -inkey berryboot.key
````

4. Compress into distro.zsmime

````
$ time ./qcompress
Compressed data written to file.
Decompressed data matches original data.
````

### File placement

5. Upload `distro.smime` and `distro.zsmime` to your web server;


6. Copy `berryboot.crt` to the root of the FAT partition on the SD card.

  * Optional: create `berryboot.ini`

  ````
  [repo]
  url=http://your-server.com/distro.zsmime
  ````

6. Back in Berryboot, click Add OS and go to Network Settings

 * Enter the URL to the `distro.zsmime` file.

### Troubleshooting

* Unable to download from Internet (HTTPs or DNS)

  * Berryboot seems to prefer HTTP over port 80

  * If trying HTTPS, Berryboot prefers a FQDN over an IP address

  * And prefers port 443 over a non-standard port

* SHA sum does not match

  * Double check the value with `sha1sum`

* Unable to open file for writing

  * If there is enough free space, this is an error state you can possibly get into if a download fails SHA matching, the file may be left in cache.

  * In this case, you can try:

    * Rename the file on the server

    * Update the distro.ini (update the name/description field as well, so you will know if your updated version has been refreshed)
    * Regenerate the S/MIME

    * Click Add OS and go to Network Settings - go to the Repository tab and then press ok - saving this form helps to re-fetch a fresh distro file.

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
