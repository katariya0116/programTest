#pragma once

#define LIB_KATA kataLib
#define LIB_KATA_NAME #LIB_KATA
#define LIB_KATA_BEGIN namespace LIB_KATA {
#define LIB_KATA_END };

// �O�̂���define��؂��Ă����iC++11������Ȃ��Ƃ���Ő؂������p�j
#define LIB_ALIGNED(x) alignas(x)

// �z�񐔃J�E���g
#define ARRAYOF(_array) sizeof(_array) / sizeof(_array[0])

// �f�o�b�O���[�h
#ifdef _DEBUG
#define DEBUG_MODE
#else
#define RELEASE_MODE
#endif