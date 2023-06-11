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
SP = SP + 2
*SP = IP
IP = W[aa]
```

- #### RET `1000 0111 0000 0000`

Returns from a function call
```
IP = *SP
SP = SP - 2
```

- #### RETI `1000 1000 0000 0000`

Same as `RET`, but also sets the `GLE` flag on the `CR` register.

## Memory instructions

- #### LD `0100 0001 0000 aabb`

Load contents of memory address given by `W[aa]`, on the bank selected on `BK` into the register `W[bb]`.

- #### ST `0100 0010 0000 aabb`

Store the contents of `W[bb]` in address given by `W[aa]`, on the bank selected on `BK`.

- #### CPY `0100 0011 0000 aabb`

Copy the contents of register `W[aa]` to the register `W[bb]`.

- #### PUSH `0100 1000 0000 00aa`

Push the contents of register `W[aa]` on the stack:

```
SP = SP + 2
*SP = W[aa]
```

- #### POP `0100 1001 0000 00aa`

Pop a value from top of the stack on register `W[aa]`:

```
W[aa] = *SP
SP = SP - 2
```

- #### PEEK `0100 1010 aaxx xxxx`

Load the value from address `SP - 0bxxxxxx` into `W[aa]`:

```
W[aa] = *(SP - xxxxxx)
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

- #### BRK `1111 1110 0000 0000`

Breakpoint (software handled) - Mostly used for testing.

- #### HLT `1111 1111 0000 0000`

Halt system
