# Overview of Systems in 3D Audio Producer

These are the main three systems that 3D Audio Producer interfaces with.
- Main Graphical User Interface, GUI
- Sound Bank
- Immediate Mode Sound Player
- OpenALSoftAudioEngine 

## Main Graphical User Interface, GUI
This is what puts together everything seen in 3D Audio Producer.


### Files to look at:
- Files with Dialog in the filename in include and src folder.
- `main_gui_editor.h`
- `main_gui_editor.cpp`

## Sound Bank
Performs audio decoding and stores audio data into files

## Immediate Mode Sound Player
Manages audio playback.

### Files to look at:
- `immediate_mode_sound_player.h`
- `immediate_mode_sound_player.cpp`

## OpenALSoftAudioEngine 
This is a class to abstract 3D audio operations involving OpenAL Soft.

It is mostly just used to initialize the OpenAL system and query and test HRTF support.

### Files to look at
- `openalsoftaudioengine.h` 
- `openalsoftaudioengine.cpp`

