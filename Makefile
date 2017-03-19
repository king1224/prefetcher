CC ?= gcc
CFLAGS = -msse2 -mavx2 --std gnu99 -O0 -Wall -Wextra

EXEC = naive_transpose sse_transpose sse_prefetch_transpose avx_transpose avx_prefetch_transpose

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

SRCS_common = main.c

naive_transpose: $(SRCS_common)
	$(CC) $(CFLAGS) \
		-DNAIVE -o $@ $(SRCS_common)

sse_transpose: $(SRCS_common)
	$(CC) $(CFLAGS) \
		-DSSE -o $@ $(SRCS_common)

sse_prefetch_transpose: $(SRCS_common)
	$(CC) $(CFLAGS) \
		-DSSEPREFETCH -o $@ $(SRCS_common)

avx_transpose: $(SRCS_common)
	$(CC) $(CFLAGS) \
		-DAVX -o $@ $(SRCS_common)

avx_prefetch_transpose: $(SRCS_common)
	$(CC) $(CFLAGS) \
		-DAVXPREFETCH -o $@ $(SRCS_common)

cache-test: $(EXEC)
	perf stat -e cache-misses,cache-references,instructions,cycles ./naive_transpose
	perf stat -e cache-misses,cache-references,instructions,cycles ./sse_transpose
	perf stat -e cache-misses,cache-references,instructions,cycles ./sse_prefetch_transpose
	perf stat -e cache-misses,cache-references,instructions,cycles ./avx_transpose
	perf stat -e cache-misses,cache-references,instructions,cycles ./avx_prefetch_transpose

output.txt:
	ls

plot: output.txt
	gnuplot scripts/runtime.gp

clean:
	$(RM) $(EXEC) main
	touch main.c
