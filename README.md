# Process String Analyzer
a tool for analyzing WIN32 process allocated memory and find ASCII -or- UNICODE strings 

Main components:
```sh
$ (1) simple server -
    uses .NET 4.0 (Client profile)
$ (2) simple client - 
    uses C/C++ 11/14 console application (toolset v141, vs2017)
```
  
Usage:
```sh
$ (1) start server (i.e. SimpleServer IP Port)
$ (2) run client (i.e. SimpleClient ServerIP ServerPort)
```

Notes:
  * currently memory analyzer just simpley analyzes explorer.exe (just as a demo) which is a hard-coded pick.
  * feel free to modify and extend it to make it generic
