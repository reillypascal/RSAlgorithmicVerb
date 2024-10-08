# RS Algorithmic Verb

Implements a number of different reverb algorithms, including plate and hall reverbs from Dattorro; Gardner's 1992 room reverbs; 4 feedback delay network reverbs using the "[FDN Toolbox](https://www.researchgate.net/publication/344467473_FDNTB_The_Feedback_Delay_Network_Toolbox)"; and two experimental/special-effect reverbs. More algorithms to come.

<!-- ![Plugin interface for a reverb plugin, with two rows of knobs; 1 larger knob for decay time; and a dropdown to select reverb algorithm in the bottom right. There is a desaturated magenta rounded rectangle around the knob area.](https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/fd7959eb-73e9-4335-b7dd-5f516fd45e06) -->

https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/25fd80de-cff3-4200-b1a6-74c84107e8cc

https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/352c736c-eec0-4abc-bbc5-bacccbba41c1

<!-- https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/a4c77f4b-dfc9-4437-9d90-56dca72af94c -->

## Build Dependencies:
- JUCE (https://juce.com/download/)
- Projucer (https://docs.juce.com/master/tutorial_new_projucer_project.html) (for creating Xcode/Visual Studio projects or Linux Makefiles in order to build)

<!--Windows:
- Compiled Windows files are available under "Releases". Unzip the files and place them in 
	- C:\Program Files\Common Files\VST3 (VST3)
	- C:\Program Files\Common Files\Avid\Audio\Plug-Ins (AAX) 
-->
## macOS:
- Compiled macOS files are available under "Releases". Unzip the files and place them in 
	- Macintosh HD/Library/Audio/Plug-Ins/Components/ (AU)
	- Macintosh HD/Library/Audio/Plug-Ins/VST3/ (VST3)
<!--	- Macintosh HD/Library/Application Support/Avid/Audio/Plug-Ins (AAX) -->
- You will likely need to disable Gatekeeper for the plugins. To do this for AU, type...
```sh
spctl --add "/Library/Audio/Plug-Ins/Components/RSAlgorithmicVerb.component"
```

...for VST3...
```sh
spctl --add "/Library/Audio/Plug-Ins/VST3/RSAlgorithmicVerb.vst3"
```

<!--...or for AAX...
```sh
spctl --add "/Library/Application Support/Avid/Audio/Plug-Ins/RSAlgorithmicVerb.aaxplugin"
```
-->
- You can also add the file path by typing...
```sh
spctl --add 
```

...(with a space at the end) and dragging the plugin file into the terminal, which will automatically add the file path.

## Linux/Windows:
Compiler targets are available for Linux and Windows. Set up JUCE on your computer, open the .jucer file in the Projucer, generate the Linux Makefile or Visual Studio project, and then you can compile the plugins.
