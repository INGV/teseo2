# Teseo2: Turn the Eldest Seismograms into the Electronic Original Ones

Teseo2 is a software tool for quick and accurate digitization/vectorization of seismogram traces from raster files.

It has been developed in the framework of the Sismos project [Michelini and the Sismos Team, 2005] at Istituto Nazionale di Geofisica e Vulcanologia (Italy). This name was inspired by the myth of Theseus and it is also an acronym for Turn the Eldest Seismograms into the Electronic Original Ones.

Teseo2 is a plug-in for GIMP - GNU Image Manipulation Program - that extends its functionalities for seismological studies. The GIMP is a multiplatform photo manipulation tool freely distributed. It works on many operating systems, in many languages.

Teseo2 allows primarily for:

  - additional operations on the vectorized trace (i.e. resampling and alignment)
  - supervised vectorization algorithms (colour weighted mean)
  - analysis after trace vectorization, such as curvature correction and time realignment
  - trace import/export in several formats (such as SAC, SVG, DXF, ASCII, Timemarks distances).

In order to keep track of the stages and parameters of a seismogram vectorization, Teseo2 is able to write this information into the image saved in xcf format.

Teseo2 is developed following the "Open-Source" philosophy and it is freely distributed under GPL license. It is cross-platform and the sources, the binaries for Linux, Windows and Mac OS X, are periodically updated on the Sismos web site.

- Website: [http://teseo.rm.ingv.it/](http://teseo.rm.ingv.it/)
- Developer e-mail: *teseo [at] ingv.it*
- User mailing list: *teseo-user [at] yahoogroups.com*
- Mailing list archive: http://groups.yahoo.com/group/teseo-user/ 

### Build

#### Requirements

  - gimp-2.2.9 or newer

The configure scripts might fail asking you the dependency libraries you have to install or to specify into PKG\_CONFIG\_PATH.

#### Quick installation

```
$ tar xzvf teseo*.tar.gz
$ cd teseo2/
$ cd gtk-addons
$ ./configure && make && sudo make install
$ cd ../newuoa
$ ./configure F77=<fortran-path> && make && sudo make install
$ cd ../teseo-2
$ ./configure && make && sudo make install
```

Details in file ./teseo-2/INSTALL

### Docker

#### Build Docker Image

```
make -f DockerMakefile build
```

#### Run Docker Image

Docker image is based on *xhost*, then you need to enable *xhost* for your local IP. For connecting from any client run `xhost +`.

```
make -f DockerMakefile run
```

### Authors

Stefano Pintore and Matteo Quintiliani.

### Contributors

Diego Franceschi, Antoine Schlupp, Daniel Amorese.

### References

Michelini, A. and the Sismos Team (2005). *Collection, digitization and distribution of historical seismological data at INGV.* EOS, 86(28). 

Pintore S., Quintiliani M., Franceschi D. (2005) *Teseo: a vectoriser of historical seismograms.* Computers & Geosciences, Volume 31, Issue 10, 2005, Pages 1277-1285, ISSN 0098-3004, [http://dx.doi.org/10.1016/j.cageo.2005.04.001](http://dx.doi.org/10.1016/j.cageo.2005.04.001)

Schlupp, Antoine & Cisternas, Armando. (2007). **Source history of the 1905 great Mongolian earthquakes (Tsetserleg, Bolnay).** Geophysical Journal International. 169. 1115 - 1131. 10.1111/j.1365-246X.2007.03323.x. 

Cadek, O. (1987). *Studying earthquake ground motion in prague from wiechert seismograph records.* Gerl. Beitr. Geoph., 96:438-447.