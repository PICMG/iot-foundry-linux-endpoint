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

## System Requirements
The following are system requirements for buidling/testing teh code in this library.

- Linux with the gnu toolchain and make tools installed.
- At least one serial port (e.g. /dev/ttyS0)
- A serial null-modem cable to connect to remote client.

## Repository Resources

- `CONRIBUTING.md` — instructions for contributing to this project.
- `LICENSE` — The license for this project (MIT)
- `Makefile` — build, generation and flash recipes.
- `README.md` — this document.
- `include/` — public headers and generator output (see `include/generated_serial_config.h`).
  - `include/core` — (template core includes)
- `src/` — application and platform C sources
  - `src/core/` — (template core sources).
- `tests/` — test scripts and requirements for host-side tests and tooling.

## Build Flow

This project contains a single makefile recepie to build IoTFoundry Linux endpoint.  This is accomplished with a simple  'make' command:

```bash
make
```

## Running device tests

Start the endpoint code on your unit under test (UUT) with the following command:
```bash
./endpoint --tty <target tty path> --baud <target baud rate>

# for example
./endpoint --tty /dev/ttyS0 --baud 9600
```
On the remote client, make sure the python requirements are intalled, and launch the test runner:
```bash
python3 -m pip install -r tests/requirements.txt /dev/ttySx 9600
```
**NOTE**: ttySx should be replaced with the serial device path for the port on the test client that connects to the UUT.  This port may not be the same as the port used on the UUT.


