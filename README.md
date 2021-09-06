# Z88DK MSX build template with sample game

## Require

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

Verification

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
cmake ..
make
```

Verification

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

## Run with MAME

First step

- [MSX ゲーム開発を mame/C-BIOS で行うメモ](https://maple4estry.netlify.app/mame-msx-cbios/)

Set MAME_HOME on `~/.bashrc`

```
export MAME_HOME=/home/hiromasa/devel/amd64/mame
```

Add example software section to `${MAME_HOME}/hash/msx1_cart.xml`

```
$ vi ${MAME_HOME}/hash/msx1_cart.xml

	<software name="example">
		<description>example</description>
		<year>2021</year>
		<publisher>example</publisher>
		<info name="serial" value="01" />
		<info name="alt_title" value="example" />
		<part name="cart" interface="msx_cart">
			<dataarea name="rom" size="16384">
				<rom name="example.rom" size="16384" crc="4e20d256" sha1="33536dac686b375ba13faf76a3baf2d6978904e0" offset="0x0" />
			</dataarea>
		</part>
	</software>
```

Deploy ROM to MAME

```
$ ls -laF dist/*.rom
-rw-rw-r-- 1 hiromasa hiromasa 16384  9月  3 18:13 dist/example.rom
$ zip -j -u ${MAME_HOME}/roms/msx1_cart/example.zip ./dist/example.rom
```

Run MAME

```
(cd ${MAME_HOME} && ./cbios cbios example -window -resolution 800x600)
```

or Debug launch

```
(cd ${MAME_HOME} && ./cbios cbios example -window -resolution 800x600 -debug)
```

## Run with MAME (z88dk-gdb)

Run MAME with gdbstub

```
$ (cd ${MAME_HOME} && ./cbios cbios example -window -resolution 800x600 -debugger gdbstub -debug)
gdbstub: listening on port 23946
```

Connect z88dk-gdb to MAME

```
$ z88dk-gdb -h 127.0.0.1 -p 23946 -x dist/example.map
Reading debug symbols...OK
Connected to the server.
                    di                                      ;[0000] f3

$0000()> break _main
Adding breakpoint at '_main' $461d (_main)

$0000()> cont
Hit breakpoint 1: @461d (_main)
_main:
                    ld      c,(hl)                          ;[461d] 4e

$461d(_main+0)> dis
_main:
                    ld      c,(hl)                          ;[461d] 4e
                    inc     hl                              ;[461e] 23
                    ld      b,(hl)                          ;[461f] 46
                    ld      hl,$0006                        ;[4620] 21 06 00
                    add     hl,bc                           ;[4623] 09
                    call    $4f03                           ;[4624] cd 03 4f
                    pop     de                              ;[4627] d1
                    add     hl,de                           ;[4628] 19
                    pop     de                              ;[4629] d1
                    call    $4f47                           ;[462a] cd 47 4f
```

`CMakefiles.txt`

```
add_compile_flags(C
    +msx
    -vn
    -llib3d
    -lm
    -lndos
    -lmsxbios
    -create-app
    -subtype=rom
    -m
    #-debug # add this line ?
    -s
    -DAMALLOC
)
```

## License

MIT License

## Thanks

[Z88DK - The Development Kit for Z80 Computers](https://github.com/z88dk/z88dk)
