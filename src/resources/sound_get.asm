; license:MIT License
; copyright-holders:Hiromasa Tanaka
_sound_get:
    DB  0
    DW  $0000
    DW  _sound_get_trk3
    DW  _sound_get_trk3
_sound_get_trk3:
    DB  201, %10, 200, 15 , 47 , 1  ,  35, 1  , 47 , 1  , 200, 0
    DB  255
