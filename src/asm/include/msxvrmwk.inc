; ====================================================================================================
; MSX VRAM WORKAREA
; ====================================================================================================
PTN_GEN_ADDR:           EQU $0000   ; VRAM パターンジェネレータテーブルの先頭アドレス
PTN_NAME_ADDR:          EQU $1800   ; VRAM パターンネームテーブルの先頭アドレス
COLOR_TABLE_ADDR:       EQU $2000   ; VRAM カラーテーブルの先頭アドレス
SPR_PTN_ADDR:	        EQU $3800   ; VRAM スプライトパターンジェネレータの先頭アドレス
SPR_ATR_ADDR:	        EQU	$1B00   ; VRAM スプライトアトリビュートエリアの先頭アドレス