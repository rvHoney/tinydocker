# 🐟 TinyDocker

A minimal container runtime implementation for educational purposes. This project demonstrates the core concepts of containerization by implementing a simple container runtime using Linux namespaces and cgroups.

## Project Goals

- Educational: Understand how container runtimes work under the hood
- Minimal: Focus on core containerization features
- Clear: Well-documented and easy to understand code
- Practical: Actually run containers with resource limits

## Technical Stack

- **OS**: Tested on Ubuntu 22.04 LTS
- **Cgroups**: v2 (using unified hierarchy)
- **Namespaces**: UTS, PID, Mount
- **Language**: C99
- **Build System**: Make

## Quick Start

### Prerequisites

- Ubuntu 22.04 LTS
- GCC compiler
- Make
- Root privileges (for running containers)

### Building

```bash
# Clone the repository
git clone https://github.com/yourusername/tinydocker.git
cd tinydocker

# Build the project
make
```

The binary will be available at `build/bin/tinydocker`.

### Running

```bash
# Run a container with default settings
sudo ./build/bin/tinydocker -- /bin/bash

# Run with custom settings
sudo ./build/bin/tinydocker -h mycontainer -c 2 -m 512 -- /bin/bash
```

## Command Line Options

```
Usage: tinydocker [OPTIONS] -- COMMAND [ARGS...]

Options:
  -h, --hostname NAME   Set container hostname (default: container)
  -r, --rootfs PATH     Set root filesystem path (default: ./rootfs)
  -c, --cpus N          Set maximum number of CPUs (default: 1)
  -m, --memory SIZE     Set maximum memory in MB (default: 512)
  --help                Display this help message

Examples:
  # Run a basic container
  sudo tinydocker -- /bin/bash

  # Run with custom hostname and resource limits
  sudo tinydocker -h myapp -c 2 -m 1024 -- /bin/bash
```

## How It Works

TinyDocker uses Linux namespaces and cgroups to create isolated containers:

- **Namespaces**: Provides isolation for:
  - UTS: Hostname and domain name
  - PID: Process tree
  - Mount: Filesystem mounts

- **Cgroups**: Manages resource limits:
  - CPU: Number of available CPUs
  - Memory: Maximum memory usage

## Roadmap

Here is the planned progression for tinydocker:

✅ = done / 🛠️ = in progress / 🔜 = planned

### Core container logic

- ✅ Create a containerized process and namespace isolation
- ✅ Change hostname from inside the container
- ✅ Mount /proc inside the container
- ✅ Implement chroot to a minimal rootfs
- ✅ Execute /bin/sh using a new process

### Resource control

- ✅ Create and apply a cgroup to limit memory/CPU
- ✅ Improve cgroup abstraction (modular code)

### Networking

- 🛠️ Create a veth pair
- 🛠️ Connect the container to a Linux bridge
- 🛠️ Assign a static IP
- 🛠️ Set up NAT with iptables

### Filesystem & volumes

- 🔜 Support -v /host:/container to mount directories

### Image loading

- 🔜 Load a .tar image (like busybox.tar) and extract it to rootfs

### CLI & usability

- 🛠️ Add a CLI with argument parsing (container name, resources, image...)
- 🛠️ Add --help and error messages

### Multi-container & images

- 🔜 Manage multiple containers
- 🔜 Local image storage (basic Docker-like registry)

> This roadmap may evolve based on experiments and design decisions.

## Learning Resources

- [Linux Namespaces](https://man7.org/linux/man-pages/man7/namespaces.7.html)
- [Control Groups v2](https://www.kernel.org/doc/html/latest/admin-guide/cgroup-v2.html)
- [clone(2) System Call](https://man7.org/linux/man-pages/man2/clone.2.html)

## Limitations

- Educational purpose only
- Limited namespace support
- Basic resource management
- No networking support
- No image management
- Requires root privileges

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.