extern crate clap;

use clap::{App, Arg};
use std::str::FromStr;

#[derive(Default)]
struct Privilege {
    exe: String,
    kind: String,
    write: u32,
    read: u32,
}

impl Privilege {
    fn new(exe: String, kind: String, write: u32, read: u32) -> Privilege {
        if exe.eq("") || kind.eq("") {
            return Privilege::default();
        }
        return Privilege {
            exe,
            kind,
            write,
            read,
        };
    }

    fn basic_new(exe: String, kind: String) -> Privilege {
        if exe.eq("") || kind.eq("") {
            return Privilege::default();
        }
        return Privilege {
            exe: exe,
            kind: kind,
            write: 0,
            read: 0,
        };
    }
}

fn add_privilege(privilege: Privilege) -> Result<(), String> {
    if privilege.exe.eq("") || privilege.kind.eq("") {
        return Err(String::from("Unassigned exe or type"));
    }
    // TODO
    // Sending process
    let content = format!(
        "{:16} {:16} {:16} {} {}",
        "add", privilege.exe, privilege.kind, privilege.write, privilege.read
    );
    println!("{}", content);
    Ok(())
}

fn update_privilege(privilege: Privilege) -> Result<(), String> {
    if privilege.exe.eq("") || privilege.kind.eq("") {
        return Err(String::from("Unassigned exe or type"));
    }
    // TODO
    // Sending process
    let content = format!(
        "{:16} {:16} {:16} {} {}",
        "update", privilege.exe, privilege.kind, privilege.write, privilege.read
    );
    println!("{}", content);
    Ok(())
}

fn delete_privilege(privilege: Privilege) -> Result<(), String> {
    if privilege.exe.eq("") || privilege.kind.eq("") {
        return Err(String::from("Unassigned exe or type"));
    }
    // TODO
    // Sending process
    let content = format!(
        "{:16} {:16} {:16} {} {}",
        "delete", privilege.exe, privilege.kind, privilege.write, privilege.read
    );
    println!("{}", content);
    Ok(())
}

fn main() -> Result<(), String> {
    let cmd = App::new("Firmiana")
        .version("0.0.1")
        .author("Snowfall99")
        .about("IS412: Firmiana")
        .subcommand(
            App::new("ADD")
                .about("Add privilege")
                .arg(
                    Arg::new("type")
                        .short('t')
                        .long("type")
                        .value_name("type")
                        .about("target type")
                        .required(true),
                )
                .arg(
                    Arg::new("exe")
                        .short('e')
                        .long("exe")
                        .value_name("exe")
                        .about("exe filename")
                        .required(true),
                )
                .arg(
                    Arg::new("write")
                        .short('w')
                        .long("write")
                        .value_name("write")
                        .about("write privilege")
                        .default_value("0"),
                )
                .arg(
                    Arg::new("read")
                        .short('r')
                        .long("read")
                        .value_name("read")
                        .about("read privilege")
                        .default_value("0"),
                ),
        )
        .subcommand(
            App::new("UPDATE")
                .about("Update privilege")
                .arg(
                    Arg::new("type")
                        .short('t')
                        .long("type")
                        .value_name("type")
                        .about("target type")
                        .required(true),
                )
                .arg(
                    Arg::new("exe")
                        .short('e')
                        .long("exe")
                        .value_name("exe")
                        .about("exe filename")
                        .required(true),
                )
                .arg(
                    Arg::new("write")
                        .short('w')
                        .long("write")
                        .value_name("write")
                        .about("write privilege")
                        .default_value("0"),
                )
                .arg(
                    Arg::new("read")
                        .short('r')
                        .long("read")
                        .value_name("read")
                        .about("read privilege")
                        .default_value("0"),
                ),
        )
        .subcommand(
            App::new("DELETE")
                .about("Delete privilege")
                .arg(
                    Arg::new("type")
                        .short('t')
                        .long("type")
                        .value_name("type")
                        .about("target type")
                        .required(true),
                )
                .arg(
                    Arg::new("exe")
                        .short('e')
                        .long("exe")
                        .value_name("exe")
                        .about("exe filename")
                        .required(true),
                ),
        )
        .subcommand(App::new("LIST").about("list all privileges"))
        .get_matches();

    match cmd.subcommand() {
        Some(("ADD", add_cmd)) => {
            let exe = match add_cmd.value_of("exe") {
                Some(exe) => exe.to_string(),
                None => String::from(""),
            };
            let kind = match add_cmd.value_of("type") {
                Some(kind) => kind.to_string(),
                None => String::from(""),
            };
            let write: u32 =
                FromStr::from_str(add_cmd.value_of("write").unwrap()).expect("parse write fail");
            let read: u32 =
                FromStr::from_str(add_cmd.value_of("read").unwrap()).expect("parse read fail");
            let privilege = Privilege::new(exe, kind, write, read);
            return add_privilege(privilege);
        }
        Some(("UPDATE", update_cmd)) => {
            let exe = match update_cmd.value_of("exe") {
                Some(exe) => exe.to_string(),
                None => String::from(""),
            };
            let kind = match update_cmd.value_of("type") {
                Some(kind) => kind.to_string(),
                None => String::from(""),
            };
            let write: u32 =
                FromStr::from_str(update_cmd.value_of("write").unwrap()).expect("parse write fail");
            let read: u32 =
                FromStr::from_str(update_cmd.value_of("read").unwrap()).expect("parse read fail");
            let privilege = Privilege::new(exe, kind, write, read);
            return update_privilege(privilege);
        }
        Some(("DELETE", delete_cmd)) => {
            let exe = match delete_cmd.value_of("exe") {
                Some(exe) => exe.to_string(),
                None => String::from(""),
            };
            let kind = match delete_cmd.value_of("type") {
                Some(kind) => kind.to_string(),
                None => String::from(""),
            };
            let privilege = Privilege::basic_new(exe, kind);
            return delete_privilege(privilege);
        }
        Some(("LIST", _)) => {
            println!("List cmd");
        }
        None => println!("No subcommand"),
        _ => println!("Unhandled subcommand"),
    }
    Ok(())
}
