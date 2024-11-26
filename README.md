# About
`Evd` is a geometry and event display built using the EVE Environment[^1]. It
loads GDML geometries and ROOT MC truth outputs from both [Geant4 Validation
App](https://github.com/celeritas-project/benchmarks) and Celeritas
(`RootStepWriter`) to draw simulated events within the geometry.

[^1]: M. Tadel. **Overview of EVE – the event visualization environment of
ROOT**. 2010 *J. Phys.: Conf. Ser.* **219** 042055.

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
- `geometry.gdml`: Load the gdml geometry. **This is the only mandatory
  input**.  
- `simulation.root`: Load the simulation run. Compatible with
  [benchmarks/geant4_validation_app](https://github.com/celeritas-project/benchmarks/tree/main/geant4-validation-app)
  and `celeritas::RootStepWriter`.  
- `-vis [vis_level]`: Set the visualization level of the gdml. Higher values =
  more details. Default value is `1`.  
- `-noworld`: Hide world volme.  
- `-e [event_id]`: Event number to be displayed. If negative, all events are
  drawn. Default: `0`.  
- `-s`: Show step points.  
- `-cms`: For `cms2018.gdml` only. Load the CMS geometry without the
  surrounding building and set the LHC beamline to invisible.

## Interface
### Keyboard / mouse commands
- `Mouse left click`: Rotate view.  
- `cmd + mouse left click`: Pan view.  
- `Mouse/trackpad scroll or j/k`: Zoom.  
- `w/e/r/t`: Toggle between different views.  
- `shift/ctrl`: Increase / decrease action rate. E.g. `shift + j/k` zooms in/out
  at larger steps. Valid for `mouse` actions as well.

### Event track list
Particle tracks are shown in the `event` directory and named using the
convention `[event_id]_[track_id]_[particle_name_or_pdg]`. PDG is only used if
it is not mapped to a name in `MCTruthViewerInterface`.

# Development
- To read events from different ROOT files, add a new concrete implementation of
  `MCTruthViewerInterface` and call it in `EventViewer`.
- TEve issue: on macOS, the `x` close button of the evd window causes ROOT to
  crash. Typing `.q` in the terminal or using the `Quit ROOT` option in the
  Browser menu avoids that.
 