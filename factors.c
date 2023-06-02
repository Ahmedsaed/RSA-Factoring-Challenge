#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>

#define UNUSED __attribute__((unused))
#define true 1
#define false 0
#define BUFFER_SIZE 1024
#define MAX_THREADS 1
#define SIEVE_LIMIT 1000000

struct ThreadData {
    mpz_t n;
    mpz_t p;
    mpz_t q;
    int* primes;
};

/* function prototypes */
void* calculate_factors(void* arg);
void sieve_of_eratosthenes(int limit, int *primes);

int main(int argc, char **argv)
{
    size_t line_buffer_size = 0;
    char *line_buffer = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        free(line_buffer);
        return 1;
    }

    FILE *fileStream = fopen(argv[1], "r");
    if (fileStream == NULL) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    // Calculate the sieve of Eratosthenes
    int primes[SIEVE_LIMIT + 1];
    sieve_of_eratosthenes(SIEVE_LIMIT, primes);

    pthread_t threads[MAX_THREADS];
    int threadCount = 0;
    struct ThreadData* threadData = NULL;

    while (true)
    {
        if (getline(&line_buffer, &line_buffer_size, fileStream) < 0) {
            break;
        }

        // Skip empty lines
        if (strlen(line_buffer) == 0) {
            continue;
        }

        threadData = (struct ThreadData*)malloc(sizeof(struct ThreadData));
        mpz_init(threadData->n);
        mpz_init(threadData->p);
        mpz_init(threadData->q);
        threadData->primes = primes;
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
    free(line_buffer);
    return (0);
}

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

    // Check if the number is within the range of the sieve
    if (mpz_cmp_ui(sqrt_n, SIEVE_LIMIT) <= 0)
    {
        int* primes = threadData->primes;
        for (mpz_set_ui(threadData->p, 2); mpz_cmp(threadData->p, sqrt_n) <= 0; mpz_add_ui(threadData->p, threadData->p, 1))
        {
            if (primes[mpz_get_ui(threadData->p)] && mpz_cmp_ui(remainder, 0) == 0)
            {
                mpz_fdiv_q(threadData->q, threadData->n, threadData->p);
                gmp_printf("%Zd=%Zd*%Zd\n", threadData->n, threadData->q, threadData->p);
                break;
            }
            mpz_fdiv_r(remainder, threadData->n, threadData->p);
        }

        if (mpz_cmp_ui(remainder, 0) != 0)
        {
            mpz_set(threadData->q, threadData->n);
            mpz_set_ui(threadData->p, 1);
            gmp_printf("%Zd=%Zd*%Zd\n", threadData->n, threadData->q, threadData->p);
        }
    }
    else
    {
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

        if (mpz_cmp_ui(remainder, 0) != 0)
        {
            mpz_set(threadData->p, threadData->n);
            mpz_set_ui(threadData->q, 1);
            gmp_printf("%Zd=%Zd*%Zd\n", threadData->n, threadData->q, threadData->p);
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

void sieve_of_eratosthenes(int limit, int *primes)
{
    primes[0] = 0;
    primes[1] = 0;

    for (int p = 2; p * p <= limit; p++)
    {
        if (primes[p])
        {
            for (int i = p * p; i <= limit; i += p)
            {
                primes[i] = 0;
            }
        }
    }
}
