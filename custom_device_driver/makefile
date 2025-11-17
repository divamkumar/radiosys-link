# --------------------------------------------------------------------------
# Makefile for Divam's Out-of-Tree Kernel Module Compilation (Cross-Architecture)
# This Makefile targets the Buildroot-generated toolchain.
# --------------------------------------------------------------------------

# --- Configuration Variables ---

# Path to the Buildroot-compiled Linux kernel source (KDIR)
KDIR := $(BUILDROOT_LINUX_DIR)

# PWD (Present Working Directory)
PWD := $(shell pwd)

# Module Definition
obj-m := divam_cdev.o

# --------------------------------------------------------------------------
# --- Build Targets ---
# --------------------------------------------------------------------------

# Default target: Compiles the module
all:
	# Calls the main Linux kernel Makefile (-C $(KDIR)) 
	# and specifies this directory (-M $(PWD)) to build modules.
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target: Removes intermediate build files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.ko *.o *.mod* Module.symvers modules.order

# Ensure 'all' and 'clean' are always treated as targets, not files
.PHONY: all clean
