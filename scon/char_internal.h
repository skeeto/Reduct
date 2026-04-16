#ifndef SCON_CHAR_INTERNAL_H
#define SCON_CHAR_INTERNAL_H 1

typedef enum _scon_char_flags
{
    _SCON_CHAR_LETTER = (1 << 0),
    _SCON_CHAR_DIGIT = (1 << 1),
    _SCON_CHAR_SYMBOL = (1 << 2),
    _SCON_CHAR_WHITESPACE = (1 << 3),
} _scon_char_flags_t;

typedef struct _scon_char_info
{
    _scon_char_flags_t flags;
    char lower;
    char decodeEscape;
    char encodeEscape;
    unsigned char integer;
} _scon_char_info_t;

static const _scon_char_info_t _sconCharTable[256] = {
    ['\a'] = {0, '\a', 0, 'a'},
    ['\b'] = {0, '\b', 0, 'b'},
    ['\033'] = {0, '\033', 0, 'e'},
    ['\f'] = {0, '\f', 0, 'f'},
    ['\v'] = {0, '\v', 0, 'v'},
    ['\\'] = {0, '\\', '\\', '\\'},
    ['\''] = {0, '\'', '\'', '\''},
    ['"'] = {0, '"', '"', '"'},
    [' '] = {_SCON_CHAR_WHITESPACE, ' '},
    ['\t'] = {_SCON_CHAR_WHITESPACE, '\t', 0, 't'},
    ['\n'] = {_SCON_CHAR_WHITESPACE, '\n', 0, 'n'},
    ['\r'] = {_SCON_CHAR_WHITESPACE, '\r', 0, 'r'},
    ['!'] = {_SCON_CHAR_SYMBOL, '!'},
    ['$'] = {_SCON_CHAR_SYMBOL, '$'},
    ['%'] = {_SCON_CHAR_SYMBOL, '%'},
    ['&'] = {_SCON_CHAR_SYMBOL, '&'},
    ['*'] = {_SCON_CHAR_SYMBOL, '*'},
    ['+'] = {_SCON_CHAR_SYMBOL, '+'},
    ['-'] = {_SCON_CHAR_SYMBOL, '-'},
    ['.'] = {_SCON_CHAR_SYMBOL, '.'},
    ['/'] = {_SCON_CHAR_SYMBOL, '/'},
    [':'] = {_SCON_CHAR_SYMBOL, ':'},
    ['<'] = {_SCON_CHAR_SYMBOL, '<'},
    ['='] = {_SCON_CHAR_SYMBOL, '='},
    ['>'] = {_SCON_CHAR_SYMBOL, '>'},
    ['?'] = {_SCON_CHAR_SYMBOL, '?', '\?', '?'},
    ['@'] = {_SCON_CHAR_SYMBOL, '@'},
    ['^'] = {_SCON_CHAR_SYMBOL, '^'},
    ['_'] = {_SCON_CHAR_SYMBOL, '_'},
    ['|'] = {_SCON_CHAR_SYMBOL, '|'},
    ['~'] = {_SCON_CHAR_SYMBOL, '~'},
    ['{'] = {_SCON_CHAR_SYMBOL, '{'},
    ['}'] = {_SCON_CHAR_SYMBOL, '}'},
    ['['] = {_SCON_CHAR_SYMBOL, '['},
    [']'] = {_SCON_CHAR_SYMBOL, ']'},
    ['#'] = {_SCON_CHAR_SYMBOL, '#'},
    ['0'] = {_SCON_CHAR_DIGIT, '0', 0, 0, 0},
    ['1'] = {_SCON_CHAR_DIGIT, '1', 0, 0, 1},
    ['2'] = {_SCON_CHAR_DIGIT, '2', 0, 0, 2},
    ['3'] = {_SCON_CHAR_DIGIT, '3', 0, 0, 3},
    ['4'] = {_SCON_CHAR_DIGIT, '4', 0, 0, 4},
    ['5'] = {_SCON_CHAR_DIGIT, '5', 0, 0, 5},
    ['6'] = {_SCON_CHAR_DIGIT, '6', 0, 0, 6},
    ['7'] = {_SCON_CHAR_DIGIT, '7', 0, 0, 7},
    ['8'] = {_SCON_CHAR_DIGIT, '8', 0, 0, 8},
    ['9'] = {_SCON_CHAR_DIGIT, '9', 0, 0, 9},
    ['A'] = {_SCON_CHAR_LETTER, 'a', 0, 0, 10},
    ['B'] = {_SCON_CHAR_LETTER, 'b', 0, 0, 11},
    ['C'] = {_SCON_CHAR_LETTER, 'c', 0, 0, 12},
    ['D'] = {_SCON_CHAR_LETTER, 'd', 0, 0, 13},
    ['E'] = {_SCON_CHAR_LETTER, 'e', 0, 0, 14},
    ['F'] = {_SCON_CHAR_LETTER, 'f', 0, 0, 15},
    ['G'] = {_SCON_CHAR_LETTER, 'g'},
    ['H'] = {_SCON_CHAR_LETTER, 'h'},
    ['I'] = {_SCON_CHAR_LETTER, 'i'},
    ['J'] = {_SCON_CHAR_LETTER, 'j'},
    ['K'] = {_SCON_CHAR_LETTER, 'k'},
    ['L'] = {_SCON_CHAR_LETTER, 'l'},
    ['M'] = {_SCON_CHAR_LETTER, 'm'},
    ['N'] = {_SCON_CHAR_LETTER, 'n'},
    ['O'] = {_SCON_CHAR_LETTER, 'o'},
    ['P'] = {_SCON_CHAR_LETTER, 'p'},
    ['Q'] = {_SCON_CHAR_LETTER, 'q'},
    ['R'] = {_SCON_CHAR_LETTER, 'r'},
    ['S'] = {_SCON_CHAR_LETTER, 's'},
    ['T'] = {_SCON_CHAR_LETTER, 't'},
    ['U'] = {_SCON_CHAR_LETTER, 'u'},
    ['V'] = {_SCON_CHAR_LETTER, 'v'},
    ['W'] = {_SCON_CHAR_LETTER, 'w'},
    ['X'] = {_SCON_CHAR_LETTER, 'x'},
    ['Y'] = {_SCON_CHAR_LETTER, 'y'},
    ['Z'] = {_SCON_CHAR_LETTER, 'z'},
    ['a'] = {_SCON_CHAR_LETTER, 'a', '\a', 0, 10},
    ['b'] = {_SCON_CHAR_LETTER, 'b', '\b', 0, 11},
    ['c'] = {_SCON_CHAR_LETTER, 'c', 0, 0, 12},
    ['d'] = {_SCON_CHAR_LETTER, 'd', 0, 0, 13},
    ['e'] = {_SCON_CHAR_LETTER, 'e', '\033', 0, 14},
    ['f'] = {_SCON_CHAR_LETTER, 'f', '\f', 0, 15},
    ['g'] = {_SCON_CHAR_LETTER, 'g'},
    ['h'] = {_SCON_CHAR_LETTER, 'h'},
    ['i'] = {_SCON_CHAR_LETTER, 'i'},
    ['j'] = {_SCON_CHAR_LETTER, 'j'},
    ['k'] = {_SCON_CHAR_LETTER, 'k'},
    ['l'] = {_SCON_CHAR_LETTER, 'l'},
    ['m'] = {_SCON_CHAR_LETTER, 'm'},
    ['n'] = {_SCON_CHAR_LETTER, 'n', '\n'},
    ['o'] = {_SCON_CHAR_LETTER, 'o'},
    ['p'] = {_SCON_CHAR_LETTER, 'p'},
    ['q'] = {_SCON_CHAR_LETTER, 'q'},
    ['r'] = {_SCON_CHAR_LETTER, 'r', '\r'},
    ['s'] = {_SCON_CHAR_LETTER, 's'},
    ['t'] = {_SCON_CHAR_LETTER, 't', '\t'},
    ['u'] = {_SCON_CHAR_LETTER, 'u'},
    ['v'] = {_SCON_CHAR_LETTER, 'v', '\v'},
    ['w'] = {_SCON_CHAR_LETTER, 'w'},
    ['x'] = {_SCON_CHAR_LETTER, 'x'},
    ['y'] = {_SCON_CHAR_LETTER, 'y'},
    ['z'] = {_SCON_CHAR_LETTER, 'z'},
};

#define _SCON_CHAR_FLAGS(_c) (_sconCharTable[(unsigned char)(_c)].flags)
#define _SCON_CHAR_TO_LOWER(_c) (_sconCharTable[(unsigned char)(_c)].lower)
#define _SCON_CHAR_IS_WHITESPACE(_c) (_SCON_CHAR_FLAGS(_c) & _SCON_CHAR_WHITESPACE)
#define _SCON_CHAR_IS_LETTER(_c) (_SCON_CHAR_FLAGS(_c) & _SCON_CHAR_LETTER)
#define _SCON_CHAR_IS_DIGIT(_c) (_SCON_CHAR_FLAGS(_c) & _SCON_CHAR_DIGIT)
#define _SCON_CHAR_IS_SYMBOL(_c) (_SCON_CHAR_FLAGS(_c) & _SCON_CHAR_SYMBOL)

#endif