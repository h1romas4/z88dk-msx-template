# Z88DK MSX build template with sample game

![](https://github.com/h1romas4/z88dk-msx-template/workflows/Build/badge.svg)

This repository contains sample games made with [Z88DK](https://github.com/z88dk/z88dk), [MAME](https://www.mamedev.org/) and [C-BIOS](http://cbios.sourceforge.net/).

@see [Z88DK を使って MSX のゲームをつくるための環境構築メモ](https://maple4estry.netlify.app/z88dk-msx/)

[Playable by WebMSX](https://webmsx.org/cbios/?MACHINE=MSX2J&ROM=https://github.com/h1romas4/z88dk-msx-template/releases/download/v1.5.1/example.rom)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/ponpon-01.png)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/ponpon-02.png)

## License

MIT License

## Special Thanks

- [Z88DK - The Development Kit for Z80 Computers](https://github.com/z88dk/z88dk) - Compiler and toolchain
- [Lovely Composer](https://1oogames.itch.io/lovely-composer) - Music Sequencer
- [@aburi6800](https://github.com/aburi6800/msx-PSGSoundDriver) - MSX Sound Driver and Convertor
- [MAME](https://www.mamedev.org/) - Emulator
- [openMSX](https://openmsx.org/) - Emulator
- [C-BIOS](http://cbios.sourceforge.net/) - MSX Open Source BIOS

## Build Require

- Ubuntu 20.04 LTS or Windows WSL2
- Setup [Z88DK](https://github.com/z88dk/z88dk/wiki/installation#linux--unix)
- cmake (`sudo apt install cmake`)

Set enviroments on `~/.bashrc`

```
# z88dk
export Z88DK_HOME=/home/hiromasa/devel/msx/z88dk
export ZCCCFG=${Z88DK_HOME}/lib/config
export PATH=${Z88DK_HOME}/bin:${PATH}
```

Verifiy

```
$ which zcc
/home/hiromasa/devel/msx/z88dk/bin/zcc
$ ls -laF ${ZCCCFG}/msx.cfg
-rw-rw-r-- 1 hiromasa hiromasa 1035  9月  1 12:10 /home/hiromasa/devel/msx/z88dk/lib/config/msx.cfg
$ zcc 2>&1 | head -5
zcc - Frontend for the z88dk Cross-C Compiler - v18586-be7c8763a-20210901

Usage: zcc +[target] {options} {files}
   -v -verbose                  Output all commands that are run (-vn suppresses)
   -h -help                     Display this text
```

## Build

Compile

```
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/z88dk.cmake ..
make
```

Verifiy

```
ls -laF ../dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa  16384  9月  3 18:13 example.rom
```

## Run with openMSX

Setup openMSX

[openMSX](https://openmsx.org/)

Run

```
$ ls -laF dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384  9月  3 18:13 dist/example.rom
$ openmsx dist/example.rom
```

Debug ([openmsx-debugger](https://github.com/openMSX/debugger))

Apply patch

-  [openmsx-debugger で z88dk の -m 形式の .map symbol を読ませる hack + Ubuntu 20.04 LTS Qt 5.14 Obsolete fix](https://gist.github.com/h1romas4/5f6579fcaad77cab3413ff437188a2f2)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/openmsx-debugger-02.png)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/openmsx-debugger-01.png)

## Run with MAME

First step (Create MSX virtual machine with `cbios`)

- [MSX ゲーム開発を MAME/C-BIOS で行うメモ](https://maple4estry.netlify.app/mame-msx-cbios/)

Set Enviroments

```
export MAME_HOME=/home/hiromasa/devel/amd64/mame
export MSX_HOME=$(pwd)
```

Verify

```
$ ls -laF ${MAME_HOME}/cbios
-rwxrwxr-x 1 hiromasa hiromasa 70065896  7月 25 14:04 /home/hiromasa/devel/amd64/mame/cbios*
```

Deploy ROM for MAME

```
$ cd ${MSX_HOME}
$ ls -laF dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384  9月  3 18:13 dist/example.rom
$ cd dist
$ zip -j ../mics/mame/roms/msx1_cart/example.zip example.rom
```

Run MAME

```
$ cd ${MSX_HOME}
(cd ${MAME_HOME} && ./cbios cbios example -window -resolution 800x600 \
    -rompath ${MSX_HOME}/mics/mame/roms/msx1_cart -hashpath ${MSX_HOME}/mics/mame/hash)
```

or Debug launch

```
(cd ${MAME_HOME} && ./hbf1 hbf1 example -window -resolution 800x600 \
    -rompath ${MSX_HOME}/mics/mame/roms/msx1_cart -hashpath ${MSX_HOME}/mics/mame/hash \
    -debug)
```

## Run with MAME (z88dk-gdb)

Enable `-debug` flag and re-build

`CMakefiles.txt`

```
add_compile_flags(C
    +msx
    -vn
    -llib3d
    -lm
    -lndos
    -lmsxbios
    -m
    -debug # Enable
    # https://github.com/z88dk/z88dk/wiki/Classic-allocation#automatic-heap-configuration
    -DAMALLOC
)
```

Set Enviroments

```
export MAME_HOME=/home/hiromasa/devel/amd64/mame
export MSX_HOME=$(pwd)
```

Verify

```
$ ls -laF ${MAME_HOME}/cbios
-rwxrwxr-x 1 hiromasa hiromasa 70065896  7月 25 14:04 /home/hiromasa/devel/amd64/mame/cbios*
```

Deploy debug ROM for MAME

```
$ cd ${MSX_HOME}
$ ls -laF dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384  9月  3 18:13 dist/example.rom
$ cd dist
$ zip -j ../mics/mame/roms/msx1_cart/example.zip example.rom
```

Run MAME with gdbstub

```
$ cd ${MSX_HOME}
$ (cd ${MAME_HOME} && ./cbios cbios example -window -resolution 800x600 \
    -rompath ${MSX_HOME}/mics/mame/roms/msx1_cart -hashpath ${MSX_HOME}/mics/mame/hash \
    -debugger gdbstub -debug)
gdbstub: listening on port 23946
```

Connect z88dk-gdb to MAME

```
$ cd ${MSX_HOME}
$ z88dk-gdb -h 127.0.0.1 -p 23946 -x dist/example.map
Reading debug symbols...OK
Connected to the server.
```

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/z88dk-gdb-01.png)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/z88dk-gdb-02.png)

VSCode Attach

`.vscode/launch.json`

```json
{
    "version": "0.2.0",
    "configurations": [
        {
          "name": "Attach to MAME gdbserver",
          "type": "gdb",
          "request": "attach",
          "target": "127.0.0.1:23946",
          "remote": true,
          "cwd": "${workspaceRoot}",
          "gdbpath": "${env:Z88DK_HOME}/bin/z88dk-gdb",
          "debugger_args": [
            "-x",
            "${workspaceRoot}/dist/example.map" // or appropriate .map of your project (-m -debug needed!)
          ],
          "autorun": [
          ]
      }
    ]
}
```

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/z88dk-gdb-03.png)

As of 2022-7, "Step Over" does not seem to return the operation. "Continue" and "Step into" work well, so please use them interchangeably.
