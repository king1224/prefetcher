#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <xmmintrin.h>

#define TEST_W 4096
#define TEST_H 4096

#include "impl.c"

typedef struct matrix Mat;
typedef void (*func_t)(int *src, int *dst, int w, int h);

struct matrix {
    int row,col;
    int *data;
    func_t transpose;
};

void init(Mat **M)
{
    if((*M = malloc(sizeof(Mat))) == NULL) {
        printf("Error malloc object Mat.\n");
        exit(0);
    }
    (*M)->row = (*M)->col = 0;
    (*M)->data = NULL;
#ifdef NAIVE
    (*M)->transpose = naive_transpose;
#endif
#ifdef SSE
    (*M)->transpose = sse_transpose;
#endif
#ifdef SSEPREFETCH
    (*M)->transpose = sse_prefetch_transpose;
#endif
#ifdef AVX
    (*M)->transpose = AVX_transpose;
#endif
#ifdef AVXPREFETCH
    (*M)->transpose = AVX_prefetch_transpose;
#endif

}

static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

int main()
{
    Mat *matrix_impl;
    init(&matrix_impl);
    /* verify the result of 64x64 matrix */
    {
        int pos = 0;
        int testin[4096];
        int testout[4096];
        int expected[4096];
        for(int i = 0 ; i < 4096 ; ++i) testin[i]=i;

        for(int i = 0 ; i < 4096 ; ++i) {
            expected[pos % 4096]=i;
            if(i % 64 != 63)
                pos += 64;
            else
                pos += 65;
        }

        matrix_impl -> transpose(testin, testout, 64, 64);
        assert(0 == memcmp(testout, expected, 4096 * sizeof(int)) &&
               "Verification fails");
    }

    {
        struct timespec start, end;
        int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

        srand(time(NULL));
        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();

        clock_gettime(CLOCK_REALTIME, &start);
        matrix_impl -> transpose(src, out, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("prefetch: \t %ld us\n", diff_in_us(start, end));

        free(src);
        free(out);
    }

    return 0;
}
