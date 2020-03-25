# FariFile compiler

## Authors

* [Louis JACOTOT](mailto:louis.jacotot@telecomnancy.net)
* [Henry LAGARDE](mailto:henry.lagarde@telecomnancy.net)

## Requirements

* GCC installed
* javac installed

## Options

* -k : Compilation on errors
* < name > : Pass the name of the Farifile if none given will use farifile by default

## Usage

```sh
make
chmod +x fari
cp fari $PROJECT
cd $PROJECT
./fari <$FARIFILE | -k>
```

## How to create a FariFile

A FariFile is made with this options :

    # : To Comments text
    C <files> : Path to one or multiple C sources files
    H <files> : Path to one or multiple headers files
    E <name> : Path to the final file to create
    F <names> : Flags for the GCC compilation
    B <names> : Librairies or path to object file to use during the compilation
    J <files> : Path to one or multiples JAVA sources files (When use it will only compile the java file)

An exemple of Farifile and it's usage can be found in the test/ folder.
