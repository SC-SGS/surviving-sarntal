# DevOps Documentation FA 2023

## Frameworks/programming languages
The game is a C++ project with the building framework CMake.
As controls a gamepad or keyboard can be used.
Additionally, it is possible to play with XBox Kinect.

The following libraries were used:
- raylib as programming library
- raygui to create the GUI
- flecs as entity component system  
- for Kinect functionality: SensorKinect, OpenNI and NITE

We used no extra library for the physics. 

For the libraries it was planned to use vcpkg, to easily download and install the needed packages, just like for
example with Maven (Java) or pip (Python). However, we quickly realized that it was not a good idea as vcpkg for example
does not install dependencies of the packages automatically and in general lacks many features which Maven or pip have.
So we decided against using vcpkg and just downloaded the few needed libraries and included them in our repo.

## Structure and building of the project
We used two different CMake configurations: One normal configuration and an extra one for Kinect.
Both configs were created using only one CMakeLists.txt and with a CMake Kinect environment variable.
The reason for separating the building of Kinect and non-Kinect is that for Kinect three additional libraries are
necessary which also have to be installed before (including dependencies).
So the CMakeLists.txt only includes these libraries if the Kinect flag was given, so that the game can also be built and
played without Kinect.

## Software on the server
For our server/workstation we used Ubuntu 22.04.
We installed GitLab (CE) for local code management and Nextcloud (the lightweight free version, NOT the AIO) as local picture storage.  
Both software have been installed in Docker containers. For the CI/CD we used the gitlab-runner feature of GitLab.
The runner itself was directly installed on the server and also build directly on the server,
although it may be a better idea to build using a Docker image.  
Usage of HTTPS was very difficult. Because the server was not publicly reachable we could not get a proper SSL certificate
and therefore tried using self-signed certificates.
But this lead to more problems because almost none of the browsers and also the software (especially GitLab) did really
support self-signed certs.
We figured out that just using plain HTTP was way easier than HTTPS (and also sufficient for a non-public server).

## Backups
It was a good idea to create some backup scripts for GitLab and Nextcloud before going to Sarntal.