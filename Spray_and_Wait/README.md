# Vanilla Spray and Wait

This is an implementation of a [Spray and Wait][1] protocol for a [Delay Tolerant Network][2].

In this implementation, the source node will periodically boardcast a packet out to all of its neighbors. Those who receive the packet will send an acknowledgement to broadcasting node to inform it that the packet was received and how many nodes are nearby. The source will give a final acknowledgement to those nodes which have been chosen to be "sprayed" and decrement the number of known nodes with the desired packet.

## Purpose

The goals of this implementation are:
 * See battery life gets affected
   * On the network as a whole
   * On node that stay nearby the source for longer durations of time
   * On the source node while surrounding nodes are moving around

## Development

For this project, the main board that is being used is the [TelosB][4]. However, [TinyOS][3] supports many other boards.

From the root project directory, run make from the shell.

```
$ make telosb
```

To program a TelosB board directly, add the install target.

```
$ make telosb install,n
```

where `n` is the optional node ID.

Note: You will need to be a member of the `dialout` group in order to write code to the USB serial device. Virtual Machine users will also have to allow the virtual machine access to the USB serial device so that it can be programmed from within the virtual machine. You may also need to be a member of the VMs group (i.e. `vboxsf` for Oracle Virtual Box) if this is checked out in a shaired directory with host machine.

## Testing Procedure

All node in the network were each given an ID via the install command above. In a relatively large room, several of these node were plugin into USB charging adaptors for cell phone and others had batteries. The source node is at one end of the room while the destination node is on the other end of the room.

In a Spray and Wait network, the first node to encouter the original node were "Sprayed" with the packet and traveled to the destination node. When coming within range of the destination node, the packet was passed to it and no other nodes along the way.

The destination node then repeats the process by sending an acknowledgement back to the source using the same technique.

[1]: https://en.wikipedia.org/wiki/Routing_in_delay-tolerant_networking#Spray_and_Wait
[2]: https://en.wikipedia.org/wiki/Delay-tolerant_networking
[3]: https://github.com/tinyos/tinyos-main
[4]: http://www.memsic.com/wireless-sensor-networks/TPR2420
