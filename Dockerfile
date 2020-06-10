FROM fedora:30

RUN dnf install gcc g++ make cmake wget expat-devel gdb qt5-devel libX11-devel freeglut-devel libXmu-devel libconfig libconfig-devel -y

RUN mkdir -p /builddir/geant4/

RUN cd /builddir/geant4 && wget https://github.com/Geant4/geant4/archive/v10.6.1.tar.gz && tar xvzf v10.6.1.tar.gz && rm v10.6.1.tar.gz && mkdir /builddir/geant4/build

RUN cd /builddir/geant4/build && cmake -DGEANT4_USE_QT=ON -DGEANT4_USE_RAYTRACER_X11=ON -DGEANT4_USE_OPENGL_X11=ON ../geant4-10.6.1 && cmake -DGEANT4_INSTALL_DATA=ON . && make -j4 && make install

LABEL build podman run --pod=sim --name=sim-build -it -v ./source/:/builddir/source:Z IMAGE '/builddir/source/containerbuild build'
LABEL clean podman run --pod=sim --name=sim-clean -it -v ./source/:/builddir/source:Z IMAGE '/builddir/source/containerbuild clean'
LABEL run podman run --pod=sim --name=sim-run -it -v ./source/:/builddir/source:Z IMAGE '/builddir/source/containerbuild run'
LABEL shell podman run --pod=sim --name=sim-shell -it -v ./source/:/builddir/source:Z IMAGE '/bin/bash'
