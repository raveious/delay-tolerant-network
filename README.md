# Delay Tolerant Network Implementation using TinyOS

## Purpose

Exploration in the area of a delay tolerant network of wireless sensor nodes.

## Development

We learned that the build process is sometimes difficult for developers who are not very familiar with Linux or the Linux shell. Here is a helpful little guide on how to setup out project.

### Checking out

After checking out this repository, you have to update the submodule(s) and build all of the tools from TinyOS. Note, this is only needed the first time and will probably never be needed again after that unless [TinyOS][1] updates it's build tools.

```
$ ./develop.sh
```

Any issues with this, refer to the [alternate instructions](https://github.com/tinyos/tinyos-main#note-new-make-system-and-tinyos-tools) and report issues to the [TinyOS][1] mainline repository.

Note: You will need to be a member of the `dialout` group in order to write code to the USB serial device. Virtual Machine users will also have to allow the virtual machine access to the USB serial device so that it can be programmed from within the virtual machine. You may also need to be a member of the VMs group (i.e. `vboxsf` for Oracle Virtual Box) if this is checked out in a shaired directory with host machine.

## Areas of Research

These are some of out different approaches that we are conducting research within delay tolerant networks.

 * [Spray and Wait](./Spray_and_Wait)
 * [Spray and Focus](./Spray_and_Focus)
 * [Utility (RAPID)](./Util_Routing)

 [1]: https://github.com/tinyos/tinyos-main
