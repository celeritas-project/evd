Evd
===


# About

`Evd` **is the Event Display of the Celeritas Project**, built using the EVE
Environment [1]. It provides full visualization control of a gdml geometry, and
can use step data information to draw particle tracks from a simulated event.

`Evd` can visualize any gdml file, and can read ROOT output files produced by
the **Geant4-Sandbox** [2] in order to draw the simulated particle trajectories.
The event drawing is done solely by the `Evd::AddEvent(...)` method. The class
is expected to be easily adapted to read other simulation files, including
future `Celeritas` simulation outputs.


# Dependencies

* ROOT


# How to install

From the Evd folder:

```
$ mkdir build  
$ cd build  
$ cmake ..  
$ make
```


# How to run

```
$ ./evd [parameters and flags]
```

## Parameters & flags

`geometry.gdml`: Loads a gdml input file. This is the only mandatory input.  
`-vis [visLevel]`: Sets the visualization level of the gdml. Higher values =
more details. Default: `1`.  
`-noworld`: Draws the volumes inside the world volume, but not the world volume
itself.  
`rootFile.root`: Loads a geant4-sandbox root file for displaying events.  
`-e [evt]`: Sets the event number to be displayed. Default: `0`.  
`-n [ntracks]`: Sets the maximum number of tracks to be displayed. If set to `0`
it will print all tracks in the event. Default: `1`.

**[TEMPORARY (?) FLAG]**  
`-cms`: Loads the CMS geometry without loading the surrounding cms building and
setting LHC beamline objects as invisible for a better view of the CMS geometry.

## Keyboard / mouse commands
`Holding mouse left click`: Rotate view.  
`Mouse/trackpad scroll`: Zoom in/out.  
`Command + holding mouse left click`: Pan view.  
`W / E / R / T`: Toggle between different view options.  
`J / K`: Zoom in/out.  
`shift / ctrl`: Increase / decrease action rate. Eg. `shift + J/K` zooms in/out
at larger steps. Valid for `mouse` actions as well.


# References

[1] M. Tadel. **Overview of EVE – the event visualization environment of ROOT**.
2010 *J. Phys.: Conf. Ser.* **219** 042055.  
[2] <https://github.com/celeritas-project/geant4-sandbox>


# Notes / Issues

`void Evd::AddEvent(const int &event, const int &trackLimit)`   
Currently, this class method limits the drawing of steps around CMS. Axes
conditions are found at:

```
// Skipping steps that are too far from the CMS detector
if (TMath::Abs(x) > 1500) continue;
if (TMath::Abs(y) > 1500) continue;
if (TMath::Abs(z) > 1500) continue;
```

The current code is very simple. There are no safeguards for wrong files or
misused flags, so expect segfaults if you miss something.

**Issue**: On Macs, clicking the `x` button at the top left corner to close
the window causes ROOT to crash. Typing `.q` in the terminal or using the `Quit
ROOT`  option in the Browser's menu avoids that.

___
