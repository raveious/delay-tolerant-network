# Delay Tolerant Network Implementation using TinyOS

## Purpose

Exploration in the area of a delay tolerant network of wireless sensor nodes. Starting with an initial implimentation of the a [spray and wait][1] routing protocol on the [TelosB][3].

Afterwards, reasearch and optimation for a smaller network of Wireless Sensor Nodes.

## Development

### Checking out

After checking out this repository, you have to update the submodule(s) and build all of the tools from TinyOS. Note, this is only needed the first time and will probably never be needed again after that unless [TinyOS][2] updates it's build tools.

```
$ ./develop.sh
```

Any issues with this, refer to the [alternate instructions](https://github.com/tinyos/tinyos-main#note-new-make-system-and-tinyos-tools) and report issues to the [TinyOS][2] mainline repository.

### Building

For this project, the main board that is being used is the [TelosB][3]. However, [TinyOS][2] supports many other boards.

From the root project directory, run make from the shell.

```
$ make telosb
```

To program a TelosB board directly, add the install target.

```
$ make telosb install
```

Note: You will need to be a member of the `dialout` group in order to write code to the USB serial device. Virtual Machine users will also have to allow the virtual machine access to the USB serial device so that it can be programmed from within the virtual machine. You may also need to be a member of the VMs group (i.e. `vboxsf` for Oracle Virtual Box) if this is checked out in a shaired directory with host machine.

[1]: https://en.wikipedia.org/wiki/Routing_in_delay-tolerant_networking#Spray_and_Wait
[2]: https://github.com/tinyos/tinyos-main
[3]: http://www.memsic.com/wireless-sensor-networks/TPR2420
