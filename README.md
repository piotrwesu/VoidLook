# VoidLook
VoidLook - Apod, Astronomy Photo of the Day in your CLI.

VoidLook is a lightweight terminal-focused image viewer designed for viewing of Astronomy Photo of the Day from NASA directly in the console using protocol **Sixel**.
It aims to provide a minimal, fast way to preview images without leaving the terminal environment.

## Example
<img width="1372" height="976" alt="voidlook" src="https://github.com/user-attachments/assets/1bd066f6-1e62-4281-8652-be6ea740d906" />

## Api Key
The ".env.example" file contains a universal key.

* Hourly Limit: 30 requests per IP address per hour
* Daily Limit: 50 requests per IP address per day

If you'd like to display more than this basic limit, or request your own developer key, please email NASA at https://api.nasa.gov/#signUp
Put your key to ".env" file.

## Dependencies
* Terminal escape sequence support (ANSI/OSC 8 / Sixel)
* CURL - libcurl
* libsixel
* STB

You need to install Curl and libsixel on your own by your package manager.
Chech Installation section



## Installation
#### Debian / Ubuntu
```bash
sudo apt install cmake pkg-config libcurl4-openssl-dev libsixel-dev
```
#### Arch
```bash
sudo pacman -S cmake pkg-config curl libsixel
```

#### Fedora / RHEL
```bash
sudo dnf install cmake pkg-config libcurl-devel libsixel-devel
```

All:

    git clone https://github.com/piotrwesu/VoidLook.git
    cd VoidLook
    mkdir build;cd build
    cmake -S .. -B .

#### Troubleshooting compilation
Different Linux distributions and package managers may store `libsixel` or `curl` headers in different locations (e.g., `/usr/include/sixel.h` vs `/usr/local/include/sixel/sixel.h`). 

If the build system fails to locate the `sixel.h` file, you can manually specify the path in source files.

## Usage

    voidlook


### Options
Showing version of program.

    voidlook --version
    voidlook -v

## Limitation
An app that only displays photos, sometimes Apod is a movie.

