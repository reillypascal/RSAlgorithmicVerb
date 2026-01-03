# RS Algorithmic Verb

Implements a number of different reverb algorithms, including plate and hall reverbs from Dattorro; Gardner's 1992 room reverbs; 4 feedback delay network reverbs using the "[FDN Toolbox](https://www.researchgate.net/publication/344467473_FDNTB_The_Feedback_Delay_Network_Toolbox)"; and two experimental/special-effect reverbs. More algorithms to come.

<!-- ![Plugin interface for a reverb plugin, with two rows of knobs; 1 larger knob for decay time; and a dropdown to select reverb algorithm in the bottom right. There is a desaturated magenta rounded rectangle around the knob area.](https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/fd7959eb-73e9-4335-b7dd-5f516fd45e06) -->

<https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/25fd80de-cff3-4200-b1a6-74c84107e8cc>

<https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/352c736c-eec0-4abc-bbc5-bacccbba41c1>

<!-- https://github.com/reillypascal/RSAlgorithmicVerb/assets/94489575/a4c77f4b-dfc9-4437-9d90-56dca72af94c -->

# Precompiled Releases

- Compiled Linux, macOS, and Windows files are available under "Releases". Unzip the files and place them in the directories listed below for your operating system.
- Note that you will likely need to disable Gatekeeper for the plugins on macOS. To do this for AU, type...

```sh
xattr -rc "/Library/Audio/Plug-Ins/Components/RSAlgorithmicVerb.component"
```

...for VST3...

```sh
xattr -rc "/Library/Audio/Plug-Ins/VST3/RSAlgorithmicVerb.vst3"
```

<!--...or for AAX...

```sh
spctl --add "/Library/Application Support/Avid/Audio/Plug-Ins/RSAlgorithmicVerb.aaxplugin"
``` -->

- You can also add the file path by typing...

```sh
xattr -rc 
```

...(with a space at the end) and dragging the plugin file into the terminal, which will automatically add the file path.

## Linux Paths

- `~/.vst3/` (VST3 user folder)
- `/usr/lib/vst3/` (VST3 global folder)
<!--- `/usr/local/lib/vst3/` (VST3 global folder—lower priority)-->

## macOS Paths

- `/Library/Audio/Plug-Ins/Components/` (AU)
- `/Library/Audio/Plug-Ins/VST3/` (VST3)
<!--	- `/Library/Application Support/Avid/Audio/Plug-Ins` (AAX) -->

## Windows Paths

- `C:\Program Files\Common Files\VST3` (VST3)
<!-- - C:\Program Files\Common Files\Avid\Audio\Plug-Ins (AAX) -->

# Building from Source

## Build Dependencies

- JUCE (<https://juce.com/download/>)
- Either:
    - Projucer (<https://docs.juce.com/master/tutorial_new_projucer_project.html>) (for creating Xcode/Visual Studio projects or Linux Makefiles in order to build)
    - CMake

## Compiling with the Projucer

Download the [JUCE repo](https://github.com/juce-framework/JUCE) onto your computer. You will need to set the Projucer so that it knows where this folder is. Open the .jucer file in the Projucer, generate the Linux Makefile, Visual Studio project, or Xcode project, and then you can compile the plugins using those respective tools. Alternatively, see below for CMake instructions.

## Compiling with CMake

<!-- - Clone the [JUCE repo](https://github.com/juce-framework/JUCE) into the working folder (`git clone https://github.com/juce-framework/JUCE.git`) -->

- Clone JUCE as a Git submodule by running the following:

```sh
git submodule update --init --recursive
```

- Run the following for a CLI build. Note that you may need to run these with `sudo`:

```sh
# sets up a default build:
cmake -S . -B build
# alternatively, you can explicitly set up a debug or release build:
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Debug
# or
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release

# you can add the flag -D COPY_PLUGIN_AFTER_BUILD=TRUE to copy the built files to the default location on macOS

# after running one of the above three options, run
cmake --build build
```

You can also use CMake to generate Xcode/Visual Studio projects.

- Run the following to generate an Xcode project:

```sh
cmake -B Builds -G Xcode
```

- Run the following to generate a Visual Studio project:

```sh
cmake -B Builds -G "Visual Studio 17 2022"
```

### Debugging

`launch.json` sets up the ability to launch an app of your choice (e.g., REAPER, JUCE's AudioPluginHost, etc.) as part of a debugging session. Change the path for the app in `launch.json` to match the one on your system.

<!-- You can configure which app in your editor; e.g., for Zed, see [the debugger documentation](https://zed.dev/docs/debugger#configuration). -->

You can also use [Pluginval](https://github.com/Tracktion/pluginval). After installing Pluginval and adding the binary to your PATH, run `pluginval --strictness-level 5 <path-to-plugin>`. Strictness level 5 is the baseline, and it can go up to 10 for more rigorous testing.

Pluginval uses aufx under the hood for AU plugins on macOS. You can run auval directly with `auval -strict -v aufx Rsav Rspi` if this plugin is installed on the computer.
