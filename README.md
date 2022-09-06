# Grab Key Library

The library is designed to provide a simple way to control console application with keyboard.

## Simple keyboard reader

The code below allows to control cursor's position in terminal with using keyboard arrow keys. Pressing ESC key will break the loop.
BTW - All the methods of **KeyboardReader** are thread safe.

```c++
#include "kbd_keys.h"
#include "kbd_reader.h"

int main() {
  // start reader
  auto kbd = keyboard::KeyboardReader::run();

  // set loop conditions
  while (kbd.is_running()) {
    // wait for a key pressed
    switch (kbd.get_key()) {
      // handle the key
      case keyboard::Key::UP:    std::cout <<  "\033[1A";   break;
      case keyboard::Key::DOWN:  std::cout <<  "\033[1B";   break;
      case keyboard::Key::RIGHT: std::cout <<  "\033[1C";   break;
      case keyboard::Key::LEFT:  std::cout <<  "\033[1D";   break;
      case keyboard::Key::ESC:   kbd.stop(); /*break loop*/ break;
    }
  }

  return 0;
} // main
```



Getting a byte sequence (ANSI) of the key is also provided - just pass vector of char like the below.

~~~c++
#include "kbd_keys.h"
#include "kbd_reader.h"

int main() {
  // start reader
  auto kbd = keyboard::KeyboardReader::run();

  // set loop conditions
  while (kbd.is_running()) {      
    // create a container
    std::vector<char> sequence;
    // pass the container inside get_key
    // and wait for a key pressed
    switch (kbd.get_key(sequence)) {
      // handle the key
      case keyboard::Key::ESC:   kbd.stop(); /*break loop*/ break;
      default:
        std::cout << "sequence: ";
        for (const auto& c : sequence)
          std::cout << (int)c; // print the ANSI sequence
        std::cout << std::endl; // is also performs flush
    }
  }

  return 0;
} // main
~~~



## Events processor

This approach is designed to focus on *actions* that are required to be performed when pressing keys instead of focusing on keys itself. The main idea is to bind functions to keys using a configuration. The processor then cares of calling the functions when the according keys are pressed.

Changing of configuration can be performed unlimited number of times. It can also be changed in another thread.

### Example with configuring processor after starting

```c++
#include <iostream>
#include "kbd_keys.h"
#include "kbd_reader.h"

int main() {
    // run the events processor
    // it starts with an empty config - so initially it ignores all the pressed keys
    auto processor = keyboard::EventsProcessor::run();

    // acquire copy of the config (the operation is thread safe)
    auto config = processor.get_config();
    
    // optionally set the handlers that are need to be executed before and after the main handler
  	config.after_handler   = [](keyboard::Key, const std::vector<char>&) { std::cout << std::flush; };
 	  // config.before_handler = [](auto, auto) {}; - no need to set empty handler if no action required	
  
  	// the handlers btw are simply std::function<void(keyboard::Key, const std::vector<char>&)>
  	// where the keyboard::Key is a pressed key and the vector of chars is ANSI sequence of the key
    
    // set the default_handler which is invoked when the pressed key wasn't configured
    // or leave as is, without handler - no need to set empty handler if no action required
    config.default_handler = [](keyboard::Key, const std::vector<char>& s) { 
      for (auto c : s) std::cout << c;
    };

    // set the main key handlers
    config.filters = { 
        keyboard::EventsFilter{ { keyboard::Key::UP    }, [] (auto, auto) { std::cout << "\033[1A"; } },
        keyboard::EventsFilter{ { keyboard::Key::DOWN  }, [] (auto, auto) { std::cout << "\033[1B"; } },
        keyboard::EventsFilter{ { keyboard::Key::RIGHT }, [] (auto, auto) { std::cout << "\033[1C"; } },
        keyboard::EventsFilter{ { keyboard::Key::LEFT  }, [] (auto, auto) { std::cout << "\033[1D"; } },
        keyboard::EventsFilter{ { keyboard::Key::ESC   }, [&](auto, auto) { processor.stop();       } }
    };

    // apply config (the operation is thread safe)
    processor.apply_config(config);

    // keep the processor going and wait until it stop
    processor.join();
} // main
```



### Example with configuring processor before starting

```c++
#include <iostream>
#include "kbd_keys.h"
#include "kbd_reader.h"

int main() {
    EventsConfig config;

    // optionally set the handlers that are need to be executed before and after the main handler
  	config.after_handler   = [](keyboard::Key, const std::vector<char>&) { std::cout << std::flush; };
 	  // config.before_handler = [](auto, auto) {}; - no need to set empty handler if no action required

    // set the default_handler which is invoked when the pressed key wasn't configured
    // or leave as is, without handler - no need to set empty handler if no action required
    config.default_handler = [](keyboard::Key, const std::vector<char>& s) { 
      for (auto c : s) std::cout << c;
    };

    // set the main key handlers
    config.filters = { 
      keyboard::EventsFilter{ { keyboard::Key::UP    }, [] (auto, auto) { std::cout << "\033[1A"; } },
      keyboard::EventsFilter{ { keyboard::Key::DOWN  }, [] (auto, auto) { std::cout << "\033[1B"; } },
      keyboard::EventsFilter{ { keyboard::Key::RIGHT }, [] (auto, auto) { std::cout << "\033[1C"; } },
      keyboard::EventsFilter{ { keyboard::Key::LEFT  }, [] (auto, auto) { std::cout << "\033[1D"; } }
    };

    // run the events processor with the prepared config
    auto processor = keyboard::EventsProcessor::run(config);

    // config can be obtained and edited at any time
    auto config = processor.get_config();
    config.filters.emplace_back(
    	keyboard::EventsFilter{ { keyboard::Key::ESC   }, [&](auto, auto) { processor.stop(); } }
    );
    
    // and also be applied to the processor
    processor.apply_config(config);
    
    // keep the processor going and wait until it stop
    processor.join();
} // main
```



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
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
```

To install the library into your system execute

```bash
make install
```

By default it copies the libraries by the next paths:

```bash
/usr/local/lib/libGrabKeyShared.so
/usr/local/lib/libGrabKeyStatic.a
```

and the includes into the next directory:

```bash
/usr/local/include/grab-key/
```

You can add a prefix to the target installation path by using DESTDIR option:

```bash
make DESTDIR=/your/prefix install
```

The library then will be placed at

```bash
/your/prefix/usr/local/lib/libGrabKeyShared.so
/your/prefix/usr/local/lib/libGrabKeyStatic.a
```

and the includes at

```bash
/your/prefix/usr/local/include/grab-key/
```

