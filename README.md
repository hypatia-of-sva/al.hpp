# al.hpp

## Under construction! Only in planning phase ATM

AL.hpp is a C++ wrapper around OpenAL, that intends to make OpenAL usable in a style more expressive of its object model

Intended use:

a) as a drop in replacement for OpenAL function calls with namespaces, like this:

```cpp
al::load(); // loads the function pointer from a DLL
ALCdevice* default_device = alc::openDevice(nullptr);
ALCcontext* ctx = alc::createContext(default_device, nullptr);
alc::makeContextCurrent(ctx);
ALUint buffer;
al::genBuffers(1,&buffer);
// ....
// use buffer, sources etc..
// ...
al::deleteBuffers(1,&buffer);
alc::destroyContext(ctx);
alc::closeDevice(default_device);

// DLL is automatically closed by a global destructor
// but you can close it manually if you want to reopen another DLL etc.
al::unload();
```

b) by the new interface. It's intended to look eventually something like this:

```cpp
//default constructor loads DLL and opens the device
al::device default_device; 
// holds the context
al::listener my_listener {default_device}; 
// sets the device, dat, length, frequency and format of the buffer
al::buffer my_buffer {default_device, data_ptr, data_ptr.size(), 44100U, al::format::mono16}; 
// sets the context, buffer, and 3D position of the source
al::source my_source {my_listener, my_buffer (al::vec3) {0.0, 0.1, 1.2}}; 

my_source.play();
//...

//at the end of scope, call the corresponding cleanup functions.

```

The eventual design of the API might be slightly different, but the idea is to give basic defaults for most things and to let the types do the cleanup like close devices, destroy buffers and hold the ```ALuint`` etc.