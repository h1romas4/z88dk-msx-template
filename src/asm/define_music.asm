; license:MIT License
; copyright-holders:Hiromasa Tanaka

; rodata_user
; https://github.com/z88dk/z88dk/blob/master/doc/overview.md#a-quick-note-for-asm-code
; rodata_user if for constant data
; kept in rom if program is in rom
SECTION rodata_user
PUBLIC _music_title, _music_main, _music_game_over, _sound_extend, _sound_get

include "../resources/music_title.asm"
include "../resources/music_main.asm"
include "../resources/music_game_over.asm"
include "../resources/sound_extend.asm"
include "../resources/sound_get.asm"
