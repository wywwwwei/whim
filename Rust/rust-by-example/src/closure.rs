fn closure_and_fnuction() {
    println!("==============> Closure and function example");

    // Increment via closures and functions
    fn function(i: i32) -> i32 {
        i + 1
    }
    // For closure, both input and return types can be inferred
    // and input variable names must be specified.
    let closure_annotated = |i: i32| i + 1;
    let closure_inferred = |i| i + 1;

    let i = 1;
    println!("function {}", function(i));
    println!("closure_annotated: {}", closure_annotated(i));
    println!("closure_inferred: {}", closure_inferred(i));

    // The return type is inferred
    let one = || i;
    println!("closure returning one: {}", one());
}

fn closure_capture() {
    use std::mem;
    println!("==============> Closure capturing");

    // Closures preferentially capture variables by reference and only go lower when required.

    // immutable reference example
    let color = String::from("green");
    let print = || println!("`color`: {}", color);
    print();
    // `color` can be borrowed immutably again, because the closures only
    // hold an immutable reference to `color`
    let _reborrow = &color;
    print();
    let _color_moved = color;

    // mutable reference example
    let mut count = 0;
    // A `mut` is required on `inc` because a `&mut` is stored inside.
    // Thus, calling the closure mutates the closure which requires a `mut`
    let mut inc = || {
        count += 1;
        println!("`count`: {}", count);
    };
    inc();
    inc();
    let _count_reborrowed = &mut count;

    // a non-copy type
    let movable = Box::new(3);
    // `mem::drop` requires `T` so this must take by value.
    // For a copy type, it would copy into the closure
    // leaving the original untouched.
    // For a non-copy type, it must move and so `movable`
    // immediately moves into the closure.
    let comsume = || {
        println!("`movable: {:?}`", movable);
        mem::drop(movable);
    };
    // `cosume` cosumes the variable so this can only
    // be called once.
    comsume();

    let haystack = vec![1, 2, 3];
    // using `move` forces closure to take ownership of captured variables
    let contains = move |needle| haystack.contains(needle);
    println!("{}", contains(&1));
    println!("{}", contains(&4));
    // here we are not allowed re-using variable `haystack`
    // after it has been moved.
}

fn closure_as_input_parameter() {
    println!("==============> Closure as input parameters");

    // When a closure is defined, the compiler implicitly creates
    // a new anonymous structure to store the captured variables inside,
    // meanwhile implementing the functionality via one of the traits:
    // Fn, FnMut, or FnOnce for this unknown type.
    // This type is assigned to the variable which is stored until calling.

    // Since this new type is of unknown type, any usage in a function will require generics.
    // However, an unbounded type parameter <T>
    // would still be ambiguous and not be allowed.
    // Thus, bounding by one of the traits: Fn, FnMut, or FnOnce
    // (which it implements) is sufficient to specify its type.
    fn apply<F>(f: F)
    where
        F: FnOnce(),
    {
        f();
    }

    fn apply_to_3<F>(f: F) -> i32
    where
        F: Fn(i32) -> i32,
    {
        f(3)
    }

    use std::mem;
    let greeting = "hello";
    // A non-copy type.
    // `to_owned` creates owned data from borrowed one
    let mut farewell = "goodbye".to_owned();
    // Capture 2 variables: `greeting` by reference and
    // `farewell` by value.
    let diary = || {
        // `greeting` is by reference: requires `Fn`.
        println!("I said {}", greeting);

        // Mutation forces `farewell` to be captured by
        // mutable reference. Now requires `FnMut`.
        farewell.push_str("!!!");
        println!("Then I screamed {}.", farewell);
        println!("Now I can sleep. zzzzz");

        // Manually calling drop forces `farewell` to
        // be captured by value. Now requires `FnOnce`.
        mem::drop(farewell);
    };
    apply(diary);

    let double = |x| 2 * x;
    println!("3 doubled: {}", apply_to_3(double));
}

fn function_as_input_parameter() {
    println!("==============> Function as input parameters");

    // Define a function which takes a generic `F` argument
    // bounded by `Fn`, and calls it
    fn call_me<F: Fn()>(f: F) {
        f();
    }
    // Define a wrapper function satisfying the `Fn` bound
    fn function() {
        println!("I am a function.");
    }

    let closure = || println!("I am a closure.");
    call_me(function);
    call_me(closure);
}

fn closure_as_output_parameter() {
    println!("==============> Closure as output parameters");

    // Anonymous closure types are, by definition, unknown,
    // so we have to use impl Trait to return them.
    fn create_fn() -> impl Fn() {
        let text = "Fn".to_owned();
        // The move keyword must be used,
        // which signals that all captures occur by value.
        // Any captures by reference would be dropped
        // as soon as the function exited,
        // leaving invalid references in the closure.
        move || println!("This is a {}", text)
    }

    let fn_plain = create_fn();
    fn_plain();
}

pub fn closure_test() {
    println!("=======> Closure Test");
    closure_and_fnuction();
    closure_capture();
    closure_as_input_parameter();
    function_as_input_parameter();
    closure_as_output_parameter();
}
