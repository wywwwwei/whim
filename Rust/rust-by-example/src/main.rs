mod formatted_print;
mod scope;
mod destructuring;
mod closure;

fn main() {
    formatted_print::print_test();
    scope::scoping_test();
    destructuring::destructuring_test();
    closure::closure_test();
}
