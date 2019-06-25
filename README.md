## Info

Small games made in 2 hours each during a 3 part SDL gamedev presentation-series sometime around 2010. Meant to be quick tutorials on how easy it is to develop meaningful games with SDL.

All code made during the presentation, including Visual C++ 2010 project files are copied here verbatim. Not meant to be pretty code, but I think they are quite readable still

Assets are not mine I think. If this bothers you, please tell me, and I will remove them immediately.

## Building and running

### Prerequisites
SDL, SDL\_image and SDL\_mixer, and their "-dev" packages, if applicable

### Building on Linux
`make`

The executables should appear in their respecitve directories. They can only be started from their own directories, because they rely on "./data"-style asset paths.

### Building on Windows
These were originally Visual Studio projects, so they naturally have a project file. They need the SDL SDK to be installed. TBH, I haven't tried building it for Windows in like 10 years.
