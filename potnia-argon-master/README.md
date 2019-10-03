# potnia-argon
Source code for the Potnia IPRO project.

## Setting up the Development Environment
This project is built with [Particle Workbench](https://www.particle.io/workbench/).  Particle has a [tutorial](https://docs.particle.io/tutorials/developer-tools/workbench/) on how to use it.

1. Clone the repo by running `git clone https://github.com/ipro-497-506-spring-2019/potnia-argon.git`.
2. Run `git update-index --skip-worktree .vscode/settings.json` to ignore changes to the settings file.  If you don't, it'll upload the ID of your argon and other stuff that shouldn't be in Git.
3. Install VSCode.
4. Install the [Particle Workbench extension](https://marketplace.visualstudio.com/items?itemName=particle.particle-vscode-pack).
5. This repository contains a [Particle Project](https://docs.particle.io/tutorials/developer-tools/workbench/#working-with-particle-projects).  Setting up local flashing can be a lot of trouble, so it's easiest to just hit Control+Shift+P and run "Particle: Cloud Flash".

## Useful features:
 - Run `Particle: Install Library` to install a Particle-supported library, doing everything needed for cloud compilation.
 - Because the Argon is sleeping most of the time, it can be tricky to upload new code. Put it into [Safe Mode](https://docs.particle.io/tutorials/device-os/led/argon/#safe-mode) to upload easily.