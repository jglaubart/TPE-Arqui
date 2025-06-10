#!/bin/bash
sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev dsound,id=speaker -machine pcspk-audiodev=speaker