# reMarkable Keyboard Adapter

An adapter to connect a USB keyboard to the reMarkable 2 and have the reMarkable behave is if there were a Type Folio connected. Based on the Raspberry Pi Pico.
Inspired by the prior work done by Dudlushka in https://github.com/Dudlushka/Remarkable_TypeFolio_Pretender.

> :warning: **This is still a work in progress**
> 
> It needs a power source right now and expects a command via UART to start the handshake with the reMarkable. Also, there is still at least one
> crashing bug somewhere in the code.

## Circuitry

The circuitry required is the same as in Dudlushka's project: https://github.com/Dudlushka/Remarkable_TypeFolio_Pretender
