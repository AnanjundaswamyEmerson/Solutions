#define FUSE_USE_VERSION 31

#include "fuse.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fuse.h>
#include <fcntl.h>
#include <cerrno>

class NVMeDevice {
    public:
        static const std::string file_path;
        static const std::string real_path;

        static const std::string& get_file_name() {
            static const std::string name = "hello.txt";
            return name;
        }

        static const std::string& get_device_path() {
            // Replace with "/dev/nvme0n1" for real hardware, or "/dev/loop0" for safe testing
            static const std::string path = "/dev/loop36"; 
            return path;
        }


        int getattr(const std::string& path, struct stat* stbuf, struct fuse_file_info* fi) {
            std::memset(stbuf, 0, sizeof(struct stat));

            if (path == "/") {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;
            } else if (path == "/" + get_file_name()) {
                stbuf->st_mode = S_IFREG | 0666; // Read/Write permissions
                stbuf->st_nlink = 1;
                stbuf->st_size = 4096;           // Virtual size allocations
                return 0;
            }
            return -ENOENT;
        }

        int readdir(const std::string &path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, fuse_readdir_flags flags) {
            if (path != "/") return -ENOENT;
            filler(buffer, ".", nullptr, 0, FUSE_FILL_DIR_PLUS);
            filler(buffer, "..", nullptr, 0, FUSE_FILL_DIR_PLUS);
            filler(buffer, get_file_name().c_str(), nullptr, 0, FUSE_FILL_DIR_PLUS);
            return 0;
        }

        int open(const std::string& path, struct fuse_file_info* fi) {
            int fd = 0;
            // PRINT THE EXACT PATH RECEIVED
            std::cout << "[DEBUG OPEN] Kernel requested path: '" << path << "'\n";
            std::cout << "[DEBUG OPEN] Expected path: '/" << get_file_name() << "'\n";
            if (path == "/" || path != "/" + get_file_name()) {
                return -ENOENT; // Cannot open root directory
            }
            std::cout << "Opening file: " << get_device_path() << std::endl;
            fd = ::open(get_device_path().c_str(), O_RDWR); // Simulate file descriptor
            if (fd < 0) {
                int err = errno;
                std::cout << "Failed to open device: " << get_device_path() << ", error: " << std::strerror(err) << std::endl;
                std::cerr << "FUSE Error opening device: " << get_device_path() << ": " << std::strerror(err) << std::endl;
                return err;
            }
            fi->fh = fd;
            std::cout << "FUSE Successfully opened device: " << get_device_path();
            return 0;
        }

        int write(const std::string& path, const char* buf, ssize_t size, off_t offset, struct fuse_file_info* fi) {
            if (path != "/" + get_file_name()) return -ENOENT; // Cannot write to root directory

            int fd = static_cast<int>(fi->fh);

            ssize_t bytes_written = ::pwrite(fd, buf, size, offset);
            if (bytes_written < 0) {
                int err = errno;
                std::cerr << "FUSE Error writing to device: " << get_device_path() << ": " << std::strerror(err) << std::endl;
                return err;
            }
            std::cout << "FUSE Successfully wrote " << bytes_written << " bytes to device: " << get_device_path() << std::endl;
            return (static_cast<int>(bytes_written));
        }

        int read(const std::string& path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
            if (path != "/" + get_file_name()) return -ENOENT; // Cannot read from root directory

            int fd = static_cast<int>(fi->fh);

            std::cout << "Reading from device: " << get_device_path() << std::endl;
            ssize_t bytes_read = ::pread(fd, buf, size, offset);
            if (bytes_read < 0) {
                int err = errno;
                std::cerr << "FUSE Error reading from device: " << get_device_path() << ": " << std::strerror(err) << std::endl;
                return err;
            }
            std::cout << "FUSE Successfully read " << bytes_read << " bytes from device: " << get_device_path() << std::endl;
            return (static_cast<int>(bytes_read));
        }

        int release(const std::string& path, struct fuse_file_info* fi) {
            if (path != "/tmp" + get_file_name()) return -ENOENT; // Cannot release root directory

            int fd = static_cast<int>(fi->fh);
            ::close(fd);
            return 0;
        }


};

static NVMeDevice nvmdevice; // Static member to represent the NVMe device

static int fuse_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* fi) {
    return nvmdevice.getattr(path, stbuf, fi);
}

static int fuse_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, fuse_readdir_flags flags) {
    return nvmdevice.readdir(path, buffer, filler, offset, fi, flags);
}

static int fuse_open(const char* path, struct fuse_file_info* fi) {
    return nvmdevice.open(path, fi);
}

static int fuse_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    return nvmdevice.write(path, buf, size, offset, fi);
}

static int fuse_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    return nvmdevice.read(path, buf, size, offset, fi);
}

const std::string NVMeDevice::file_path = "/tmp/example.txt";
const std::string NVMeDevice::real_path = "/dev/loop0";

int main(int argc, char* argv[]) {
    static struct fuse_operations operations = {};
    operations.getattr = fuse_getattr;
    operations.readdir = fuse_readdir;
    operations.open = fuse_open;
    operations.write = fuse_write;
    operations.read = fuse_read;

    return fuse_main(argc, argv, &operations, nullptr);
}