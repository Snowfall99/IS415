extern crate clap;

use clap::{App, Arg};
use std::str::FromStr;

fn main() {
    let cmd = App::new("Evil")
        .version("0.0.1")
        .author("Snowfall99")
        .about("IS415 Group 8: Evil test");
}
