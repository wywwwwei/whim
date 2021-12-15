fn tuple_test() {
    println!("==============> Destructuring Tuple Test");
    let triple = (0, -2, 3);
    println!("Tell me about {:?}", triple);

    // Match can be used to destructure a tuple
    match triple {
        (0, y, z) => println!("First is `0`, `y` is {:?}, and `z` is {:?}", y, z),
        // `..` can be used to ignore the rest of the tuple
        (1, ..) => println!("First is `1`, and the rest doesn't matter"),
        // `_` means don't bind the value to a variable
        _ => println!("It doesn't matter what they are"),
    }
}

fn enum_test() {
    println!("==============> Destructuring Enum Test");
    enum Color {
        Red,
        Green,
        Blue,
        RGB(u32, u32, u32),
        HSV(u32, u32, u32),
        HSL(u32, u32, u32),
        CMY(u32, u32, u32),
    }

    let color = Color::RGB(122, 17, 40);
    match color {
        Color::Red => println!("The color is Red!"),
        Color::Green => println!("The color is Green!"),
        Color::Blue => println!("The color is Blue!"),
        Color::RGB(r, g, b) => println!("Red: {}, green: {}, and blue: {}!", r, g, b),
        Color::HSV(h, s, v) => println!("Hue: {}, saturation: {}, value: {}!", h, s, v),
        Color::HSL(h, s, l) => println!("Hue: {}, saturation: {}, lightness: {}!", h, s, l),
        Color::CMY(c, m, y) => println!("Cyan: {}, magenta: {}, yellow: {}!", c, m, y),
    }
}

fn pointer_test() {
    println!("==============> Destructuring Pointer Test");
    let reference = &4;
    match reference {
        &val => println!("Got a value via destructuring: {:?}", val),
    }
    match *reference {
        val => println!("Got a value via dereferencing: {:?}", val),
    }

    let value = 5;
    let mut mut_value = 6;
    match value {
        ref r => println!("Got a reference to a value: {:?}", r),
    }
    match mut_value {
        ref mut m => {
            *m += 10;
            println!("We add 10. `mut_value`: {:?}", m);
        }
    }
}

fn struct_test() {
    println!("==============> Destructuring Struct Test");
    struct Foo {
        x: (u32, u32),
        y: u32,
    }

    let foo = Foo { x: (1, 2), y: 3 };

    match foo {
        Foo { x: (1, b), y } => println!("First of x is 1, b = {}, y = {}", b, y),
        Foo { y: 2, x: i } => println!("y is 2, i = {:?}", i),
        Foo { y, .. } => println!("y = {}, we don't care about x", y),
    }
}

pub fn destructuring_test() {
    println!("=======> Destructuring Test");
    tuple_test();
    enum_test();
    pointer_test();
    struct_test();
    println!();
}
