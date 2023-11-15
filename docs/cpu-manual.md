## Registers

All registers are 16bit size. IP and SR are special registers (not directly accessible).

| Name   | Description                  | Default value
|---     |---                           |---
| IP     | Instruction Pointer          | 0x0000
| CR     | Control Register             | 0x9000
| SP     | Stack Pointer                | 0x8000
| W0     | Work register 0              | 0x0000
| W1     | Work register 1              | 0x0000
| W2     | Work register 2              | 0x0000
| W3     | Work register 3              | 0x0000

### Control register bits

| 15   | 14  | 13    | 12    | 11    | 10    |  9   |  8   |  7   |  6   |  5  |  4  |  3   |  2 |  1  |  0
|---   |---  |---    |---    |---    |---    |---   |---   |---   |---   |---  |---  |---   |--- |---  |---  
| BK1  | BK0 | SB0   | SB1   |       |       | TIE1  | TIE0  | IIO3 | IIO2 | IIO1 | IIO0 | GIE  | OV    |     |

- BK[1-0]: Memory bank selection (00, 01, 10 or 11). Default: `10`
- SB[1-0]: Stack memory bank selection. Default: `01`
- TIE[1-0]: If set, Time Interrupt is enabled. Default: `00`
- IIO[3-0]: If set, I/O Interrupt is enabled. Default: `00`
- GIE: If unset, all Interrupts are disabled. Default: `0`
- OV: Overflow bit. Default: `0`
- Remaining bits unused

## Memory layout

Memory bank `00`: Program code

Memory bank `01`

| Address           | Purpose
|---                |---
| 0x0000 - 0x7cff   | Video memory
| 0x7d00 - 0x7dff   | Interrupt table
| 0x7e00 - 0x7eff   | Input information (e.g. Keyboard data)
| 0x7f00 - 0x7fff   | Reserved
| 0x8000 - 0xffff   | Default stack region

Memory banks `10` and `11` are General Purpose memory

## Instruction Set

See the [Instruction set manual](isa.md).

## Interrupts

Interrupt table:

| Interrupt  | Address
|---         |---
| Timer 0    | 0x7d00
| Timer 1    | 0x7d04

### Time

There are 2 timer interrupts in micro16. They are all initially disabled.
To enable an interrupt, it's flag must be active in the `CR` register, and the `GIE` bit must also be enabled.

- Timer 0: Triggered once every 50ms
- Timer 1: Triggered once every 500ms

Every time it's triggered, the timer interrupt will:

1. Disable all interrupts
2. Save the current IP on the stack
3. `JMP` to the code address given by the Interrupt Table

In order to return from an interrupt, `IRET` may be used, as it will return from the interrupt and re-enable interrupts.
If reentrancy is wanted, `EAI` must be called explicitly from inside the interrupt routine.

## Peripherals

- #### Keyboard

TODO

- #### Video

Micro16 comes with a 320x200 16color (4bit) screen.
The color map is given:

| Color                                                                                                              | Color
|--------------------------------------------------------------------------------------------------------------------| ---
| <span style="background-color: #191919; color: white">&nbsp;0000&nbsp;</span> <img src="../img/colormap/0000.png"> | <span style="background-color: #ccdb25; color: black">&nbsp;1000&nbsp;</span> <img src="../img/colormap/1000.png">
| <span style="background-color: #cbcbcb; color: black">&nbsp;0001&nbsp;</span> <img src="../img/colormap/0001.png"> | <span style="background-color: #ccdb88; color: black">&nbsp;1001&nbsp;</span> <img src="../img/colormap/1001.png">
| <span style="background-color: #ac3232; color: black">&nbsp;0010&nbsp;</span> <img src="../img/colormap/0010.png"> | <span style="background-color: #d2842a; color: black">&nbsp;1010&nbsp;</span> <img src="../img/colormap/1010.png">
| <span style="background-color: #ac716b; color: black">&nbsp;0011&nbsp;</span> <img src="../img/colormap/0011.png"> | <span style="background-color: #d2ac7a; color: black">&nbsp;1011&nbsp;</span> <img src="../img/colormap/1011.png">
| <span style="background-color: #4fac43; color: black">&nbsp;0100&nbsp;</span> <img src="../img/colormap/0100.png"> | <span style="background-color: #824aad; color: black">&nbsp;1100&nbsp;</span> <img src="../img/colormap/1100.png">
| <span style="background-color: #92b687; color: black">&nbsp;0101&nbsp;</span> <img src="../img/colormap/0101.png"> | <span style="background-color: #b795c2; color: black">&nbsp;1101&nbsp;</span> <img src="../img/colormap/1101.png">
| <span style="background-color: #5b69ac; color: black">&nbsp;0110&nbsp;</span> <img src="../img/colormap/0110.png"> | <span style="background-color: #1f7d6e; color: black">&nbsp;1110&nbsp;</span> <img src="../img/colormap/1110.png">
| <span style="background-color: #acadc8; color: black">&nbsp;0111&nbsp;</span> <img src="../img/colormap/0111.png"> | <span style="background-color: #87ccc8; color: black">&nbsp;1111&nbsp;</span> <img src="../img/colormap/1111.png">

Each pixel is mapped in memory bank 1 from 0x0000-0x7cff. But note that each byte contain 2 pixels, as each pixel is 4 bits long.

- #### Disk

TODO
