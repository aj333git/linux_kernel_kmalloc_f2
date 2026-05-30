# Linux Kernel Memory Allocation Control Plane Demo

A learning-oriented project that explores Linux kernel memory management using a simple Control Plane → Data Plane architecture.

The project demonstrates:

* Linux Kernel Modules (LKM)
* Dynamic memory allocation with `kmalloc()`
* Memory cleanup using `kfree()`
* Module parameters
* Control Plane / Data Plane separation
* Pointer tracking
* Dangling pointer concepts
* F#-driven configuration workflow

---

## Architecture

```text
Control Plane (F#)
        │
        ▼
 alloc_plan.txt
        │
        ▼
 Linux Kernel Module
 (Data Plane)
        │
        ▼
 kmalloc() / kfree()
        │
        ▼
 Kernel Heap
```

### Control Plane

The control plane defines allocation intent.

Example:

```text
buffer,1024
big_buffer,4096
```

Responsibilities:

* Define allocation plans
* Generate configuration data
* Control system behavior

---

### Data Plane

The kernel module executes the plan.

Responsibilities:

* Allocate memory
* Track allocations
* Log allocation events
* Release resources during unload

---

## Memory Lifecycle

### Allocation Phase

User provides:

```text
names="buffer,big_buffer"
values=1024,4096
```

Kernel module performs:

```c
kmalloc(1024, GFP_KERNEL);
kmalloc(4096, GFP_KERNEL);
```

Allocated pointers are stored in:

```c
void *buffers[];
size_t sizes[];
```

---

### Running State

The module maintains:

```text
buffers[0] -> memory block
buffers[1] -> memory block

sizes[0] = 1024
sizes[1] = 4096
```

The tracking tables allow the module to manage all allocations safely.

---

### Cleanup Phase

During module unload:

```c
kfree(buffers[i]);
```

All allocated memory is returned to the kernel heap.

---

## Dangling Pointer Concept

After:

```c
kfree(ptr);
```

The pointer variable still contains an address.

Example:

```text
ptr -> 0xA100
```

However the memory behind that address no longer belongs to the module.

Such a pointer is called a **dangling pointer**.

Dereferencing a dangling pointer may cause:

* Undefined behavior
* Memory corruption
* Kernel crashes
* Use-after-free bugs

A common defensive practice is:

```c
kfree(ptr);
ptr = NULL;
```

---

## Why `void *buffers[]`?

```c
void *buffers[MAX_BUFFERS];
```

Each entry can point to any type of memory:

* Raw bytes
* Strings
* Structures
* Network buffers
* Device data

This makes the allocation tracker generic.

---

## Build Workflow

### Generate Control Plan

```bash
dotnet fsi control_plane.fsx
cp alloc_plan.txt /tmp/
```

### Build Module

```bash
make clean
make
```

### Sign Module

```bash
sudo /usr/src/linux-headers-$(uname -r)/scripts/sign-file \
sha256 \
~/kernel_keys/MOK.key \
~/kernel_keys/MOK.crt \
kmalloc_data_plane.ko
```

### Load Module

```bash
sudo insmod kmalloc_data_plane.ko \
names="buffer,big_buffer" \
values=1024,4096
```

### View Logs

```bash
dmesg | tail
```

### Unload Module

```bash
sudo rmmod kmalloc_data_plane
```

---

## Learning Goals

This project is intended to teach:

* Linux kernel module development
* Kernel heap allocation
* Resource ownership
* Memory lifecycle management
* Dangling pointers
* Control Plane / Data Plane architecture
* Systems programming fundamentals

---

## Future Enhancements

* Structured allocation plans
* Allocation verification passes
* Use-after-free detection
* Memory leak detection
* DSL-driven allocation generation
* Intermediate Representation (IR) layer
* Code generation backend

