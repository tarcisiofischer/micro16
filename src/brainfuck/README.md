# Brainfuck compiler

Transforms a brainfuck (`.bf`) file into a `m16asm` file, which can then be turned into machine code
using `micro16_asm`.

Brainfuck is "an esoteric programming language " (...) "Notable for its extreme minimalism".
(See https://en.wikipedia.org/wiki/Brainfuck).

### Considerations

The `.` command is mapped to the [Micro16's Video Memory](../../docs/cpu-manual.md).
Each time `.` is called, it moves the 4 lower bits of the current memory pointer to the
video memory *and advances the "video pointer"*. This is meant to be similar to writing
a `char` and advancing (which is usually the case for Brainfuck interpreters).

Example: `+....` will set the 4 first pixels with white (Even without moving the memory pointer!)

The `,` command currently does nothing.
