CES
===

C++ Embedded Script - CES is used to write template file for code generation. 

# Problem 
Code generation tools are often needed in large projects, writing such tools is not complex but as the project grows and the generation needs evolve, mainting them becomes more and more tedious. 

The solution of embedding script code into _source_ code simplifies both reading and maintainability of template files.
CES is a simple library that parse template file with embedded Lua code and generate files.

# Features

* Source-Language independant (CES doesn't parse the _source_ language, it can be whatever you want) 
* Expose your C++ to Lua (You can access to your datas/methods from embedded-Lua script)
* Simple embedding syntax

# Requirements

CES uses *CMake* as build system
*Boost*, *SWIG* and *Lua* libraries need to be installed.

# Getting Started

Run the following code:

    #include <iostream>
    #include "ces/CES.hpp"
    #include "ces/LuaVM.hpp"
    #include "ces/Interpreter.hpp"

    int main(int argc,char* argv[])
    {
      ces::LuaVM vm;
      ces::Parser parser;
      ces::Interpreter inter;
      
      vm.open();
      parser.parse("Hello@[for i=1,5 do]@ @i@,@[end]@ World");
      inter.interpret(&vm, parser.ast(), std::cout);
      vm.close();
    
      return 0;
    }
  
This should produce the following output:

    Hello 1, 2, 3, 4, 5, World
  
## Details

Let's see how does it work. 
The following line open and close the Lua virtual machine. 
  
    vm.open();
    ...
    vm.close();

LuaVM is necessary to run the embedded script writen in Lua.
Then the `parser` parses (obviously) a chunk of CES Script:

    parser.parse("Hello@[for i=1,5 do]@ @i@,@[end]@ World");
  
The details of the CES syntax are presented below.
And finally the `interpreter` interprets the parser's AST (Abstract Syntax Tree), produces a Lua script, run this script using the Lua virtual machine `vm` and stream the output to `std::cout`.

# CES Syntax

### Lua delimiters

The syntax of CES is very simple. The goal is to mix Lua in anything thus there are delimiters to mark when Lua code is starting and when _anything_ is starting.

    "Hello@[for i=1,5 do]@ @i@,@[end]@ World"

Lua code is delimited using `@[` for starting a Lua block and `]@` for closing it.
The equivalent for the previous example would be something like
  
    Anything("Hello")
    Lua(for i=1,5 do) 
    @i@ Anything(",") 
    Lua(end)
    Anything("World")

### Output delimiters

So, what are the `@...@` ? 
The character `@` alone is used to mark a block as _output_, it's the equivalent of `std::cout` for CES.
The content of _output_ blocks is evaluated and redirected to CES output (can be whichever stream you like: cout, file, buffer, ...). 
To do this CES uses a custom method in Lua: `CES.out`:

    @i@ = CES.out(i)
  
In fact the _anything_ blocks are also redirected to the CES output using `CES.out` but not evaluated.

### Result

CES produces Lua scripts equivalent for each CES code interpreted:
  
    "Hello@[for i=1,5 do]@ @i@,@[end]@ World"
  
    // Produces
  
    CES.out("Hello")    // Hello
    for i=1,5 do        // @[for i=1,5 do]@
        CES.out(i)      // @i@
        CES.out(",")    // ,
    end                 // @[end]@
    CES.out(" World")   //  World



