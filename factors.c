#include "main.h"
#include <gmp.h>

void calculate_factors(mpz_t n, mpz_t p, mpz_t q);

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
    size_t line_length;
    char *line_buffer;

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return (1);
    }

    FILE *fileStream = fdopen(fd, "r");
    if (fileStream == NULL) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    mpz_t n, p, q;
    mpz_init(n);
    mpz_init(p);
    mpz_init(q);

    while (true)
    {
        if (getline(&line_buffer, &line_length, fileStream) < 0) {
            break;
        }

        mpz_set_str(n, line_buffer, 10);

        calculate_factors(n, p, q);

        gmp_printf("%Zd=%Zd*%Zd\n", n, q, p);
    }

    mpz_clear(n);
    mpz_clear(p);
    mpz_clear(q);

    close(fd);
    return (0);
}


/**
 * calculate_factors - calculates the factors of n
 *
 * @n: the number to calculate the factors of
 * @p: the pointer to the first factor
 * @q: the pointer to the second factor
 */
void calculate_factors(mpz_t n, mpz_t p, mpz_t q)
{
    mpz_t remainder, sqrt_n;
    mpz_init(remainder);
    mpz_init(sqrt_n);

    mpz_set_ui(p, 2);
    mpz_fdiv_r(remainder, n, p);
    if (mpz_cmp_ui(remainder, 0) == 0)
    {
        mpz_fdiv_q(q, n, p);
        return;
    }

    mpz_sqrt(sqrt_n, n);
    for (mpz_set_ui(p, 3); mpz_cmp(p, sqrt_n) <= 0; mpz_add_ui(p, p, 2))
    {
        mpz_fdiv_r(remainder, n, p);
        if (mpz_cmp_ui(remainder, 0) == 0)
        {
            mpz_fdiv_q(q, n, p);
            break;
        }
    }

    mpz_clear(remainder);
    mpz_clear(sqrt_n);
}
