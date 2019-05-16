
/**
 * Build with -DSSE if SIMD extensions are available on target CPU.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

#ifdef SSE
    #include <immintrin.h>
    #include <emmintrin.h>
#endif

#define min(a, b) a > b ? b : a
#define nearmulof(a, b) (((b) + ((a) - 1)) & ~((a) - 1))

#define BUFFER_DEFAULT_SIZE 16

typedef struct {
    size_t len;
    char * alloc, * data;
} str_t;

str_t * strnws(size_t n) {
    str_t * self = malloc(sizeof(str_t));
    self->len = n;
    self->data = self->alloc = calloc(n + 1, 1);
    return self;
}

str_t * strn(void) {
    return strnws(BUFFER_DEFAULT_SIZE);
}

void strres(str_t * self, size_t n);

__asm(
    "strres: \n\
    pushq %rbp \n\
    movq %rdi, %rbp \n\
    pushq %rbx \n\
    leaq 511(%rsi), %rbx \n\
    andq $-512, %rbx \n\
    leaq 1(%rbx), %rsi \n\
    subq $8, %rsp \n\
    movq %rbx, (%rdi) \n\
    movq 8(%rdi), %rdi \n\
    call realloc \n\
    vmovq %rax, %xmm1 \n\
    vpunpcklqdq %xmm1, %xmm1, %xmm0 \n\
    vmovups %xmm0, 8(%rbp) \n\
    movb $0, (%rax,%rbx) \n\
    addq $8, %rsp \n\
    popq %rbx \n\
    popq %rbp \n\
    ret"
);

void strapp(str_t * self, char c) {
    size_t prev = strlen(self->data);
    size_t needed = 1 + prev;
    if (self->len > needed) {
        strncat(self->data, &c, 1);
        return;
    }
    strres(self, needed);
    strncat(self->data, &c, 1);
}

str_t * strsub(str_t * buf, size_t from, ssize_t to) {
    size_t n = to - from;
    str_t * self = strnws(n);
    memcpy(self->data, buf->data + from, n);
    return self;
}

inline void nread(int * n) {
    register char c = 0;
    while (c < 33)
        c = getc_unlocked(stdin);
    while (c > 32) {
        *n = *n * 10LL + (c - 48);
        c = getc_unlocked(stdin);
    }
}

str_t * ice(void) {
    int n, i = 0;
    nread(&n);
    str_t * out = strn();
    for (; i < n; i++) {
        char c;
        int l = 0, t = 0;
        while (isspace(c=getchar()));
        nread(&l);
        for (; t < l; t++)
            strapp(out, c == 'W' ? 1 : 2);
    }
    return out;
}

int trymatch(str_t * a, str_t * b) {
    int x = 0;
    #ifndef SSE
    int aW = 0, aT = 0, bW = 0, bT = 0;
    #else
    __m128i vec = _mm_set_epi32(0, 0, 0, 0);
    int * arr;
    #endif
    for (; x < a->len; x++) {
        #ifndef SSE
        if (a->data[x] == '0') aW++;
        else                   aT++;
        if (b->data[x] == '0') bW++;
        else                   bT++;
        #else
        vec = _mm_add_epi32(vec, _mm_set_epi32(
                                a->data[x] == 1,
                                a->data[x] != 1,
                                b->data[x] == 1,
                                b->data[x] != 1
                            ));
        #endif
    }
    #ifdef SSE
    arr = (int *) &vec;
    return arr[0] == arr[2] && arr[1] == arr[3];
    #else
    return aT == bT && aW == bW;
    #endif
}

inline void nwrite(unsigned int n) {
    char tab[12];
    int p = 0;
    while (n != 0)
        tab[p++] = (n % 10) + 48, n /= 10;
    while (p--)
        putc_unlocked(tab[p], stdout);
    exit(0);
}

void match(str_t * a, str_t * b, int len) {
    int ai, bi, al = strlen(a->data), bl = strlen(b->data);
    for (ai = 0; ai < al - len; ai++)
        for (bi = 0; bi < bl - len; bi++)
            if (__builtin_expect(trymatch(strsub(a, ai, ai + len), strsub(b, bi, bi + len)), 0))
                nwrite(len);
}

int main(void) {
    str_t * a = ice(), * b = ice();
    int maxlen = min(strlen(a->data), strlen(b->data));
    while (maxlen-- > 0)
        match(a, b, maxlen + 1);
    putchar('0');
}
