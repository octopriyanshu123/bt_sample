#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    const char* src_filename = "file1.txt";
    const char* dest_filename = "file2.txt";

    // Open source file
    int src_fd = open(src_filename, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return 1;
    }

    // Get the size of the source file
    struct stat st;
    if (fstat(src_fd, &st) == -1) {
        perror("Error getting file size");
        close(src_fd);
        return 1;
    }
    size_t file_size = st.st_size;

    // Memory map the source file
    void* src_ptr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_ptr == MAP_FAILED) {
        perror("Error mapping source file to memory");
        close(src_fd);
        return 1;
    }

    // Open destination file (create it if it doesn't exist)
    int dest_fd = open(dest_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        munmap(src_ptr, file_size); // Unmap memory before returning
        close(src_fd);
        return 1;
    }

    // Write the content from the source file to the destination file
    if (write(dest_fd, src_ptr, file_size) == -1) {
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

    std::cout << "File content copied successfully from " << src_filename << " to " << dest_filename << std::endl;
    return 0;
}


#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    const char* src_filename = "file1.txt"; // Source file
    const char* dest_filename = "file2.txt"; // Destination file

    // Open source file
    int src_fd = open(src_filename, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return 1;
    }

    // Get the size of the source file
    struct stat st;
    if (fstat(src_fd, &st) == -1) {
        perror("Error getting file size");
        close(src_fd);
        return 1;
    }
    size_t file_size = st.st_size;

    // Memory map the source file
    void* src_ptr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_ptr == MAP_FAILED) {
        perror("Error mapping source file to memory");
        close(src_fd);
        return 1;
    }

    // Open destination file (create it if it doesn't exist, open it for appending)
    int dest_fd = open(dest_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        munmap(src_ptr, file_size); // Unmap memory before returning
        close(src_fd);
        return 1;
    }

    // Check if the destination file has content. If so, append a newline before the new content.
    
    off_t current_offset = lseek(dest_fd, 0, SEEK_END); // Get current end of file offset
    if (current_offset > 0) {
        // If file has content, add a newline before appending new content.
        const char* newline = "\n";
        if (write(dest_fd, newline, 1) == -1) {
            perror("Error writing newline to destination file");
            munmap(src_ptr, file_size); // Unmap memory before returning
            close(src_fd);
            close(dest_fd);
            return 1;
        }
    }

    // Write the content from the source file to the destination file
    if (write(dest_fd, src_ptr, file_size) == -1) {
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
