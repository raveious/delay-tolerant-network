# Delay Tolerant Network Implementation for Telos using TinyOS

## Purpose

Exploration in the area of a delay tolerant network of wireless sensor nodes. Starting with an initial implimentation of the a [spray and wait][1] routing protocol.

Afterwards, reasearch and optimation for a smaller network of Wireless Sensor Nodes.

## Development

### Checking out

After checking out this repository, you have to update the submodule(s) and build all of the tools from TinyOS. Note, this is only needed the first time and will probably never be needed again after that unless [TinyOS][2] updates it's build tools.

```
$ ./develop.sh
```

Any issues with this, refer to the [alternet instructions](https://github.com/tinyos/tinyos-main#note-new-make-system-and-tinyos-tools) and report issues to the [TinyOS][2] mainline repository.

### Building

For this project, the main board that is being used is the TelosB. However, [TinyOS][2] supports many other boards.

From the root project directory, run make from the shell.

```
$ make telosb
```

To program a TelosB board directly, add the install target.

```
$ make telosb install
```

[1]: https://en.wikipedia.org/wiki/Routing_in_delay-tolerant_networking#Spray_and_Wait
[2]: https://github.com/tinyos/tinyos-main
