use std::fmt;

// fmt::Debug
struct UnPrintable(i32);

#[derive(Debug)]
struct DebugPrintable(i32);

#[derive(Debug)]
struct DebugPrintableWrap(DebugPrintable);

// fmt::Display
struct Displayable(i32, i32);

impl fmt::Display for Displayable {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "({}, {})", self.0, self.1)
    }
}

struct List(Vec<i32>);

impl fmt::Display for List {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let vec = &self.0;
        // '?' operator: if it errors, return the error. Otherwise continue.
        write!(f, "[")?;
        for (count, v) in vec.iter().enumerate() {
            if count != 0 {
                write!(f, ", ")?;
            }
            write!(f, "{}", v)?;
        }
        write!(f, "]")
    }
}

// fmt::Debug: Uses the {:?} marker. Format text for debugging purposes.
// fmt::Display: Uses the {} marker. Format text in a more elegant, user friendly fashion.
pub fn print_test() {
    println!("Now {:?} will print!", DebugPrintable(3));
    println!(
        "Now {:?} will print!",
        DebugPrintableWrap(DebugPrintable(3))
    );
    println!("Now {} will print!", Displayable(1, 2));

    let v = List(vec![1, 2, 3]);
    println!("{}", v);
}