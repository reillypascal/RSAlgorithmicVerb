RSAlgorithmicVerb
- Implements a number of different reverb algorithms, including Dattorro's 1997 plate reverb "in the style of Griesinger," the popular "Freeverb" Schroeder reverb, as well as FDN options.

macOS:
- Compiled macOS files are available under "Releases"
- Put these files in Library/Audio/Plug-Ins/Components/ (for AU) or Library/Audio/Plug-Ins/VST3/ (for VST)
- You will likely need to disable Gatekeeper for the plugins. To do this for AU, type...
```sh
	spctl --add "/Library/Audio/Plug-Ins/VST3/RSAlgorithmicVerb.component"
```

...or change the file extension to ".vst3" for VST.
- You can also add the file path by typing...
```sh
	spctl --add 
```

...(with a space at the end) and dragging the plugin file into the terminal, which will automatically add the file path.

Windows:
- For Windows, you will need to compile the plugin yourself
	- Install JUCE and the Projucer (https://docs.juce.com/master/tutorial_new_projucer_project.html)
	- Install Visual Studio (Community Edition is free: https://visualstudio.microsoft.com/vs/community/)
	- Open the .jucer file in the Projucer, export it to Visual Studio, and build from there.