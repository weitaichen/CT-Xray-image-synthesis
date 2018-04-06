# CT-Xray-image-synthesis
Extract 2D image from CT data and synthesize X-ray like image using OpenCV3.0 library on Visual studio 2012 platform. This program provide two functions: 1. Extract a 2D slice image from 3D CT data in any indicated angle and position. 2. Synthesize a 2D X-Ray-like image from 3D CT data by integrating all 2D slicing image in same angle but different postion.


## How can you get started
https://docs.opencv.org/2.4/doc/tutorials/introduction/windows_visual_studio_Opencv/windows_visual_studio_Opencv.html<br />
https://www.youtube.com/watch?v=l4372qtZ4dc<br />
Above are tutorial webisites which tell you how to install OpenCV library on visual studio platform, following are steps:<br />
* Download OpenCV on https://opencv.org/releases.html<br />
* Setting environment variable:PC->Properties->Advanced system setting->Environment variables->Path<br />
->Edit->New->OpenCV bin directory<br />
* Open new projects: Open Visual studio->File->New project->Visual C++->Win32 Console application->Finish<br />
* Choose configuration: Configuration manager->Active solution platform->New->Choose x86 or x64 platform <br />
* Set include path: Project->Properties->C/C++->Additional include directories->Opencv include directory<br />
* Add library directories:Project->Properties->linker->General->Additional library directories->Opencv lib directory<br />
* Add library dependency:Project->Properties->linker->Input->Additional dependecies->opencv_world300d.lib<br />
* Download msvcp120d.dll and msvcr120d.dll to main.exe folder <br />
* Copy all file into your project folder<br />
* Build project and find main.exe directory, download Fig folder and put it into main.exe folder  <br />
* Open cmd.exe and go to your debug directory where main.exe was generated <br />
* command variable: main.exe, method, x_rotation_angle, y_rotation_angle, z_rotation_angle, slice_distance  <br />
* Enter command "main.exe 1 90 0 0 10" <br />
* Have fun! <br />


## How does this work
CT data
<p align="center"><img src="/fig1.png" height="100%" width="100%"></p><br />
<p align="center"><img src="/fig2.png" height="80%" width="80%"></p><br />

