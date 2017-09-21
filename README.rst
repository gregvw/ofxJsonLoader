ofxJsonLoader 
=============

*NOTE: This guide is written for Linux users.*

Using JavaScript Object Notation (JSON) with openFrameworks

The aim of this example is to demonstrate how you can create
a set of input parameters for your openFrameworks project
that will allow you try different values without having to 
recompile your code after every change. JSON is one popular
format that allows us to specify values of a wide variety 
of types in human-readable text format and jsoncpp imports
the contents of these files so that they are "C++-readable"
as well. You can read about JSON format at http://www.json.org

*NOTE:* Let $ indicate a terminal prompt so that when you see

`$ command`

It means you are to type "command" after a terminal prompt
and hit enter.


Installing ofxJsonLoader
------------------------

From your .../openFrameworks/addons directory type

$ git clone https://github.com/gregvw/ofxJsonLoader.git

of if you use ssh

$ git clone git@github.com:gregvw/ofxJsonLoader.git

then 

$ cd ofxJsonLoader

$ mv jsonParticlesExample .../openFrameworks/examples/


Getting and installing jsoncpp
------------------------------

The next order of business is in get jsoncpp which will 
read .json input files and write values found in them 
to variables in your C++ code. If you are connected to
a network, open a terminal and type 


`$ git clone https://github.com/open-source-parsers/jsoncpp.git`

or

$ git clone git@github.com:open-source-parsers/jsoncpp.git


After this downloads, do the following:

`$ cd jsoncpp
$ mkdir build  
$ cd build`


Generate a Makefile to build jsoncpp in the build directory 
using the CMake build system

$ cmake .. 

This command will compile and link all the jsoncpp components
from the source code you downloaded above. The flag -j tells
make to build in parallel using multiple processing units 
(probably 2 times the number of cores in your CPU) so that -j4 
would build using four processing unites . The flag -j`nproc` tells 
make  to use as many processing units as your computer has to 
perform the build. Usually this speeds things up. 

$ make -j`nproc`

Now you need to install the headers and libraries for jsoncpp. 
If you have administrator rights on your computer, you can type

$ sudo make install

and enter your password. By default, this will install headers to 
/usr/local/include and the compiled library to /usr/local/lib

If you don't have sudo access, you can still install to a path 
inside your home directory. For example:

$ export DESTDIR="$HOME/lib/jsoncpp/"




Using JSON in your openFrameworks projects
------------------------------------------

The include example has been copied and modified from 

/openFrameworks/examples/math/particlesExample

which has in its /src directory the following five files:

main.cpp
ofApp.h
ofApp.cpp
demoParticle.h
demoParticle.cpp

Each of the original .cpp files contains values that are 
"hard-coded." They are fixed at compile time, which means that 
every time you want to see what happens when you change value,
you must edit, save, and recompile the code. Some of these
values have been replaced with ones to be read from 
data/parameters.json


Building the Project
--------------------

If you were able to install json to /usr/local/lib and 
/usr/local/include above, then make should be able to find them 
automatically and you will not get any compiler error complaining 
that it can't find json.h. The header tells the compiler what 
constitutes legitimate calls to jsoncpp components,
but not how to actually use them. This is done in the jsoncpp 
library you built earlier. You may get  linker errors saying 
things like 

undefined reference to 'Json::Reader::Reader()'

This means that you need to tell make what it should do to find
these references. One way to do this is to open the file 
config.make in the project and find the line

# PROJECT_LDFLAGS=-Wl,-rpath=./libs

and change it to 
PROJECT_LDFLAGS=-ljsoncpp

This tells make that it needs to link your object code to 
the jsoncpp library which actually does the parsing and 
reading/writing values that we need. 

If you installed jsoncpp elsewhere you will need to do one of
the following:

1. Add it your library path

  $ export LD_LIBRARY_PATH=/path/to/your/libjsoncpp:$LD_LIBRARY_PATH

2. Edit config.make to specify the library path as well as the link
   command
  
   PROJECT_LDFLAGS=-ljsoncpp -L/path/to/your/libjsoncpp

   You may also need to specify the source path to find the headers

   PROJECT_EXTERNAL_SOURCE_PATHS = /path/to/your/json.h
  
  







