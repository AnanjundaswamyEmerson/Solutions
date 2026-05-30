// Sample FUSE implementation for Linux
#include <fuse.h>
#include <sys/stat.h>

class Fuse {
public:
    Fuse();
    void run();
    int do_open(const char* path, int flags);
    int do_read(const char* path, char* buf, size_t size, off_t offset);
    int do_write(const char* path, const char* buf, size_t size, off_t offset);
    int do_release(const char* path);
    size_t do_getattr(const char* path, struct stat* stbuf);

private:
    void init();
};
