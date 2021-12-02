extern crate clap;

use clap::{App, Arg};
use std::fs;
use std::fs::{File, OpenOptions};
use std::os::unix::fs::PermissionsExt;
use std::io::prelude::*;

fn main() {
    let cmd = App::new("Evil")
        .version("0.0.1")
        .author("Snowfall99")
        .about("IS415 Group 8: Evil test")
        .subcommand(App::new("WRITE")
            .about("test WRITE privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for write")))
        .subcommand(App::new("READ")
            .about("test READ privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for read")))
        .subcommand(App::new("OPEN")
            .about("test OPEN privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for open")))
        .subcommand(App::new("CHMOD")
            .about("test CHMOD privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for chmod")))
        .subcommand(App::new("CREAT")
            .about("test CREAT privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for creat")))
        .subcommand(App::new("MKDIR")
            .about("test MKDIR privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for mkdir")))
        .subcommand(App::new("RMDIR")
            .about("test RMDIR privilege")
            .version("0.0.1")
            .arg(Arg::new("target")
                .short('t')
                .about("target file for rmdir")))
        .get_matches();

    match cmd.subcommand_name() {
        Some("READ") => {
            let read = read(); match read {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e), 
            }
        }
        Some("WRITE") => {
            let write = write(); match write {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e),
            }
        }
        Some("OPEN") => {
            let open = open(); match open {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e),
            }
        }
        Some("CHMOD") => {
            let chmod = chmod(); match chmod {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e),
            }
        }
        Some("CREAT") => {
            let creat = creat(); match creat {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e),
            }
        }
        Some("MKDIR") => {
            let mkdir = mkdir(); match mkdir {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e),
            }
        }
        Some("RMDIR") => {
            let rmdir = rmdir(); match rmdir {
                Ok(_) => println!("OK"),
                Err(e) => eprintln!("{}", e),
            }
        }
        None => println!("No subcommand was used"),
        _ => unreachable!(),
    };
}

fn read() -> std::io::Result<()> {
    let mut file = File::open("test.txt")?;
    let mut content = String::new();
    file.read_to_string(&mut content)?;
    assert_eq!(content, "hello world\n");
    Ok(())
}

fn write() -> std::io::Result<()> {
    let mut file = OpenOptions::new()
        .write(true)
        .truncate(true)
        .open("test.txt")
        .unwrap();
    file.write(b"hi\n")?;
    Ok(())
}

fn open() -> std::io::Result<()> {
    let _file = File::open("test.txt")?;
    Ok(())
}

fn creat() -> std::io::Result<()> {
    let _file = File::create("new.txt")?;
    Ok(())
}

fn chmod() -> std::io::Result<()> {
    fs::set_permissions("test.txt", fs::Permissions::from_mode(0o644)).unwrap();
    Ok(())
}

fn mkdir() -> std::io::Result<()> {
    fs::create_dir("./new")?;
    Ok(())
}

fn rmdir() -> std::io::Result<()> {
    fs::remove_dir("./new")?;
    Ok(())
}
