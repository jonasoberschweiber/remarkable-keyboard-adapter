#!/bin/sh

# Flash an ELF file via a CMSIS DAP adapter (for example, the Raspberry Pi Debug
# probe).

openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000; program $1 verify reset exit"
