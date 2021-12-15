mod formatted_print;
mod scope;
mod destructuring;

fn main() {
    formatted_print::print_test();
    scope::scoping_test();
    destructuring::destructuring_test();
}
