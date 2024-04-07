#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define INPUT_MAXIMUM (100000000)

int input_size;
bool check_to_unlock;
char input[INPUT_MAXIMUM];
int alr_read = 0;
int alr_written = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;;
pthread_mutex_t mutex  = PTHREAD_MUTEX_INITIALIZER;

void *input_func(void *defau)
{
    while (true) {
        pthread_mutex_lock(&mutex);
        while (true) {
            if(input_size <= 0) break;
            pthread_cond_wait(&cond,
                              &mutex);
        }

        alr_read = read(0,
                        input,
                          INPUT_MAXIMUM);
        if (alr_read < 0) {
            exit(1);
        }

        input_size += alr_read;
        if (alr_read == 0) {
            input[alr_read] = '\0';
            check_to_unlock = 1;
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        bool check = check_to_unlock;
        if (check) pthread_exit(NULL);
    }
}

void *output_func(void *defau)
{
    while (true) {
        pthread_mutex_lock(&mutex);
        while (true) {
            if(input_size!=0) break;
            if (check_to_unlock) {
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&cond,
                              &mutex);
        }

        alr_written = write(1,
                              input,
                              input_size);
        if (alr_written == -1) exit(1);

        input_size -= alr_written;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        bool check = check_to_unlock;
        if (check) pthread_exit(NULL);
    }
}

int main()
{
    input_size = 0;
    check_to_unlock = 0;

    pthread_t thread_to_input, thread_to_output;

    pthread_create(&thread_to_input,
                   NULL,
                   input_func,
                   NULL);
    pthread_create(&thread_to_output,
                   NULL,
                   output_func,
                   NULL);
    pthread_join(thread_to_input,
                 NULL);
    pthread_join(thread_to_output,
                 NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}/*
 gcc -std=c99 -Werror=implicit-function-declaration -pthread biriukova.c -o biriukova
 ./biriukova < test.in.txt  > test.out.txt

 */