# Grab Key Library

The library is designed to provide a simple way to control console application with keyboard. Check the examples below.

## keyboard reader

The code below allows to control cursor's position in terminal with using keyboard arrow keys. Pressing ESC key will break the loop.

```c++
#include <iostream>
#include "grabkeypp/kbd_reader.h"

int main() {
  // create reader
  keyboard::KeyboardReader reader;
  
  // setup loop conditions
  bool keep_going = true;
  while (keep_going) {
    // read key
    auto [key, raw] = reader.get_key();
    
    // react on key
    switch (key) {
      case keyboard::Key::UP:
      case keyboard::Key::DOWN:
      case keyboard::Key::RIGHT:
      case keyboard::Key::LEFT:
      case keyboard::Key::ENTER:
        std::cout << raw.data() << std::flush; break;
      case keyboard::Key::ESC: keep_going = false; break;
    }
  }

  return 0;
} // main
```

## Events processor

This approach is designed to focus on *actions* that are required to be performed when pressing keys instead of focusing on keys itself. The main idea is to bind functions to keys using a configuration. The processor then cares of the functions invocation when the according keys are pressed.

```c++
#include <iostream>
#include "grabkeypp/events_processor.h"

int main() {
    // Create an EventsProcessor
    keyboard::EventsProcessor processor;
    
    // Get copy of the existing configuration
    auto config = processor.get_config();

    // Configure reactions on your keys
    config.handlers[keyboard::Key::ESC] = [&processor](auto) { processor.stop(); };
    config.default_handler = [](auto key) { std::cout << key.buffer.data(); };
    config.after_handler = [](auto) { std::cout << std::flush; };
    
    // Set the configuration back to the processor
    processor.apply_config(config);

    // Run the processor's loop
    processor.start();

    return 0;
} // main
```

## Customization

There are KeyboardReaderBase and EventsProcessorBase templates exists in the code - so they are highly customisable for your needs. Check the examples at Examples folder.

In case you need to to use it with C application - there are C components at GrabKeyLib/grabkey. 
The C++ parts are located at GrabKeyLib/grabkeypp.

## Build and Install

### Linux and MacOS

```bash
git clone https://github.com/frickelangelo/GrabKeyLib.git
mkdir GrabKeyLib/build && cd GrabKeyLib/build
cmake .. && make
```

By default it builds a release version of the library.
In case if you need a debug version - build with "CMAKE_BUILD_TYPE" option:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug
```

The build by default produce static libraries. To build shared libraries use  BUILD_SHARED_LIBS option:

```bash
cmake -DBUILD_SHARED_LIBS=YES
```

To install the library on your system execute:

```bash
make install
```

By default it copies binaries by the next paths:

```bash
/usr/local/lib/grabkey/
```

and the includes into the next directory:

```bash
/usr/local/include/grabkey/
```

You can add a prefix to the target installation path by using DESTDIR option:

```bash
make DESTDIR=/your/prefix install
```

The binaries then will be placed at

```bash
/your/prefix/usr/local/lib/
```

and the includes at

```bash
/your/prefix/usr/local/include/grabkey/
```

### Packaging

For building package uses cpack. The build command should run from your build directory (for example *GrabKeyLib/build*).

```bash
cpack -G <GENERATOR>
```

Specify generator from the list:
```
  7Z                           = 7-Zip file format
  DEB                          = Debian packages
  External                     = CPack External packages
  IFW                          = Qt Installer Framework
  NSIS                         = Null Soft Installer
  NSIS64                       = Null Soft Installer (64-bit)
  NuGet                        = NuGet packages
  RPM                          = RPM packages
  STGZ                         = Self extracting Tar GZip compression
  TBZ2                         = Tar BZip2 compression
  TGZ                          = Tar GZip compression
  TXZ                          = Tar XZ compression
  TZ                           = Tar Compress compression
  TZST                         = Tar Zstandard compression
  ZIP                          = ZIP file format
```

## Importing into yor project

The project has config files that are being installed at

```bash
/your/prefix/usr/local/cmake/
```

To import the library in your project use cmake find_package command:

*GrabKey* package has a *grabkeylib* target which is an interface library that contains all the required directories and libraries to link with.
Same as *GrabKeyPp* package - it has grabkeypplib target.

```cmake
# Linking with C library GrabKey
find_package(GrabKey REQUIRED)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE grabkeylib)

# linking with C++ library GrabKeyPp
find_package(GrabKeyPp REQUIRED)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE grabkeypplib)
```

After linking with the interface libraries use grabkey or grabkeypp prefix for including headers:

```c++
// including GrabKeyPp headers
#include "grabkeypp/events_processor.h"
#include "grabkeypp/events_config.h"
#include "grabkeypp/kbd_keys.h"

// including GrabKey headers
#include "grabkey/terminal.h"
#include "grabkey/fd_monitor.h"
```

