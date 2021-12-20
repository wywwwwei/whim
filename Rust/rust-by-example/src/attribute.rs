// `#[allow(dead_code)]` is an attribute 
// that disables the `dead_code` lint
#[allow(dead_code)]
fn unused_function() {}

// Conditional check
// This function only gets compiled if the target OS is linux
#[cfg(target_os = "linux")]
fn are_you_on_linux() {
println!("You are running linux!");
}
// This function only gets compiled if the target OS is not linux
#[cfg(not(target_os = "linux"))]
fn are_you_on_linux() {
    println!("You are not running linux");
}

fn conditional_check_test() {
    println!("==============> Conditional Check Test");
    are_you_on_linux();
    
    if cfg!(target_os = "linux") {
        println!("It is definitely linux!");
    } else {
        println!("It is definitely not linux!");
    }
}

pub fn attribute_test() {
    println!("=======> Attribute Test");
    conditional_check_test();
}