#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <gmp.h>
#include <pthread.h>

#define UNUSED __attribute__((unused))
#define true 1
#define false 0
#define BUFFER_SIZE 1024
#define MAX_THREADS 8

struct ThreadData {
    mpz_t n;
    mpz_t p;
    mpz_t q;
};

/* function prototypes */
void* calculate_factors(void* arg);

/**
 * @brief entry point for the program
 *
 * @param argc
 * @param argv
 *
 * @return int
 */
int main(int argc, char **argv)
{
    int fd;
    size_t line_buffer_size = 0;
    char *line_buffer = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        free(line_buffer);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        free(line_buffer);
        return 1;
    }

    FILE *fileStream = fdopen(fd, "r");
    if (fileStream == NULL) {
        close(fd);
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[MAX_THREADS];
    int threadCount = 0;
    struct ThreadData* threadData = NULL;

    while (true)
    {
        if (getline(&line_buffer, &line_buffer_size, fileStream) < 0) {
            break;
        }

        threadData = (struct ThreadData*)malloc(sizeof(struct ThreadData));
        mpz_init(threadData->n);
        mpz_init(threadData->p);
        mpz_init(threadData->q);
        mpz_set_str(threadData->n, line_buffer, 10);

        pthread_create(&threads[threadCount], NULL, calculate_factors, (void*)threadData);
        threadCount++;

        // Create additional threads if necessary
        if (threadCount >= MAX_THREADS) {
            for (int i = 0; i < threadCount; i++) {
                pthread_join(threads[i], NULL);
            }
            threadCount = 0;
        }
    }

    // Wait for remaining threads to finish
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(fileStream);
    close(fd);
    free(line_buffer);
    return (0);
}


/**
 * calculate_factors - calculates the factors of n
 *
 * @param arg: pointer to a struct ThreadData
 */
void* calculate_factors(void* arg)
{
    struct ThreadData* threadData = (struct ThreadData*)arg;
    mpz_t remainder, sqrt_n;
    mpz_init(remainder);
    mpz_init(sqrt_n);

    mpz_set_ui(threadData->p, 2);
    mpz_fdiv_r(remainder, threadData->n, threadData->p);
    if (mpz_cmp_ui(remainder, 0) == 0)
    {
        mpz_fdiv_q(threadData->q, threadData->n, threadData->p);
        gmp_printf("%Zd=%Zd*%Zd\n", threadData->n, threadData->q, threadData->p);
        mpz_clear(remainder);
        mpz_clear(sqrt_n);
        mpz_clear(threadData->n);
        mpz_clear(threadData->p);
        mpz_clear(threadData->q);
        free(threadData);
        pthread_exit(NULL);
    }

    mpz_sqrt(sqrt_n, threadData->n);
    for (mpz_set_ui(threadData->p, 3); mpz_cmp(threadData->p, sqrt_n) <= 0; mpz_add_ui(threadData->p, threadData->p, 2))
    {
        mpz_fdiv_r(remainder, threadData->n, threadData->p);
        if (mpz_cmp_ui(remainder, 0) == 0)
        {
            mpz_fdiv_q(threadData->q, threadData->n, threadData->p);
            gmp_printf("%Zd=%Zd*%Zd\n", threadData->n, threadData->q, threadData->p);
            break;
        }
    }

    mpz_clear(remainder);
    mpz_clear(sqrt_n);
    mpz_clear(threadData->n);
    mpz_clear(threadData->p);
    mpz_clear(threadData->q);
    free(threadData);

    pthread_exit(NULL);
}
