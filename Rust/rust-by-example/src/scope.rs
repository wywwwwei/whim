// RAII
#[derive(Debug)]
struct StrWrap(&'static str);

impl Drop for StrWrap {
    fn drop(&mut self) {
        println!("{:?} is being dropped", &self);
    }
}

fn create_box() {
    let _box1 = Box::new(StrWrap("Box1"));
    // `_box1` is destroyed here, and memory gets freed
}

fn raii_test() {
    println!("==============> RAII Test");
    let _box2 = Box::new(StrWrap("Box2"));
    
    // A nested scope:
    {
        let _box3 = Box::new(StrWrap("Box3"));
        // `_box3` is destroyed here, and memory gets freed
    }

    create_box();
    // `_box2` is destroyed here, and memory gets freed
}

fn print_box(c: Box<u32>) {
    println!("c contains {}", c);
    // `c` is destroyed and the memory freed
}

fn owner_test() {
    println!("==============> Ownership Test");
    // Stack allocated integer
    let x = 5u32;
    let y = x;
    println!("x is {}, and y is {}", x, y);
    // A pointer to a heap allocated integer
    let a = Box::new(5u32);
    println!("a contains {}", a);
    // `a` can no longer access the data after move, because it no longer owns the heap memory
    let b = a;
    println!("b contains {}", b);
    // This function takes ownership of the heap allocated memory from `b`
    print_box(b);
}

fn partial_move_test() {
    println!("==============> Partial Move Test");
    #[derive(Debug)]
    struct Person {
        name: String,
        age: u8,
    }

    let person = Person {
        name: String::from("Alice"),
        age: 20,
    };

    let Person { name, ref age } = person;
    println!("The person's age is {}", age);
    println!("The person's name is {}", name);
    // `person` cannot be used but `person.age` can be used as it is not moved
    println!("The person's age from person struct is {}", person.age);
}

// This function takes ownership of a box and destroys it
fn eat_box_i32(boxed_i32: Box<i32>) {
    println!("Destroying box that contains {}", boxed_i32);
}

fn borrow_i32(borrowed_i32: &i32) {
    println!("This int is {}", borrowed_i32);
}

fn borrow_test() {
    println!("==============> Borrow Test");
    let boxed_i32 = Box::new(5_i32);
    let stacked_i32 = 6_i32;

    // Borrow the contents of the box. Ownership is not taken,
    // so the contents can be borrowed again.
    borrow_i32(&boxed_i32);
    borrow_i32(&stacked_i32);

    {
        let _ref_to_i32 = &boxed_i32;
        // Can't destroy `boxed_i32` while the inner value is borrowed later in scope.
        // eat_box_i32(boxed_i32);
        borrow_i32(_ref_to_i32);
    }
    // `boxed_i32` can now give up ownership to `eat_box` and be destroyed
    eat_box_i32(boxed_i32);
}

pub fn scoping_test() {
    println!("=======> Scoping Test");
    raii_test();
    owner_test();
    partial_move_test();
    borrow_test();
    println!();
}
