# 🐚 tinydocker

**tinydocker** is an educational project written in **C11**, designed to reproduce the core features of a Linux container from scratch, without using Docker.

This project implements:

- Process creation using `clone()` and isolation via **Linux namespaces**:
- Mounting `/proc` inside the container
- Changing the root filesystem using `chroot()` or `pivot_root()`
- Executing an isolated shell (`/bin/sh`)
- Creating and configuring a minimal **cgroup** (CPU or memory limitation)

## 📦 Build

```bash
make debug
```

## 🚀 Run

```bash
sudo ./build/tinydocker rootfs/ /bin/sh
```

⚠️ Requires a Linux system with support for namespaces and cgroups (v1 or v2) and a minimal root filesystem inside the `rootfs/` directory (e.g., based on BusyBox).

## 📁 Project structure

``` plaintext
tinydocker/
├── src/ # Source code (main, namespaces, cgroups)
├── include/ # Header files
├── rootfs/ # Minimal root filesystem
├── build/ # Compiled binaries and objects
├── Makefile # Build rules
├── .gitignore # Ignored files
└── README.md # This file
```

## 🎯 Project goals

- Learn how Linux containers work under the hood
- Experiment with namespaces, cgroups, and process isolation
- Build a simple, self-contained container runtime in C

> This project is for educational purposes only. It is **not** production-ready or secure by design.

---

## ⚠️ Work in progress

This project is a **work in progress** and its internal structure will evolve significantly over time.

Expect:

- Breaking changes in folder layout
- Refactors as features are added
- Shifts in responsibilities between files (e.g., CLI vs container logic)

Feel free to explore, but don’t rely on stability just yet.

## 🗺️ Roadmap

Here is the planned progression for tinydocker:

✅ = done / 🛠️ = in progress / 🔜 = planned

### Core container logic

- 🔜 Create a containerized process and namespace isolation
- 🔜 Change hostname from inside the container
- 🔜 Mount `/proc` inside the container
- 🔜 Implement chroot to a minimal rootfs
- 🔜 Execute `/bin/sh` using a new process

### Resource control

- 🔜 Create and apply a cgroup to limit memory/CPU
- 🔜 Improve cgroup abstraction (modular code)

### Networking

- 🔜 Create a `veth` pair
- 🔜 Connect the container to a Linux bridge
- 🔜 Assign a static IP
- 🔜 Set up NAT with `iptables`

### Filesystem & volumes

- 🔜 Support `-v /host:/container` to mount directories

### Image loading

- 🔜 Load a `.tar` image (like busybox.tar) and extract it to rootfs

### CLI & usability

- 🔜 Add a CLI with argument parsing (container name, resources, image...)
- 🔜 Add `--help` and error messages

### Multi-container & images

- 🔜 Manage multiple containers
- 🔜 Local image storage (basic Docker-like registry)

---

This roadmap may evolve based on experiments and design decisions.