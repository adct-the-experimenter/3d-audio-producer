
# 3D Audio Producer

This is a fork of the binaural-audio-editor project, but wxwidgets and openscenegraph are replaced with raygui and raylib.

This project is an audio application that produces 3D audio from 2D mono audio samples and positional information given through the graphical user interface. 
Listen to 3D audio through stereo headphones or surround sound system.

Features from binaural-audio-editor will be ported to 3d audio producer.

## Examples of productions made by 3D Audio Producer
- https://www.youtube.com/watch?v=byJa1Za55PY&t=20s


## Required Libraries
- OpenAL Soft https://github.com/kcat/openal-soft

- raylib 4.2

- Boost Math Quaternion headers and Boost ASIO serial https://www.boost.org/users/history/version_1_70_0.html

- PugiXML https://github.com/zeux/pugixml/

## How to Install

1. Install required libraries.
2. Clone this repository
3. cd 3d-audio-producer
4. mkdir build
5. cd build
6. cmake .. 
7. make
8. ./3d-audio-producer

* Make sure to have C++17 support to use filesystem feature.

## Controls



## Instructions

## Control Camera

Mouse Wheel to Zoom in-out

Mouse Wheel Pressed to Pan

Alt + Mouse Wheel Pressed to Rotate

Alt + Ctrl + Mouse Wheel Pressed for Smooth Zoom

Z to zoom to (0, 0, 0)

### Control Listener
  
  Use W,A,S,D,Q,E keys to move the listener(blue cube) in 3d space.
  
### Control SoundProducers
  
  Select the sound producer(red cube) and then move the sound producer with I,J,K,L,U,O keys.

### Coordinate System of 3D Audio Producer and OpenAL Soft
  Like OpenAL, the application uses a right handed coordinate system to move listener and sound producers, where in a frontal default view X (thumb) points right,  Y points up (index finger), and Z points towards the viewer/camera (middle finger). 
  
  - Up is positive y. Down is negative y.
  
  - Back is positive z. Forward is negative z.
  
  - Right is positive x. Left is negative x.

## Important Note About Multi-channel Audio Input:
Stereo(2 channel) audio does not get 3d spatialization and is instead used to play as background music.

Have audio that you want to be played in 3D be mono(1 channel) audio
and load it into the audio track.

Stereo audio can be changed to mono audio using Audacity or other programs.

sndfile-mix-to-mono is an application that successfully turns multi-channel audio mix into mono-channel audio mix.

  
## Experimental Feature for 5.1,6.1,7.1 channel Surround Sound Output

Run the alsoft-config program that came with the installation of OpenAL Soft to
set the output to 5.1, 6.1, or 7.1 channels.
OpenAL Soft should automatically convert 3d audio information into audio output for surround sound.

For more information, read https://github.com/kcat/openal-soft/blob/master/docs/3D7.1.txt 

## To use with Digital Audio Workstation

TODO

## Shortcut Keys

### General editor keys
Key 1 - Edit whatever was picked with mouse in 3d space.

Key Delete or Key Del - Remove whatever was picked with mouse in 3d space. Also remove current selected timeline point.

### Project editor file operation keys

Control S - Save project data to file.

Control O - Load project data from file.

### Timeline keys

Key B - Add position point to timeline.


Key Z - Add start / restart playback marker.

Key X - Add pause playback marker.

Key C - Add resume playback marker.

Key V - Add end playback marker.

Key P - Show timeline parameters properties.

Key Left Arrow - Timeline frame decrement.

Key Right Arrow - Timeline frame increment.

Key Enter - Select timeline frame.

## Timeline

One timeline controls only 1 object(soundproducer, listener), and the object controlled can be selected. 

Make a new timeline to control a different object.

### To initiate playback of audio linked to sound producer.
1. Open timeline by clicking on timeline button.
2. Select sound producer from object dropdown box.
3. Select a timeline frame for start of playback of audio linked to sound producer. Line should be highlighted yellow.
4. Add start playback marker by pressing the 'z' key.
5. Click on "Play" button and wait for timeline to pass the selected timeline frame.


## Project Management

### File management
Projects save and load audio and timeline data from specific directories within the project directory.
The project directory path is automatically extracted by the program.

This allows for sharing of projects by putting the project directory and its files into a compressed
archive(.e.g .zip, .gz, etc.) 

### Creating a new project
1. Click on 'New Project' button.
2. Choose where to put the project directory.
3. Put in a name for project in the text box.
4. Press 'Select' button when finished with name. The project directory and project .xml file will be created.

### Loading a project
1. Click on 'Load Project' button.
2. Choose the project directory/folder. 
3. Open the project .xml file.
4. Press 'Select' button. 

### Saving a project
1. Click on 'Save Project' button.

## Important Details to Know
- 3D Audio Producer works with a copy of the audio data for audio files that are uploaded to sound bank.
The sound format that his copy of audio data uses is a signed 32-bit integer format.
This is done to prevent corruption of audio files.

## Feedback 

Please email questions or comments to this email address bringerofawesomefood @ gmail . com without spaces.
