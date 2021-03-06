# Delay Tolerant Network Implementation using TinyOS

## Purpose

Exploration in the area of a [Delay Tolerant Network][2] of wireless sensor nodes.

## Areas of Implementation Research

These are some of out different approaches that we are conducting research within [Delay Tolerant Networks][2].

 * Spray and Wait
   * [Vanilla Spray and Wait](./Spray_and_Wait)
   * [Binary Spray and Wait](./Binary_Spray_and_Wait)
   * [Spray and Pray](./Spray_and_Pray)
 * [Utility (RAPID)](./Util_Routing)

## Areas of Simulation Research

* Spray and Wait
  * [Carrier Sense Multiple Access](./simulations)

## Development

We learned that the build process is sometimes difficult for developers who are not very familiar with Linux or the Linux shell. Here is a helpful little guide on how to setup out project.

### Checking out

After checking out this repository, you have to update the submodule(s). This will get TinyOS into your checkout.

```
$ git submodule init
$ git submodule update
```

You can build the tools out of the [TinyOS][1] checkout or install all of the tools from the package manager.

```
$ apt-get install automake tinyos-tools
```

There exists a script which runs all of this which only needs to be ran the first time this is checked out.

```
$ ./develop.sh
```

Any issues with this, refer to the [alternate instructions](https://github.com/tinyos/tinyos-main#note-new-make-system-and-tinyos-tools) and report issues to the [TinyOS][1] mainline repository.

Note: You will need to be a member of the `dialout` group in order to write code to the USB serial device.

 [1]: https://github.com/tinyos/tinyos-main
 [2]: https://en.wikipedia.org/wiki/Delay-tolerant_networking
