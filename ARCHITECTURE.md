# SOC-Agnostic Hardware Test Framework - Architecture Overview

## System Architecture Diagram

```
┌──────────────────────────────────────────────────────────────────┐
│                      LAVA Framework (Optional)                   │
│                   Job Scheduling, Power Control                  │
└──────────────────────────┬───────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                    validator Application                         │
│                    (Main Entry Point)                            │
└──────────────────────────┬───────────────────────────────────────┘
                           │
     ┌─────────────────────┼─────────────────────┐
     │                     │                     │
     ▼                     ▼                     ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────────┐
│ CLI Parser   │  │ Config Loader│  │ Plugin Manager   │
└──────────────┘  └──────────────┘  └──────────────────┘
     │                 │                     │
     └─────────────────┼─────────────────────┘
                       ▼
        ┌──────────────────────────────┐
        │   RuntimeConfig Merger       │
        │  (YAML/JSON Merge: Common +  │
        │   Platform-Specific)         │
        └──────────────────────────────┘
                       ▼
    ┌──────────────────────────────────────────────┐
    │         Unified Runtime Configuration        │
    │  - Device definitions                        │
    │  - Hardware bindings                         │
    │  - Function mappings                         │
    │  - Capability list                           │
    │  - Test metadata                             │
    └──────────────────────────────────────────────┘
                       ▼
    ┌──────────────────────────────────────────────┐
    │    Core Management Components                │
    │  ┌────────────────────────────────────────┐  │
    │  │ Device Manager                         │  │  Runtime device registry
    │  │ - Stores HAL device objects            │  │
    │  │ - Provides lookup by name/type         │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ Function Manager                       │  │  Logical abstraction layer
    │  │ - Maps functions to devices            │  │
    │  │ - Validates dependencies               │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ Capability Manager                     │  │  Availability tracking
    │  │ - Tracks available capabilities        │  │
    │  │ - Filters tests by capability          │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ Test Registry                          │  │  Test discovery
    │  │ - Stores test definitions              │  │
    │  │ - Filters by group/capability          │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ Test Engine                            │  │  Test execution
    │  │ - Executes tests with timeout          │  │
    │  │ - Captures results                     │  │
    │  └────────────────────────────────────────┘  │
    └──────────────────────────────────────────────┘
                       ▼
    ┌──────────────────────────────────────────────┐
    │         HAL Interface Layer                  │
    │  (Hardware Abstraction - Generic)            │
    │  ┌────────────────────────────────────────┐  │
    │  │ IGPIO      II2CDevice   ISPI   ICamera │  │
    │  │ IEeprom    ISensor      IUART  IPWM    │  │
    │  │ ... (Abstract interfaces)              │  │
    │  └────────────────────────────────────────┘  │
    └──────────────────────────────────────────────┘
                       ▼
    ┌──────────────────────────────────────────────┐
    │         Plugin Layer                         │
    │  (Platform-Specific Implementations)         │
    │  ┌────────────────────────────────────────┐  │
    │  │ Linux Backend Plugin                   │  │
    │  │ - GPIO via libgpiod/sysfs              │  │
    │  │ - I2C via /dev/i2c-X                   │  │
    │  │ - SPI, UART, Camera (V4L2), etc.       │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ RK3588 Vendor Plugin (optional)        │  │
    │  │ - Vendor SDK integration               │  │
    │  │ - Special ISP/encoder access           │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ NT9853x Vendor Plugin (optional)       │  │
    │  │ - ISP-specific functions               │  │
    │  └────────────────────────────────────────┘  │
    │  ┌────────────────────────────────────────┐  │
    │  │ Simulation Plugin                      │  │
    │  │ - In-memory device simulation          │  │
    │  │ - Deterministic testing                │  │
    │  └────────────────────────────────────────┘  │
    └──────────────────────────────────────────────┘
                       ▼
    ┌──────────────────────────────────────────────┐
    │         Hardware Layer                       │
    │  GPIO | I2C | SPI | Camera | Sensors |       │
    │  Network | Storage | Power | Encoders        │
    └──────────────────────────────────────────────┘
```

---

## Data Flow: From Configuration to Test Execution

```
Step 1: Load Configuration
┌────────────────────────────────────┐
│ ConfigLoader reads YAML/JSON files │
│ - Load configs/common/*            │
│ - Load configs/platforms/RK3588/*  │
└────────┬───────────────────────────┘
         │
         ▼
Step 2: Merge Configurations
┌────────────────────────────────────┐
│ ConfigMerger combines all files    │
│ - Device definitions               │
│ - Function mappings                │
│ - Hardware bindings (overrides)    │
│ - Capability flags                 │
└────────┬───────────────────────────┘
         │
         ▼
Step 3: Create Plugins
┌────────────────────────────────────┐
│ PluginManager loads appropriate    │
│ plugins based on SoC/platform      │
│ - Linux backend (always)           │
│ - Optional: vendor plugins         │
│ - Optional: simulation             │
└────────┬───────────────────────────┘
         │
         ▼
Step 4: Instantiate HAL Devices
┌────────────────────────────────────┐
│ Plugins create HAL device objects  │
│ based on runtime bindings:         │
│                                    │
│ gpio_led_red →                     │
│   linux_gpio_create("gpiochip0",24)│
│                                    │
│ i2c_temp →                         │
│ linux_i2c_create("/dev/i2c-1",0x48)│
│                                    │
│ Returns: HALDevice* objects        │
└────────┬───────────────────────────┘
         │
         ▼
Step 5: Register Devices
┌────────────────────────────────────┐
│ DeviceManager stores all HAL       │
│ device objects for runtime lookup  │
│                                    │
│ Registry:                          │
│  gpio_led_red  → HALDevice* obj1   │
│  i2c_temp      → HALDevice* obj2   │
└────────┬───────────────────────────┘
         │
         ▼
Step 6: Map Functions
┌────────────────────────────────────┐
│ FunctionManager builds mappings:   │
│                                    │
│ led_status →                       │
│function_manager_get_gpio("led_status")│
│   looks up gpio_led_red in config  │
│   looks up gpio_led_red in registry│
│   returns HALDevice* object        │
│                                    │
│ Same for all functions             │
└────────┬───────────────────────────┘
         │
         ▼
Step 7: Create TestContext
┌────────────────────────────────────┐
│ Prepare test execution environment │
│                                    │
│ TestContext {                      │
│   DeviceManager* dev_mgr;          │
│   FunctionManager* func_mgr;       │
│   CapabilityManager* cap_mgr;      │
│   ... other services ...           │
│ }                                  │
└────────┬───────────────────────────┘
         │
         ▼
Step 8: Filter Tests by Capability
┌────────────────────────────────────┐
│ Load test registry                 │
│ For each test, check if required   │
│ functions are available            │
│                                    │
│ If all required functions → RUN    │
│ If any missing function → SKIP     │
│ If error during lookup → SKIP      │
└────────┬───────────────────────────┘
         │
         ▼
Step 9: Execute Tests
┌────────────────────────────────────┐
│ For each test (in order/filtered): │
│                                    │
│ 1. Call test_func(TestContext*)    │
│ 2. Test calls:                     │
│    IGPIO* led = function_manager_  │
│      get_gpio(ctx->func_mgr,       │
│                "led_status")       │
│ 3. Function manager returns        │
│    HALDevice object registered     │
│ 4. Test calls:                     │
│    hal_gpio_set(led, 1)            │
│ 5. HAL wrapper calls ops->set()    │
│ 6. Plugin operation executes       │
│    actual hardware access          │
│ 7. Test captures result            │
│ 8. Return 0 (PASS) or <0 (FAIL)    │
│                                    │
│ NEVER: Test has GPIO numbers,      │
│        device paths, SoC names     │
└────────┬───────────────────────────┘
         │
         ▼
Step 10: Generate Reports
┌────────────────────────────────────┐
│ Report generator formats results:  │
│ - Human-readable text              │
│ - JSON for parsing                 │
│ - HTML dashboard                   │
│ - JUnit XML for CI/CD              │
└────────────────────────────────────┘
```

---

## Configuration Merging Algorithm

```
Input:
  common/devices.json
  common/functions.json
  common/tests.json
  platforms/rk3588/platform.json

Process:

1. Load common/devices.json
   RuntimeConfig.devices = { gpio_led_red, i2c_temp, ... }

2. Load common/functions.json
   RuntimeConfig.functions = { led_status, temp_reading, ... }

3. Load common/tests.json
   RuntimeConfig.tests = { test_led_blink, test_temp_read, ... }

4. Load platforms/rk3588/platform.json
   platform.bindings = { gpio_led_red → chip0:24, ... }
   platform.capabilities = { led_status: true, ... }

5. Merge bindings:
   For each device in RuntimeConfig.devices:
     If platform.bindings[device.name] exists:
       device.binding = platform.bindings[device.name]

6. Merge capabilities:
   For each function in RuntimeConfig.functions:
     If platform.capabilities[function.name] exists:
       function.available = platform.capabilities[function.name]

Output:
  Fully resolved RuntimeConfig ready for execution
```

---

## Plugin Architecture

### Plugin Interface Contract

```c
// Every plugin must implement this interface
typedef struct {
    // Plugin metadata
    const char* name;              // e.g., "linux_backend"
    const char* version;           // e.g., "1.0.0"
    const char* supported_soc;     // e.g., "generic" or "rk3588"
    
    // Lifecycle
    int (*init)(RuntimeConfig* cfg);
    int (*shutdown)(void);
    
    // Device creation
    HALDevice* (*create_device)(
        const char* device_name,
        const char* device_type,
        void* binding_params
    );
} PluginInterface;
```

### Plugin Loading Process

```
1. PluginManager calls plugin discovery
2. For each plugin:
   - dlopen() dynamic library
   - dlsym() PluginInterface symbol
   - Check supported_soc matches target
   - Call init()

3. For each device in config:
   - For each loaded plugin (in priority order):
     - Ask plugin to create device
     - If successful, register with DeviceManager
     - Break

4. On shutdown:
   - Call shutdown() on each plugin
   - dlclose() plugin library
```

### Example: Linux Backend Plugin

```c
// linux_backend_plugin/linux_backend.c

#include "hal_interface.h"
#include "device_manager.h"

static int linux_plugin_init(RuntimeConfig* cfg) {
    // Initialize libgpiod, I2C subsystem, etc.
    return 0;
}

static int linux_plugin_shutdown(void) {
    // Cleanup resources
    return 0;
}

static HALDevice* linux_plugin_create_device(
    const char* device_name,
    const char* device_type,
    void* binding_params
) {
    if (strcmp(device_type, "gpio") == 0) {
        GpioBinding* binding = (GpioBinding*)binding_params;
        return linux_gpio_create(binding->chip, binding->line);
    }
    // ... handle other device types
}

// Export plugin interface
PluginInterface plugin = {
    .name = "linux_backend",
    .version = "1.0.0",
    .supported_soc = "generic",  // Works on any Linux system
    .init = linux_plugin_init,
    .shutdown = linux_plugin_shutdown,
    .create_device = linux_plugin_create_device,
};
```

---

## Test Execution Flow

```
validator --run all

↓

CLI Parser → ExecutionContext {
    .command = COMMAND_RUN,
    .target = TARGET_ALL,
    ...
}

↓

RuntimeEngine::initialize(ctx)
  - Load config
  - Merge configs
  - Load plugins
  - Create devices
  - Register devices
  - Initialize managers

↓

RuntimeEngine::execute(ctx)
  - Load test registry
  - For each test:
    - Check capabilities (skip if unavailable)
    - Check dependencies
    - Call test_func(TestContext*)
    - Capture result
    - Record metrics

↓

ReportGenerator::generate()
  - Format results
  - Generate report
  - Output to console/file

↓

Exit with status code
```

---

## Key Design Patterns

### 1. Strategy Pattern (Plugins)

Multiple implementations of device creation, selected at runtime based on platform.

### 2. Factory Pattern (DeviceManager)

Central factory for creating and retrieving device instances.

### 3. Registry Pattern (TestRegistry)

Central registry of all available tests, discovered at startup.

### 4. Facade Pattern (FunctionManager)

Simplifies test access to complex device hierarchy.

### 5. Template Method Pattern (Test Execution)

Common test setup/execution/teardown flow with customizable test code.

### 6. Observer Pattern (Future: Events)

Components publish events (device hotplug, test state changes).

---

## Isolation Boundaries

### Test Code

- **CAN**: Use function names, TestContext, HAL interfaces
- **CANNOT**: Know GPIO numbers, device paths, SoC names, plugin implementations

### Plugin Code

- **CAN**: Use vendor SDKs, direct hardware access, platform APIs
- **CANNOT**: Contain test logic, validation rules, scheduling decisions

### Configuration Code

- **CAN**: Define hardware topology, capability mappings, bindings
- **CANNOT**: Contain test code, business logic, vendor API calls

### Core Framework

- **CAN**: Orchestrate overall flow, manage resources
- **CANNOT**: Know about specific hardware, contain test logic

---

## Extensibility Points

### 1. New Device Type

1. Add HAL interface operations
2. Create device creation function in plugin
3. Update DeviceManager type enum
4. Update Device Manager documentation

### 2. New Test Type

1. Write test function
2. Register in test registry
3. Add to tests.json
4. Document required functions

### 3. New Platform Support

1. Create platform.json with bindings
2. Create plugin if special hardware access needed
3. No changes to tests or common configs

### 4. New Report Format

1. Implement ReportGenerator strategy
2. Register in CLI options
3. No changes to core logic

---

## Error Handling Strategy

```
Test Execution Errors:
  - Missing function → SKIP test
  - Capability unavailable → SKIP test
  - Device not found → SKIP test
  - Device operation fails → FAIL test
  - Test timeout → FAIL test
  - Test throws exception → FAIL test

Configuration Errors:
  - json parse error → Early exit, error message
  - Missing device definition → Early exit, error message
  - Invalid binding → Skip affected tests
  - Circular dependency → Early exit, error message

Plugin Errors:
  - Plugin not found → Log, try next plugin
  - Plugin init fails → Log, continue (capabilities reduced)
  - Device creation fails → Log, skip affected tests
```

---

## Performance Considerations

1. **Lazy Loading**: Plugins loaded only when needed
2. **Caching**: Device objects cached in registry
3. **Batching**: Tests execute in optimal order
4. **Parallel Execution** (Future): Tests with no dependencies run in parallel

---

## Security Considerations

1. **Input Validation**: All JSON configs validated before use
2. **Plugin Sandboxing**: Future: Consider plugin capability restrictions
3. **Resource Limits**: Timeout on all test operations
4. **Audit Trail**: All operations logged
5. **Access Control**: Future: Role-based access to sensitive devices

---

