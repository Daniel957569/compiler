- optimization
- codegen

fun x(): int {
    let b: int = 30:
    while b == 30 {
        b = b + 1;
    }

    if b == 21 {
        b = b * 10;
    }

    return b;
}


fun main(): int {
    let output: int = x();
}

