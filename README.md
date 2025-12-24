# Z88DK MSX build template with sample game

![](https://github.com/h1romas4/z88dk-msx-template/workflows/Build/badge.svg) ![](https://github.com/h1romas4/z88dk-msx-template/workflows/Release/badge.svg)

This repository contains sample games and utilities for MSX development using [Z88DK](https://github.com/z88dk/z88dk), [MAME](https://www.mamedev.org/), and [C-BIOS](http://cbios.sourceforge.net/).

See also: [Z88DK を使って MSX のゲームをつくるための環境構築メモ](https://maple4estry.netlify.app/z88dk-msx/)

[Playable on WebMSX](https://webmsx.org/?MACHINE=MSX2J&ROM=https://github.com/h1romas4/z88dk-msx-template/releases/download/v1.6.0/example.rom)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/ponpon-01.png)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/ponpon-02.png)

## License

MIT License

## Special Thanks

- [Z88DK - The Development Kit for Z80 Computers](https://github.com/z88dk/z88dk) - Compiler and toolchain
- [Lovely Composer](https://1oogames.itch.io/lovely-composer) - Music sequencer
- [@aburi6800](https://github.com/aburi6800/msx-PSGSoundDriver) - MSX sound driver and converter
- [MAME](https://www.mamedev.org/) - Emulator
- [openMSX](https://openmsx.org/) - Emulator
- [C-BIOS](http://cbios.sourceforge.net/) - MSX open-source BIOS
- [Native Debug](https://github.com/WebFreak001/code-debug) - Native debugging for VS Code
- [DeZog](https://github.com/maziac/DeZog) - Visual Studio Code Debugger for Z80/ZX Spectrum.

## Build

### Requirements

- Ubuntu 24.04 LTS or Ubuntu 22.04 LTS or Windows WSL2
- Z88DK v2.4
    - Setup [Z88DK](https://github.com/z88dk/z88dk/wiki/installation#linux--unix)
    - [.github/workflows/build-release.yml](https://github.com/h1romas4/z88dk-msx-template/blob/833b328d5be6da5c8071ae7fa85b6f0d8f48729c/.github/workflows/build-release.yml#L20-L45)
- cmake (`sudo apt install cmake`)

Set environment variables in `~/.bashrc`

```
# z88dk
export Z88DK_HOME=/home/hiromasa/devel/msx/z88dk
export ZCCCFG=${Z88DK_HOME}/lib/config
export PATH=${Z88DK_HOME}/bin:${PATH}
```

Verify

```
$ which zcc
/home/hiromasa/devel/msx/z88dk/bin/zcc
$ ls -laF ${ZCCCFG}/msx.cfg
-rw-rw-r-- 1 hiromasa hiromasa 1627 12月 22 20:18 /home/hiromasa/devel/msx/z88dk/lib/config/msx.cfg
$ zcc 2>&1 | head -5
zcc - Frontend for the z88dk Cross-C Compiler - v23854-4d530b6eb7-20251222

Usage: zcc +[target] {options} {files}
   -v -verbose                  Output all commands that are run (-vn suppresses)
   -h -help                     Display this text
```

### Build

Compile

```
cmake -S . -B build
cmake --build build
```

Verify

```
$ ls -laF ../dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384 12月 24 14:11 dist/example.rom
```

## Run with openMSX

Setup openMSX

[openMSX](https://openmsx.org/)

Run

```
$ ls -laF dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384 12月 24 14:11 dist/example.rom
$ openmsx dist/example.rom
```

## Run with MAME (z88dk-gdb)

Setup MAME

@see [DeZog + Z88DK + MAME で MSX アセンブリーをデバッグする手順](https://maple4estry.netlify.app/dezog-mame-msx/)

Enable `-debug` flag

`CMakeLists.txt`

```
target_compile_options(${PROJECT_NAME} PRIVATE
    +msx
    -O2
    -vn
    -m
    #$<$<COMPILE_LANGUAGE:C>:-debug> # <- Enable this line
    $<$<COMPILE_LANGUAGE:ASM>:--list>
)
```

Build with debug

```
rm -Rf build/
cmake -S . -B build
cmake --build build
```

Verify

Deploy debug ROM for MAME

```
$ ls -laF dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384  9月  3 18:13 dist/example.rom
$ cd dist
$ zip -j ../mics/mame/roms/msx1_cart/example.zip example.rom
```

Run MAME with gdbstub

```
$ cd mics/mame
$ ./mame cbiosm1jp example -debugger gdbstub -debug
gdbstub: listening on port 23946
```

Connect z88dk-gdb to MAME

```
$ z88dk-gdb -h 127.0.0.1 -p 23946 -x dist/example.map
Reading debug symbols...OK
Connected to the server.
```

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/z88dk-gdb-01.png)

![](https://raw.githubusercontent.com/h1romas4/z88dk-msx-template/main/docs/images/z88dk-gdb-02.png)

VSCode Attach ([Native Debug](https://github.com/WebFreak001/code-debug))

- Breakpoints may shift; set breakpoints after startup.
- As of July 2022, "Step Over" may not behave reliably. Use "Continue" or "Step Into" instead.

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
