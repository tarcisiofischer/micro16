# Micro16

[![CMake](https://github.com/tarcisiofischer/micro16/actions/workflows/cmake.yml/badge.svg)](https://github.com/tarcisiofischer/micro16/actions/workflows/cmake.yml)

Yet another 16bit computer emulator for a computer that doesn't exist.


## Description

The computer has an 16-bit fictional CPU.
Memory has basically two levels: CPU registers and 4 banks of 64KB of RAM each (Total 256KB).
Since some parts of the memory are dedicated, general purpose memory is actually less than that.

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

### Arithmetic & Logic instructions

- #### NOP `0000 0000 0000 0000`

No operation

- #### ADD `0000 0001 00cc aabb`

`W[cc] = W[aa] + W[bb]`\
If overflow occurs, register `CR` bit `OV` is set.

- #### SUB `0000 0010 00cc aabb`

`W[cc] = W[aa] - W[bb]`

- #### AND `0000 0011 0000 0000`

Bitwise AND

`W[cc] = W[aa] AND W[bb]`

- #### OR `0000 0100 0000 0000`

Bitwise OR

`W[cc] = W[aa] OR W[bb]`

- #### XOR `0000 0101 0000 0000`

Bitwise XOR

`W[cc] = W[aa] XOR W[bb]`

- #### INC `0000 0110 0000 00aa`

`W[aa] = W[aa] + 1`
If overflow occur, ignore carry bit.

- #### DEC `0000 0111 0000 00aa`

`W[aa] = W[aa] - 1`

- #### SET `0000 1000 aayy xxxx`

Set 4 bits of register `W[aa]` with `xxxx`.\
Remaining bits are unchanged.\
The bits are selected with `yy`:

`yy=00` -> `W[aa][0-3]`\
`yy=01` -> `W[aa][4-7]`\
`yy=10` -> `W[aa][8-11]`\
`yy=11` -> `W[aa][12-15]`

- #### CLR `0000 1010 0000 00aa`

Clear register `W[aa]`

- #### NOT `0000 1011 0000 00aa`

`W[aa] = ~W[aa]`
Bitwise NOT operation on register `W[aa]`.

## Branch instructions

- #### JMP `1000 0000 0000 00aa`

Set `IP = W[aa]`.

- #### BRE `1000 0010 00cc aabb`

If `W[aa] == W[bb]`, then set `IP = W[cc]`.

- #### BRNE `1000 0011 00cc aabb`

If `W[aa] != W[bb]`, then set `IP = W[cc]`.

- #### BRL `1000 0100 00cc aabb`

If `W[aa] < W[bb]`, then set `IP = W[cc]`.

- #### BRH `1000 0101 00cc aabb`

If `W[aa] > W[bb]`, then set `IP = W[cc]`.

- #### BRNZ `1000 1001 0000 ccaa`

If `W[aa] != 0`, then set `IP = W[cc]`.

- #### CALL `1000 0110 0000 00aa`

Performs a function call:
```
SP = SP - 2
*SP = IP
IP = W[aa]
```

- #### RET `1000 0111 0000 0000`

Returns from a function call
```
IP = *SP
SP = SP + 2
```

- #### RETI `1000 1000 0000 0000`

Same as `RET`, but also sets the `GLE` flag on the `CR` register.

## Memory instructions

- #### LD `0100 0001 0000 aabb`

Load contents of memory address given by `W[aa]`, on the bank selected on `BK` into the register `W[bb]`.

- #### ST `0100 0010 0000 aabb`

Store the contents of `W[bb]` in address given by `W[aa]`, on the bank selected on `BK`.

- #### MV `0100 0011 0000 aabb`

Move the contents of register `W[aa]` to the register `W[bb]`.

- #### PUSH `0100 1000 0000 00aa`

Push the contents of register `W[aa]` on the stack:

```
SP = SP - 2
*SP = W[aa]
```

- #### POP `0100 1001 0000 00aa`

Pop a value from top of the stack on register `W[aa]`:

```
W[aa] = *SP
SP = SP + 2
```


### Control instructions

- #### DAI `1100 0000 0000 0000`

Disable global interrupt

- #### EAI `1100 0001 0000 0000`

Enable global interrupt

- #### DTI `1100 0010 0000 000a`

Disable time interrupt `a`

- #### ETI `1100 0011 0000 000a`

Enable time interrupt `a`

- #### SELB `1100 0100 0000 00aa`

Select Memory Bank `aa` in `CR` register

- #### HLT `1111 1111 0000 0000`

Halt system

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

### I/O

TODO

## Peripherals

- #### Keyboard

TODO

- #### Video

Micro16 comes with a 320x200 16color screen.
The color map is given:

| Color                                                                                 | Color
| ---                                                                                   | ---
| <span style="background-color: #191919; color: white">&nbsp;0000&nbsp;</span>         | <span style="background-color: #ccdb25; color: black">&nbsp;1000&nbsp;</span>
| <span style="background-color: #cbcbcb; color: black">&nbsp;0001&nbsp;</span>         | <span style="background-color: #ccdb88; color: black">&nbsp;1001&nbsp;</span>
| <span style="background-color: #ac3232; color: black">&nbsp;0010&nbsp;</span>         | <span style="background-color: #d2842a; color: black">&nbsp;1010&nbsp;</span>
| <span style="background-color: #ac716b; color: black">&nbsp;0011&nbsp;</span>         | <span style="background-color: #d2ac7a; color: black">&nbsp;1011&nbsp;</span>
| <span style="background-color: #4fac43; color: black">&nbsp;0100&nbsp;</span>         | <span style="background-color: #824aad; color: black">&nbsp;1100&nbsp;</span>
| <span style="background-color: #92b687; color: black">&nbsp;0101&nbsp;</span>         | <span style="background-color: #b795c2; color: black">&nbsp;1101&nbsp;</span>
| <span style="background-color: #5b69ac; color: black">&nbsp;0110&nbsp;</span>         | <span style="background-color: #1f7d6e; color: black">&nbsp;1110&nbsp;</span>
| <span style="background-color: #acadc8; color: black">&nbsp;0111&nbsp;</span>         | <span style="background-color: #87ccc8; color: black">&nbsp;1111&nbsp;</span>

Each pixel is mapped in memory bank 1 from 0x0000-0x7cff.

- #### Disk

TODO
