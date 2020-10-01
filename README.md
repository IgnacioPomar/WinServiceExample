# libOS
Currently is a simple wrapper for the creation of services in windows

## Motivation

This project was born to cover a specific need: to create a library that 
contains the entry point of the application (simplify the creation of services 
in windows).

Once created, the objective is to expand this platform including other utilities.


## Requirements
The solution brings two projects. the library itself has no dependency.

The tester project uses libmicrohttpd to launch a usable example with some 
practical effect. 
To start it, it is necessary to keep the libbrary in the following structure:
```
$(SolutionDir)../vc/$(PlatformShortName)/include/
$(SolutionDir)../vc/$(PlatformShortName)/lib/
$(SolutionDir)../vc/$(PlatformShortName)/dll/
```

**Optionally**, you can disable this library from srvcTesterCfg.h, in which 
case you will have no dependencies.


## Authors
- Ignacio Pomar Ballestero

## License

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

**IMPORTANT:** This license does not include the file SourceBuiltInEncryption_Base64 
wich comes from stackOverflow, and therefore, is unther the CC BY-SA 3.0 





