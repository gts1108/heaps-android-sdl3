#include <AL/al.h>

typedef void (*foldback_cb)(int, int);

// fallbacks lol
void alRequestFoldbackStart(ALenum format, ALsizei count, ALsizei size, ALint *mem, foldback_cb cb) {
    (void)format; (void)count; (void)size; (void)mem; (void)cb;
}

void alRequestFoldbackStop(void) {}