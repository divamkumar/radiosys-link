# RadioSys-Link: Embedded System-of-Systems (Radio & Mission Computer)

### A Hardware-in-the-Loop (HIL) Avionics Prototype

**RadioSys-Link** is a distributed embedded system simulating a critical avionics communications loop. It consists of a **Mission Computer** (Custom Linux OS on Raspberry Pi) interfacing with a **Software Defined Radio / Sensor Node** (ESP32) via a custom binary protocol.

This project demonstrates the full vertical slice of Embedded Linux Engineering: form **schematic design** and **signal integrity** to **Board Support Package (BSP)** creation, **Kernel Module** development, and **User Space** system programming.

-----

## 🏗 System Architecture

The system is designed as a distributed "System-of-Systems" (SoS) with two distinct compute nodes:

1.  **The Mission Computer (Node A):**
      * **Hardware:** Raspberry Pi 4 (ARMv8).
      * **OS:** Custom Minimal Linux built via **Buildroot** (External Tree).
      * **Role:** Hosts the custom Linux Kernel Module and C++ Telemetry Daemon.
2.  **The Radio/Sensor (Node B):**
      * **Hardware:** ESP32 Microcontroller (Xtensa).
      * **Firmware:** Bare-metal C++ (HAL/IDF).
      * **Role:** Simulates sensor data acquisition and handles high-priority interrupts.

### 🔌 Interface & Protocol

  * **Data Bus:** UART (115200 Baud, 8N1) for telemetry packets.
  * **Control Plane:** Dedicated GPIO Interrupt line for low-latency "Critical Alert" signaling.
  * **Debug Interface:** JTAG headers provisioned for OCD (On-Chip Debugging).

-----

## ⚡ Hardware Engineering & Signal Integrity

To ensure reliable operation in a noisy simulated environment, this project incorporates analog filtering techniques.

### The "De-bouncing" Circuit

Raw mechanical switching introduces high-frequency noise (switch bounce). A passive **RC Low-Pass Filter** was designed to condition the input signal before it reaches the microcontroller.

  * **Equation:** $\tau = R \times C$
  * **Values:** $R = 10k\Omega$, $C = 1\mu F$
  * **Result:** $\tau = 10ms$. Any transient voltage spikes or mechanical bounces shorter than 10ms are filtered out, ensuring a clean logic transition.

*(Include schematic screenshot here in your repo)*

-----

## 🐧 Embedded Linux Implementation

This is not a standard Raspberry Pi OS install. The operating system was cross-compiled from scratch.

### 1\. Buildroot (BSP Development)

Used `BR2_EXTERNAL` to create a professional, reproducible Board Support Package.

  * **Toolchain:** glibc, C++ support, custom kernel headers.
  * **RootFS:** Stripped down to minimal BusyBox; system boots in \<4 seconds.
  * **Init System:** Custom SysVinit scripts (`S99avionics`) to launch the telemetry daemon on boot.

### 2\. Device Tree Overlay (`.dts`)

Hardware abstraction is handled via a custom Device Tree Overlay, decoupling the driver from the physical pinout.

```dts
// overlay/radiosys-alert.dts
fragment@1 {
    target-path = "/";
    __overlay__ {
        radiosys_alert_device {
            compatible = "radiosys,mission-alert";
            gpios = <&gpio 23 1>; // GPIO 23, Rising Edge
        };
    };
};
```

### 3\. Linux Kernel Module (Platform Driver)

A custom **Platform Driver** (`radiosys_interrupt.ko`) was written to handle the "Critical Alert" signal in Kernel Space.

  * **Mechanism:** The driver probes the Device Tree for the `radiosys,mission-alert` compatible string.
  * **ISR:** Registers a hardware interrupt handler (`request_irq`) to preempt user-space tasks when the alert line goes high.

-----

## 📡 Firmware & Communications

### Binary Protocol

A custom packet structure was implemented to serialize data between the Xtensa (ESP32) and ARM (Pi) architectures, accounting for potential endianness mismatches.

`[ 0xAA (Header) ] [ CMD_ID ] [ PAYLOAD_LEN ] [ PAYLOAD... ] [ CRC8 ]`

### Verification & Debugging

  * **Wireshark / Logic Analysis:** The UART bus was inspected to verify timing integrity and baud rate synchronization.
  * **JTAG/GDB:** The Mission Computer application was compiled with debug symbols (`-g`), allowing for remote `gdbserver` attachment to step through the C++ daemon during runtime.

-----

## 📂 Repository Structure

```text
.
├── hardware/
│   └── schematic_kicad.pdf      # Circuit diagrams and RC Filter design
├── firmware-radio/              # ESP32 Source Code
│   └── src/main.cpp             # Sensor logic and UART formatting
├── os-mission-computer/         # Buildroot BR2_EXTERNAL Tree
│   ├── Config.in
│   ├── external.mk
│   ├── board/
│   │   └── overlays/            # Device Tree Source (.dts)
│   ├── package/
│   │   ├── radiosys-driver/      # C++ User Space Daemon
│   │   └── radiosys-module/      # Linux Kernel Module (LKM)
│   └── configs/
│       └── radiosys_defconfig    # Build configuration
└── docs/
    └── trace_logs/              # Wireshark captures and dmesg logs
```

-----

## 🛠 Skills Matrix

| Domain | Technologies / Concepts Demonstrated |
| :--- | :--- |
| **Embedded Linux** | Buildroot, Kernel Configuration (`menuconfig`), Cross-Compilation Toolchains |
| **Kernel Development** | Loadable Kernel Modules (LKM), Platform Drivers, Interrupt Handling (ISR), `dmesg` |
| **Hardware Description** | Device Trees (DTS), Overlays, GPIO Subsystems, Pin Muxing |
| **Electrical Eng** | Schematic Reading, RC Filters, Signal Conditioning, UART/Serial Interfaces |
| **System Programming** | C/C++, File Descriptors, POSIX APIs (`termios`), Multi-threading |
| **Debugging** | Remote GDB, Logic Analysis, Protocol Decoding, JTAG (Provisioning) |

-----

## 🚀 How to Build

1.  **Clone the Repo:**
    ```bash
    git clone https://github.com/divamkumar/radiosys-link.git
    ```
2.  **Build the OS:**
    ```bash
    cd buildroot
    make BR2_EXTERNAL=../os-mission-computer radiosys_defconfig
    make
    ```
3.  **Flash & Run:**
      * Flash `output/images/sdcard.img` to SD Card.
      * Connect Pi GPIO 14/15 to ESP32 UART.
      * Connect Pi GPIO 23 to ESP32 Alert Pin.
      * Boot and observe Kernel Logs: `dmesg -w`.

-----

## 🔮 Future Roadmap (Security Hardening)
* **Secure Boot:** Implement U-Boot verified boot to ensure only signed Kernel images are loaded.
* **Bus Encryption:** Upgrade the UART protocol to use AES-128 encryption to prevent bus snooping between the Radio and Mission Computer.
