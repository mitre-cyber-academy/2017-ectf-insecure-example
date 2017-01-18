# Embedded CTF Example Code

This repository contains an example reference system for MITRE's 2017 
[Embedded System CTF](http://mitrecyberacademy.org/competitions/embedded/).
This example meets all the requirements outlined in the challenge writeup
document, but is not implemented securely.  

## Disclaimer
This code is incomplete, insecure, and does not meet MITRE standards for
quality.  This code is being provided for educational purposes to serve as a
simple example that meets the minimum functional requirements for the 2017 MITRE
eCTF competition.  Use this code at your own risk!

# Getting Started
Before you can work on your bootloader, you'll first need to get up and running
with our common development environment. All eCTF development and testing will
happen in a vagrant provisioned virtual machine.

## Prerequisites
Note: if you already have one of these dependencies installed you should not
need to install it again.

1. Download and install VirtualBox here:
   [https://www.virtualbox.org/wiki/Downloads] using the provided instructions.
2. Download and install VirtualBox Extensions (for USB support) from the same
   link: [https://www.virtualbox.org/wiki/Downloads] using the provided
   instructions.
3. Download and install Vagrant here:
   [https://www.vagrantup.com/downloads.html] using the provided instructions.
4. Download and install git here:
   [https://git-scm.com/download] using the provided instructions.

## Downloading the Build Environment
Clone this repo using git by running the following command in your preferred
shell:


```
git clone https://github.com/mitre-cyber-academy/2017-ectf-insecure-example.git
```

This will download the latest version of the build environment and example
source code into the current directory.

## Getting the VM Up and Running
Once you have downloaded the build environment, change into its direcotry in 
your shell and follow these steps to start up your VM:

1. Copy `Vagrantfile.local.example` to `Vagrantfile.local`.
2. Open `Vagrantfile.local` and ensure that the configurations make sense for
   your system configuration.
3. Run `vagrant up` in the shell/command line of your choice to download the VM
   image, provision your VM, and start it up.
4. Run `vagrant ssh` to log in to the VM.

If any errors occur during steps 3 or 4, try to resolve them by modifying
`Vagrantfile.local` rather than `Vagrantfile`.

The AVR Dragon and USB to RS232 converters should be automatically handed over
to the VM when it is running, but if they are not you should be able to attach
them through the virtualbox GUI. If you run into problems with USB that you
cannot resolve on your own, please ask for help in the eCTF Slack channel.

## Connecting the Boards
To connect the AVR Dragon to the Protostack board, use the included ribbon cable
to connect the 6-pin ISP header on the AVR Dragon to the ISP10 header on the
protostack board.  The notch on the connecting cable should face towards pin-1
on the Dragon. Do not use the 10-pin connector on the Dragon -- this is for JTAG
and is not needed to get up and running.

# Provided Files
1. `Vagrantfile`
The base configurations for vagrant. You are not allowed to modify this file.

2. `Vagrantfile.local` (and `Vagrantfile.local.example`)
Local configurations for the VM on _your computer only_. An example is provided
with basic configurations for reference. You should not submit this file, as we
have our own configurations.

3. `Vagrantfile.team`
Configurations for your team's submission. This is where you add tools,
dependencies, and configurations that are required for your submission. Please
add only required dependencies and configurations (editors, IDEs, etc go in
Vagrantfile.local).

4. `bootloader/`
In this directory is the example bootloader we have provided to you for
reference. When you submit your code, your bootloader should be in this
directory. See `bootloader/README.md` for bootloader-specific instructions and
help.

5. `host_tools/`
In this directory are the example host tools we have provided to you for
reference. When you submit your code, your host tools should be in this
directory. See `host_tools/README.md` for host-tool-specific instructions and
help.

# Host Tools
The host tools are intended to be run from your VM. They communicate with the
bootloader over UART1 on the AVR.

## How to Run the Tools
All of the example host tools are written in python, but do not have .py file
extensions. In Linux (e.g., on your vagrant VM) you should be able to run them
as a regular program/script (i.e., `./bl_build`). If that isn't working you may
need to run them an argument to the python interpreter (i.e.,
`python bl_build`). The result will be the same either way.

All tools that take arguments should have a help flag (`-h`) that will provide
descriptions.

## Programming the Board
The following command should program your board with the output from your
`bl_build` tool:

```
avrdude -P usb -p m1284p -c dragon_isp -B 200 -u -U flash:w:flash.hex:i \
                                                 -U eeprom:w:eeprom.hex:i \
                                                 -U lfuse:w:lfuse.hex:i \
                                                 -U hfuse:w:hfuse.hex:i \
                                                 -U efuse:w:efuse.hex:i
```

Refer to the
[avrdude documentation](http://www.nongnu.org/avrdude/user-manual/avrdude.html)
for additional help with avrdude.

The AVR dragon may occasionally end up in a state where it no longer responds to
avrdude. If this happens, the problem can be resolved by disconnecting and
reconnecting the dragon's USB cable.

## Tool Specifications
Detailed tool specifications should be found in the challenge rules document.

Note that while you may completely rewrite the tools, they should still live in
this directory.

Your tools must be written to the exact specifications in the rules with no
additional required arguments or other user inputs. This allows us to write
automated tests to check if your submission is complete. If you have questions
about the specifications please contact the organizers in Slack or by email:
ectf@mitre.org.
