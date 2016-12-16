# The ONE

The Opportunistic Network Environment simulator(ONE simulator).

The ONE is a Opportunistic Network Environment simulator which provides a
powerful tool for generating mobility traces, running DTN messaging
simulations with different routing protocols, and visualizing both
simulations interactively in real-time and results after their completion.
The [wiki page](https://github.com/akeranen/the-one/wiki) has the latest information.

Download from [the ONE homepage at GitHub](http://akeranen.github.io/the-one/).

Compiling
---------
Requirements : Java 6 JDK or later.
In windows, compile.bat
In ubuntu, ./compile.sh

Running
-------

Synopsis:
./one.sh [-b runcount] [conf-files]

Options:
  -b Run simulation in batch mode. Doesn't start GUI but prints
information about the progress to terminal. The option must be followed
by the number of runs to perform in the batch mode or by a range of runs
to perform, delimited with a colon (e.g, value 2:4 would perform runs 2,
3 and 4).

Parameters:
  conf-files: The configuration file names where simulation parameters
are read from. Any number of configuration files can be defined and they are
read in the order given in the command line. Values in the later config files
override values in earlier config files.

Example configuration files are available in sample_settings folder

Reports
-------
check in reports folder



