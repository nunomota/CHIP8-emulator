## Chip8 emulator:

This is a really simple implementation of a Chip8 emulator.

I chose to make it an ***interpreting*** and not a ***recompiling*** emulator for the sake of simplicity (as a static recompiling 
one has its downfall for self-modifying code, a dynamic one should be chosen instead - which would mean a lot of work) 
but I might change this in the future.

**Implementation choices:**
   1. use base size `int` over `short` or `long` 
      - reduces amount of code the compiler generates to convert between different integer lengths;
      - may also reduce the memory access time, as some CPUs work fastest when reading/writing data of the base size;
   2. use `for(;;)` instead of `while(CPUIsRunning)` or `while(1)`
      - `while(CPUIsRunning)` can terminate the loop at any moment by setting the variable to 0, which is good,
      but checking the variable on each pass can waste quite a lot of CPU time;
      - `while(1)` is clearly a bad choice, as some compilers will generate code checking whether 1 is true or not;
   3. make memory read/write directly to an array and not through a function
      - this will avoid the overhead associated with a function call;
      - it is just possible because we are not using, for example, paged memory;
___
### Informational references:

   - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
   - https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
   - http://fms.komkon.org/EMUL8/HOWTO.html