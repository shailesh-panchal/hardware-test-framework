# Hardware Test Framework

Hardware Test Framework is a **SOC-Agnostic and Hardware-Independent Validation Framework** designed for Linux-based embedded systems. Its primary goal is to establish a **standardized, reusable, and scalable testing methodology** that works across different hardware platforms without requiring test-case modifications.

The framework separates **logical functionality** from **hardware implementation** using a layered architecture based on:

* Device abstraction
* Platform-specific bindings
* Function abstraction
* Configuration-driven test definitions
* Plugin-based hardware access

This approach allows the same validation suite to run on different SoCs and boards by changing only platform configuration files, while keeping test logic completely independent of hardware details.

## Key Objectives

* Provide a unified validation framework for Linux-based embedded platforms.
* Eliminate hardware-specific code from test cases.
* Enable reuse of tests across multiple SoCs and hardware variants.
* Support platform-specific hardware through configuration and plugins.
* Standardize validation processes across projects and product lines.
* Reduce test development and maintenance effort.
* Enable automated manufacturing, validation, and regression testing.

## Core Design Principles

### Hardware Independence

Tests never reference:

* GPIO numbers
* I2C bus numbers
* SPI device paths
* Camera device nodes
* SoC-specific resources

### SoC Agnostic Architecture

Support multiple platforms such as:

* STM32MP
* NXP i.MX Series
* TI AM62x
* NVIDIA Jetson
* Custom Linux-based Embedded Platforms

without changing test implementations.

### Configuration-Driven Design

The framework uses:

```text
device.yaml/jsonc
platform.yaml/jsonc
function.yaml/jsonc
test.yaml/jsonc
```

to describe:

```text
Logical Devices
Platform Bindings
System Functions
Validation Tests
```

### Plugin-Based Hardware Access

Hardware-specific implementations are isolated within plugins:

```text
gpio_linux
i2c_linux
spi_linux
v4l2_camera
simulation_backend
```

allowing the core framework to remain platform independent.

## Architecture Overview

```text
                +----------------+
                |   Test Engine  |
                +--------+-------+
                         |
                         v
                +----------------+
                | Function Manager|
                +--------+-------+
                         |
                         v
                +----------------+
                | Device Manager |
                +--------+-------+
                         |
                         v
                +----------------+
                | Plugin Manager |
                +--------+-------+
                         |
                         v
                +----------------+
                | HAL Interface  |
                +--------+-------+
                         |
                         v
                +----------------+
                | Linux Hardware |
                +----------------+
```

## Benefits

* Write tests once, run anywhere.
* Simplified support for new hardware platforms.
* Consistent validation process across products.
* Reduced maintenance cost.
* Scalable architecture for future devices and peripherals.
* Suitable for development, validation, manufacturing, and regression testing.

## Target Applications

* AI Edge Devices
* Industrial Gateways
* IoT Devices
* Embedded Linux Platforms
* Custom ARM-Based Systems
* Manufacturing Test Stations
* Hardware Validation Labs

## Build

### Prerequisites

The following tools are required:

- CMake 3.16 or later
- GCC or Clang compiler
- Linux development environment

### Clone Repository

```bash
git clone https://github.com/<your-org>/hardware-test-framework.git
cd hardware-test-framework
```

### Configure Build

Create an out-of-tree build directory and generate build files:

```bash
mkdir build
cd build

cmake ..
```
