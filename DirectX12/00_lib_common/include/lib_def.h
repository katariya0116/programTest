#pragma once

#define LIB_KATA kataLib
#define LIB_KATA_NAME #LIB_KATA
#define LIB_KATA_BEGIN namespace LIB_KATA {
#define LIB_KATA_END };

// 念のためdefineを切っておく（C++11環境じゃないところで切った時用）
#define LIB_ALIGNED(x) alignas(x)

// 配列数カウント
#define ARRAYOF(_array) sizeof(_array) / sizeof(_array[0])

// デバッグモード
#ifdef _DEBUG
#define DEBUG_MODE
#else
#define RELEASE_MODE
#endif