# Binary Spray and Wait

This is an implementation of the [Binary Spray and Wait][1] protocol for a [Delay Tolerant Network][2].

Traditionally with the [Spray and Wait][1] routing protocol, a source node will only send out packets to nodes which do not already have it. In this implementation, when a new node in encountered, the source node will pass "half" of the copies to the new node. Both nodes will then continue to pass out "half" of their stacks until all copies have been distriduted out.

## Purpose

The goals of this implementation are:
 * See battery life gets affected
   * On the network as a whole
   * On node that stay nearby the source for longer durations of time
   * On the source node while surrounding nodes are moving around
 * Distribution of the information in a more efficient way

## Development

For this project, the main board that is being used is the [TelosB][4]. However, [TinyOS][3] supports many other boards.

From the root project directory, run make from the shell.

```
$ make telosb
```

To program a TelosB board directly, add the install target.

```
$ make telosb install
```

Note: You will need to be a member of the `dialout` group in order to write code to the USB serial device. Virtual Machine users will also have to allow the virtual machine access to the USB serial device so that it can be programmed from within the virtual machine. You may also need to be a member of the VMs group (i.e. `vboxsf` for Oracle Virtual Box) if this is checked out in a shaired directory with host machine.

[1]: https://en.wikipedia.org/wiki/Routing_in_delay-tolerant_networking#Spray_and_Wait_versions
[2]: https://en.wikipedia.org/wiki/Delay-tolerant_networking
[3]: https://github.com/tinyos/tinyos-main
[4]: http://www.memsic.com/wireless-sensor-networks/TPR2420
