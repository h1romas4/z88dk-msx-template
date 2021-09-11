#include <stdio.h>
#include <stdlib.h>

#include <msx/gfx.h>

#define MSX_CLIKSW  0xf3db
#define MSX_JIFFY   0xfc9e

#define VRAM_NONE   0x20
#define VRAM_START  0x1800
#define VRAM_WIDTH  32
#define VRAM_HEIGHT 24
#define VPOS(x, y)  (VRAM_START + VRAM_WIDTH * y + x)

// chars.asm::_chars ラベルの参照(VRAM PCG 転送用)
extern unsigned char chars[];

// ゲームの状態遷移
typedef enum {
    TITLE_INIT,
    TITLE_ADVERTISE,
    GAME_INIT,
    GAME_MAIN,
    GAME_OVER
} game_state_t;

// ゲームの状態
typedef struct {
    game_state_t state;
    uint8_t remein_clear;
    uint16_t score;
    uint16_t score_hi;
    uint8_t stick_state;
} game_t;

game_t game;

// ボールの状態
typedef struct {
    uint8_t x;
    uint8_t y;
    int8_t vy;
    uint16_t tick;
} ball_t;

ball_t ball;

// タイトルアドバタイズデモの状態
typedef struct {
    uint16_t y;
    int16_t vy;
    uint16_t tick;
    uint8_t trigger_state;
} title_t;

title_t title;

/**
 * vsync カウント待ち
 */
void wait_vsync(uint16_t count)
{
    uint16_t *interval_timer = (uint16_t *)MSX_JIFFY;
    *interval_timer = 0;
    while(*interval_timer < count);
}

/**
 * グラフィックス初期化
 */
void init_graphics()
{
    // スクリーンモード
    set_color(15, 1, 1);
    set_mangled_mode();

    // スプライトモード
    set_sprite_mode(sprite_default);

    // キークリックスイッチ(OFF)
    *(uint8_t *)MSX_CLIKSW = 0;

    // 画面クリア
    fill(VRAM_START, VRAM_NONE, VRAM_WIDTH * VRAM_HEIGHT);

    // PCG 設定(3面に同じデーターを転送)
    vwrite(chars, 0x0000, 0x800);
    vwrite(chars, 0x0800, 0x800);
    vwrite(chars, 0x1000, 0x800);

    // 色設定(0000|0000 = 前|背景)
    set_char_color('=', 0x54, place_all);
    set_char_color('$', 0xa0, place_all);
    set_char_color('>', 0x6d, place_all);
    set_char_color('?', 0x60, place_all);
}

/**
 * スコア等表示
 */
void print_state()
{
    uchar score_string[VRAM_WIDTH + 1];
    sprintf(score_string, "SCORE %06u  HISCORE %06u  %02u", game.score, game.score_hi, game.remein_clear);
    vwrite(score_string, VPOS(0, 0), VRAM_WIDTH);
}

/**
 * タイトル表示及びアドバタイズデモ用初期化
 */
void title_init()
{
    // 画面クリア
    fill(VRAM_START, VRAM_NONE, VRAM_WIDTH * VRAM_HEIGHT);

    game.remein_clear = 0;
    print_state();

    vwrite("========   =======  ==     ==   ", VPOS(0,  5), VRAM_WIDTH);
    vwrite("==     == ==     == ===    ==   ", VPOS(0,  6), VRAM_WIDTH);
    vwrite("==     == ==     == ====   ==   ", VPOS(0,  7), VRAM_WIDTH);
    vwrite("========  ==     == ==  == ==   ", VPOS(0,  8), VRAM_WIDTH);
    vwrite("==        ==     == ==   ====   ", VPOS(0,  9), VRAM_WIDTH);
    vwrite("==         =======  ==     ==   ", VPOS(0, 10), VRAM_WIDTH);
    vwrite("                                ", VPOS(0, 11), VRAM_WIDTH);
    vwrite("   ========   =======  ==     ==", VPOS(0, 12), VRAM_WIDTH);
    vwrite("   ==     == ==     == ===    ==", VPOS(0, 13), VRAM_WIDTH);
    vwrite("   ==     == ==     == ====   ==", VPOS(0, 14), VRAM_WIDTH);
    vwrite("   ========  ==     == ==  == ==", VPOS(0, 15), VRAM_WIDTH);
    vwrite("   ==        ==     == ==   ====", VPOS(0, 16), VRAM_WIDTH);
    vwrite("   ==         =======  ==     ==", VPOS(0, 17), VRAM_WIDTH);
    vwrite("          HIT SPACE KEY         ", VPOS(0, 22), VRAM_WIDTH);

    title.y = 6;
    title.tick = 0;
    title.vy = 1;

    // アドバタイズデモに遷移
    game.state = TITLE_ADVERTISE;
}

/**
 * タイトルアドバタイズデモ
 */
void title_advertise(uint8_t trigger)
{
    // フレーム間先行入力
    // ゲームオーバー後の連続入力無視のため最初の 30 tick は入力を受け付けない
    if(title.tick > 30 && trigger) {
        title.trigger_state = trigger;
    } else {
        title.trigger_state = 0;
    }

    // HIT SPACE KEY
    if(title.trigger_state) {
        // アドバタイズデモの経過 tick 数で乱数シードを初期化する
        // ぽんぽん目押しで好きな面を狙える
        seed_rnd(title.tick);
        // ゲーム初期化に遷移
        game.state = GAME_INIT;
    }

    // tick per 6
    if(title.tick++ % 6 != 0) return;

    // ぽんぽん
    vpoke(VPOS(14, title.y), VRAM_NONE);
    if(title.y >= 9) title.vy = -1;
    if(title.y <= 6) title.vy = 1;
    title.y += title.vy;
    vpoke(VPOS(14, title.y), '?');
}

/**
 * ゲーム（ボム及びゴールド配布）
 */
void game_randam_block(uint8_t count)
{
    // クリアまでの個数
    game.remein_clear = 10;
    print_state();

    // ゴールドと障害物
    for(uint8_t i = 0; i < count; i++) {
        uint8_t x = get_rnd() % 31 + 1;
        uint8_t y = get_rnd() % 21 + 1;
        uint16_t add = VPOS(x, y);
        if(vpeek(add) == VRAM_NONE || vpeek(add) == '>') {
            if(i % 3) {
                // ゴールドはボムを潰せる
                vpoke(add, '$');
            } else {
                // 自分の縦移動方向にはボムは配置しない
                if(ball.x != x) {
                    vpoke(add, '>');
                }
            }
        }
    }
}

/**
 * ゲーム初期化
 */
void game_init()
{
    // 画面クリア
    fill(VRAM_START, VRAM_NONE, VRAM_WIDTH * VRAM_HEIGHT);

    // 壁
    fill(VPOS(0,  1) , '=', VRAM_WIDTH);
    fill(VPOS(0, 22), '=', VRAM_WIDTH);
    for(uint8_t y = 2; y < 22; y++) {
        vpoke(VPOS( 0, y), '=');
        vpoke(VPOS(31, y), '=');
        if(y == 7 || y == 16) {
            vwrite("=======      ======      =======", VPOS(0, y), 32);
        }
    }

    // ボール（下から上へ）
    ball.x = 2;
    ball.y = 6;
    ball.vy = -1;

    // 初期ゴールドとボム生成
    game_randam_block(40);

    // ステートクリア
    ball.tick = 0;
    game.score = 0;
    game.stick_state = 0;

    // ステート表示
    print_state();

    // ゲームに遷移
    game.state = GAME_MAIN;
}

/**
 * ゲームオーバー
 */
void game_over(uint8_t trigger)
{
    vwrite("=                              =", VPOS(0,  9), VRAM_WIDTH);
    vwrite("=          GAME OVER           =", VPOS(0, 10), VRAM_WIDTH);
    vwrite("=                              =", VPOS(0, 11), VRAM_WIDTH);
    vwrite("=        HIT SPACE KEY         =", VPOS(0, 12), VRAM_WIDTH);
    vwrite("=                              =", VPOS(0, 13), VRAM_WIDTH);

    // HIT SPACE KEY
    if(trigger) {
        // タイトルに遷移
        game.state = TITLE_INIT;
    }
}

/**
 * ゲームメイン
 */
void game_main(uint8_t stick)
{
    uint8_t ball_next_x;
    uint8_t ball_next_y;
    int8_t ball_vx = 0;

    // フレーム間先行入力
    if(stick & st_left || stick & st_right) {
        game.stick_state = stick;
    }

    // tick per 6
    if(ball.tick++ % 6 != 0) return;

    // ボール移動先算出
    ball_next_y = ball.y + ball.vy;
    if(ball.x > 1 && game.stick_state & st_left) ball_vx = -1;
    if(ball.x < 30 && game.stick_state & st_right) ball_vx += 1;
    ball_next_x = ball.x + ball_vx;

    // 判定
    uint8_t next_block = vpeek(VPOS(ball_next_x, ball_next_y));
    if(next_block == '=') {
        // 壁反射（ボール的 1 tick 縦移動を停止させる。アイテム破壊猶予）
        ball_next_y = ball_next_y - ball.vy;
        ball.vy = ball.vy * -1;
    } else if(next_block == '$') {
        // ゴールド取得
        game.score += 10;
        if(game.score > game.score_hi) {
            game.score_hi = game.score;
        }
        game.remein_clear--;
        print_state();
        // ネクストステージ
        if(game.remein_clear <= 0) {
            game_randam_block(20);
        }
    } else if(next_block == '>') {
        // ボム激突
        // ゲームオーバーに遷移
        game.state = GAME_OVER;
    }

    // ボールクリア
    vpoke(VPOS(ball.x, ball.y), VRAM_NONE);
    // ボール表示
    vpoke(VPOS(ball_next_x, ball_next_y), '?');

    // アップデート
    ball.x = ball_next_x;
    ball.y = ball_next_y;

    // フレーム間先行入力クリア
    game.stick_state = 0;
}

/**
 * ゲームループ及び状態遷移
 */
void loop()
{
    while(1) {
        // vsync wait
        wait_vsync(1);
        // 入力取得
        uint8_t stick = st_dir[get_stick(0)];
        uint8_t trigger = get_trigger(0);
        // ゲーム
        switch(game.state) {
            case TITLE_INIT:
                title_init();
                break;
            case TITLE_ADVERTISE:
                title_advertise(trigger);
                break;
            case GAME_INIT:
                game_init();
                break;
            case GAME_MAIN:
                game_main(stick);
                break;
            case GAME_OVER:
                game_over(trigger);
                break;
            default:
                break;
        }
    }
}

/**
 * メイン
 */
void main()
{
    // 画面初期化
    init_graphics();

    // ゲームステート初期化
    game.state = TITLE_INIT;
    game.score_hi = 300;

    // start
    loop();
}
