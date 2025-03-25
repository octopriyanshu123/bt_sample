#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    const char *src_filename = "file1.txt";  // Source file
    const char *dest_filename = "file2.txt"; // Destination file
    bool newLine = false;
    bool append = false;

    if (argc >= 2 && std::string(argv[1]) == "true")
        append = true;

    if (argc == 3 && std::string(argv[2]) == "true")
        newLine = true;

    // Open source file
    int src_fd = open(src_filename, O_RDONLY);
    if (src_fd == -1)
    {
        perror("Error opening source file");
        return 1;
    }

    // Get the size of the source file
    struct stat st;
    if (fstat(src_fd, &st) == -1)
    {
        perror("Error getting file size");
        close(src_fd);
        return 1;
    }
    size_t file_size = st.st_size;

    // Memory map the source file
    void *src_ptr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_ptr == MAP_FAILED)
    {
        perror("Error mapping source file to memory");
        close(src_fd);
        return 1;
    }

    // Determine flags for destination file
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int dest_fd = open(dest_filename, flags, 0644);
    if (dest_fd == -1)
    {
        perror("Error opening destination file");
        munmap(src_ptr, file_size); // Unmap memory before returning
        close(src_fd);
        return 1;
    }

    // If appending, check if we need a newline
    if (append && newLine)
    {
        off_t current_offset = lseek(dest_fd, 0, SEEK_END); // Get current end of file offset
        if (current_offset > 0)
        {
            // If file has content, add a newline before appending new content.
            const char *newline = "\n";
            if (write(dest_fd, newline, 1) == -1)
            {
                perror("Error writing newline to destination file");
                munmap(src_ptr, file_size);
                close(src_fd);
                close(dest_fd);
                return 1;
            }
        }
    }

    // Write the content from the source file to the destination file
    if (write(dest_fd, src_ptr, file_size) == -1)
    {
        perror("Error writing to destination file");
        munmap(src_ptr, file_size); // Unmap memory before returning
        close(src_fd);
        close(dest_fd);
        return 1;
    }

    // Clean up
    munmap(src_ptr, file_size); // Unmap the source file
    close(src_fd);
    close(dest_fd);

    std::cout << "File content copied and appended successfully from " << src_filename << " to " << dest_filename << std::endl;
    return 0;
}
