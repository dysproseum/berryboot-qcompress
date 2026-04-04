## BerryBoot qCompress Utility

The `qcompress` utility uses same underlying DEFLATE algorithm as the zlib library, but adds a proprietary header.

* This nonstandard format can add an extra level of obfuscation for sensitive data - see [Other Uses](#other-uses)

Compatibility with standard zlib compressed files can be enforced with the `-zlib` parameter.

### Use in Custom BerryBoot Repositories

If you use [BerryBoot](https://www.berryterminal.com/doku.php/berryboot) you probably have added images to your Pi via USB or from a network share (SMB, NFS)

Hosting your own version of the default listing is discussed in [Creating Custom Repository #66](https://github.com/maxnet/berryboot/issues/66), letting us access BerryBoot images over the Internet and include metadata such as title, description and thumbnail image.

This utility implements the same `qCompress()` logic used in BerryBoot to generate the target compressed file `distro.zsmime` that secures and powers the listing.

#### Prep work

1. Generate private key and certificate `berryboot.crt`:

````
$ openssl genrsa -des3 -out berryboot.key 1024
$ openssl req -new -key berryboot.key -out berryboot.csr
$ openssl x509 -req -days 30000 -in berryboot.csr -signkey berryboot.key -out berryboot.crt
````

2. Download the sample [distro.smime](http://dl.berryboot.com/distro.smime) and extract signed data:

````
$ openssl smime -verify -in distro.smime -noverify -out distro.ini
````

  3. Prepare `distro.ini` by modifying entries in the file; each item requires the following parameters where the INI heading is unique and the SHA-1 sum and filesize match the image file:

````ini
[_dytest]
name=Dysproseum BerryBoot Test v1.2
url=http://your-server.com/ez_wifi_broadcast_v1.6.img
memsplit=192
sha1=d3fed6d7fb6764f0142ea1e4fe8c01238823d628
description=EZ-Wifi Broadcast v1.6 2018-04-02\nCan be on two lines.
size=513015808
group=Popular
icon_b64="@ByteArray(...)"
````

Base-64 version of the icon should be a PNG image at 128x128.

#### Signing

The file is signed in [S/MIME](https://en.wikipedia.org/wiki/S/MIME) format so its integrity can be verified.

4. Use openssl to sign `distro.ini` as `distro.smime`:

````
$ openssl smime -sign -in distro.ini -text -out distro.smime \
  -signer berryboot.crt \
  -inkey berryboot.key
````

5. Compress into `distro.zsmime`

````
$ ./qcompress -f distro.smime
Compressed data written to file.
Decompressed data matches original data.
````

#### On the web server

6. Upload `distro.smime` and `distro.zsmime` to your web server;

#### On the Pi

7. Copy `berryboot.crt` to the root of the FAT partition on the SD card.

  * Optional: create `berryboot.ini`

  ````
  [repo]
  url=http://your-server.com/distro.zsmime
  ````

8. Back in BerryBoot, click Add OS and go to Network Settings

 * Enter the URL to the `distro.zsmime` file.

### Troubleshooting

* `Error downloading distribution list from Internet`:

  * BerryBoot seems to prefer `http://` on port 80

  * If trying `https://`, BerryBoot prefers a FQDN over an IP address, and prefers port 443 over a non-standard port

* Whenever you update the distro file:

  * Click Add OS and go to Network Settings - go to the Repository tab and then press ok - saving this form helps to re-fetch a fresh distro file.

* `Error downloading file from Internet. file corrupt (sha1 does not match)`

  * Double check the value with `sha1sum`

* `Error downloading file from Internet. Error opening output file`:

  * If a download fails, the file may get stuck in an error state and fail on subsequent attempts until the file is manually cleared from cache.

  * If you have access to the device, you can click More Options - open Console

    * Remove files in `/mnt/tmp/cache` folder.

  * Otherwise, on the server, you can try:

    * Rename the file on the server

    * Update the distro.ini (update the name/description field as well, so you will know if your updated version has been refreshed)

    * Regenerate the S/MIME and `distro.zsmime` files

### Other Uses

* Produces slightly smaller files than gzip or zip

### Compiling

To compile the `qcompress` binary, install the required packages for your distribution:

#### Debian x86_64:

````
$ time sudo apt install qt5-default
````

#### Raspberry Pi armv7:

````
$ time sudo apt install qtbase5-dev qtchooser qt5-make qtbse5-devtools qtcreator qtdeclarative5-dev
````

The instructions below are generic for compiling a Qt project.

---

To compile a basic Qt application from the command line, follow these steps:

#### Create a minimal Qt program

Save the following code in a file named main.cpp:

````cpp
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
