#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <android/log.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern int hl_game_main(int argc, char **argv);

static int pfd[2];

static void *log_thread(void *arg)
{
    (void)arg;
    char buf[1024];
    ssize_t n;
    while ((n = read(pfd[0], buf, sizeof(buf) - 1)) > 0) {
        buf[n] = 0;
        __android_log_write(ANDROID_LOG_INFO, "HL", buf);
    }
    return NULL;
}

// redirect stdout/stderr to logcat
static void redirect_stdio(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    if (pipe(pfd) != 0) return;
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);
    pthread_t t;
    pthread_create(&t, NULL, log_thread, NULL);
    pthread_detach(t);
}

int main(int argc, char *argv[])
{
    redirect_stdio();

    int ret = hl_game_main(argc, argv);
    SDL_Log("hl_game_main returned %d", ret);
    return ret;
}
