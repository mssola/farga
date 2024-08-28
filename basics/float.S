.text
.globl greater_than_ten
.type greater_than_ten, @function

// bool greater_than_ten(double a, uint64_t b);
greater_than_ten:
    // Upon entry a -> fa0; and b -> a0. That is, the ABI increments the
    //(f)a<n> register per argument type.

    // Change the rounding to "Rounds Towards Zero" (bits: 0b001).
    li t0, 0b001
    fsrm t0

    // Convert the second parameter to double.
    fcvt.d.lu ft0, a0

    // ret = a + b
    fadd.d fa0, fa0, ft0

    // ret = (ret * b) + b
    fmv.d ft1, ft0
    fmadd.d fa0, fa0, ft0, ft1

    // Now we need to compare the number with 10.0. There are multiple ways to
    // load a floating point immediate:
    //   1. Loading an integer immediate and then convert it into a double. This
    //      is discouraged on the RISC-V assembly manual.
    //   2. Load a double with `fld` from a memory location on the data section.
    //   3. Load a floating point immediate with `fli` (requires the `Zfa`
    //      extension).
    // I don't have the `Zfa` extension on my board, so I'm opting for the
    // second way.
    fld ft1, .TEN, t0
    fle.d a0, ft1, fa0

    ret

.data
.TEN:
    .double 10.0
