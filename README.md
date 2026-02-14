# IoTFoundry Linux Endpoint

![License](https://img.shields.io/github/license/PICMG/iot-foundry-linux-endpoint)
![Coverage](https://img.shields.io/codecov/c/github/PICMG/iot-foundry-linux-endpoint)
![Issues](https://img.shields.io/github/issues/PICMG/iot-foundry-linux-endpoint)
![Forks](https://img.shields.io/github/forks/PICMG/iot-foundry-linux-endpoint)
![Stars](https://img.shields.io/github/stars/PICMG/iot-foundry-linux-endpoint)
![Last Commit](https://img.shields.io/github/last-commit/PICMG/iot-foundry-linux-endpoint)

This project implements an IoTFoundry serial MCTP/PLDM endpoint for the embedded linux endpoints.

This code implements endpoint behavior for remote nodes.  This is destinct behavior from the native Linux support, which provides for routing of mctp messages.

The code and build process in this project relies upon the template code found in the IoTFoundry endpoint project on github (https://github.com/PICMG/iot-foundry-endpoint) to implement the core features.  This project implements a platform-specific interface layer, and platform-specific build process.

This repository is part of the IoTFoundry family of open source projects.  For more information about IoTFoundry, please visit the main IoTFoundry site at: [https://picmg.github.io/iot-foundry/](https://picmg.github.io/iot-foundry/)

## Cloning the Repository

This project uses git submodules for external dependencies. When cloning, use the `--recursive` flag to automatically initialize and update the submodules:

```bash
git clone --recursive https://github.com/PICMG/iot-foundry-linux-endpoint.git
```

If you've already cloned the repository without the `--recursive` flag, you can initialize and update the submodules with:

```bash
git submodule update --init --recursive
```

To synchronize submodules after pulling changes that update submodule references:

```bash
git submodule update --recursive
```

## System Requirements
The following are system requirements for building/testing the code in this library.

- Linux with the GNU toolchain (GCC) installed
- CMake 3.15 or higher
- At least one serial port (e.g. /dev/ttyS0)
- A serial null-modem cable to connect to remote client
- wget (for downloading IoTFoundry core template)
- Git (for submodule management)

## Repository Resources

- `CONTRIBUTING.md` — instructions for contributing to this project.
- `LICENSE` — The license for this project (MIT)
- `CMakeLists.txt` — CMake build configuration.
- `README.md` — this document.
- `include/` — public headers and generator output.
  - `include/core/` — (template core includes, downloaded during build)
- `src/` — application and platform C sources
  - `src/core/` — (template core sources, downloaded during build)
- `subprojects/` — git submodules for external dependencies
  - `subprojects/libmctp/` — libmctp source files (used directly, not as library)
  - `subprojects/libpldm/` — PLDM protocol definitions and utilities
- `tests/` — test scripts and requirements for host-side tests and tooling.

## Build Flow

This project uses CMake as its build system. The build process automatically downloads the IoTFoundry core template and integrates libmctp source files from the git submodule.

### Building the Project

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure the project with CMake:
```bash
cmake ..
```

3. Build the endpoint:
```bash
make
```

The compiled `endpoint` binary will be in the `build` directory.

### Build Options

- `PLDM_SUPPORT`: Enable PLDM support (default: ON)
  ```bash
  cmake -DPLDM_SUPPORT=OFF ..
  ```

### Additional Targets

- `make download-core` — Download IoTFoundry core sources (runs automatically during build)
- `make clean-core` — Remove downloaded core sources
- `make install` — Install the endpoint binary (default: /usr/local/bin)

## Running Device Tests

Start the endpoint code on your unit under test (UUT) with the following command:
```bash
./build/endpoint --tty <target tty path> --baud <target baud rate>

# for example
./endpoint --tty /dev/ttyS0 --baud 9600
```
On the remote client, make sure the python requirements are intalled, and launch the test runner:
```bash
python3 -m pip install -r tests/requirements.txt /dev/ttySx 9600
```
**NOTE**: ttySx should be replaced with the serial device path for the port on the test client that connects to the UUT.  This port may not be the same as the port used on the UUT.


