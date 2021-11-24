; license:MIT License
; copyright-holders:Hiromasa Tanaka
_sound_extend:
    DB  0
    DW  _sound_extend_trk1
    DW  _sound_extend_trk2
    DW  _sound_extend_trk3
_sound_extend_trk1:
    DB  201, %10, 200, 14 , 20 , 15 , 200, 0  , 0  , 5  , 200, 14 , 20 , 5  , 200, 0
    DB  0  , 5  , 200, 14 , 20 , 5  , 200, 0  , 0  , 5  , 200, 14 , 20 , 5  , 200, 0
    DB  0  , 5  , 200, 14 , 20 , 30 , 22 , 30 , 24 , 50
    DB  255
_sound_extend_trk2:
    DB  201, %10, 200, 14 , 60 , 15 , 200, 0  , 0  , 5  , 200, 14 , 60 , 5  , 200, 0
    DB  0  , 5  , 200, 14 , 60 , 5  , 200, 0  , 0  , 5  , 200, 14 , 60 , 5  , 200, 0
    DB  0  , 5  , 200, 14 , 60 , 30 , 62 , 30 , 64 , 50
    DB  255
_sound_extend_trk3:
    DB  201, %10, 200, 14 , 56 , 15 , 200, 0  , 0  , 5  , 200, 14 , 56 , 5  , 200, 0
    DB  0  , 5  , 200, 14 , 56 , 5  , 200, 0  , 0  , 5  , 200, 14 , 56 , 5  , 200, 0
    DB  0  , 5  , 200, 14 , 56 , 30 , 58 , 30 , 60 , 50
    DB  255
