# About
`Evd` is a geometry and event display built using the EVE Environment[^1]. It
loads gdml geometries and ROOT outputs from the [Geant4 Validation
App](https://github.com/celeritas-project/benchmarks) to draw simulated events
within the geometry.


# Dependencies
- ROOT


# Build
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```


# Run
```shell
$ ./evd geometry.gdml [simulation.root] [flags]
```

## Input files and flags
- `geometry.gdml`: Loads the gdml geometry. **This is the only mandatory
  input**.  
- `simulation.root`: Loads the simulation run.  
- `-vis [vis_level]`: Sets the visualization level of the gdml. Higher values =
  more details. Default: `1`.  
- `-e [event_id]`: Event number to be displayed. If negative, all events are
  drawn (careful if file is large). Default: `0`.  
- `-cms`: For `cms2018.gdml` only. Loads the CMS geometry without the
  surrounding building and sets the LHC beamline to invisible.


## Keyboard / mouse commands
- `Mouse left click`: Rotate view.  
- `cmd + mouse left click`: Pan view.  
- `Mouse/trackpad scroll or j/k`: Zoom.  
- `w/e/r/t`: Toggle between different views.  
- `shift/ctrl`: Increase / decrease action rate. Eg. `shift + j/k` zooms in/out
  at larger steps. Valid for `mouse` actions as well.


# References
[^1]: M. Tadel. **Overview of EVE – the event visualization environment of
ROOT**. 2010 *J. Phys.: Conf. Ser.* **219** 042055.


# Development notes
The `x` close button of the evd window causes ROOT to crash. Typing `.q` in the
terminal or using the `Quit ROOT` option in the Browser's menu avoids that.
