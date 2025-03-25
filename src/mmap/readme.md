# File Copy and Append Utility

This program copies the contents of one file to another, with optional flags for appending or adding a newline before appending the contents.

## Workflow

The program performs the following steps:

1. **Open Source File**: The source file (`file1.txt`) is opened in read-only mode.
2. **Get File Size**: The size of the source file is determined to ensure the entire file is read.
3. **Memory Mapping**: The source file is memory-mapped to allow direct access to its contents.
4. **Open Destination File**: The destination file (`file2.txt`) is opened with the specified flags:
   - If the `append` flag is set, the file is opened for appending.
   - If the `append` flag is not set, the file is opened in truncate mode (i.e., overwriting the existing contents).
5. **Newline Insertion**: If the `newLine` flag is set and the destination file already has content, a newline is written before appending the new content.
6. **Write Content**: The contents of the source file are written to the destination file.
7. **Cleanup**: The memory-mapped source file is unmapped and both the source and destination file descriptors are closed.

## Flags

The program accepts the following flags via command-line arguments:

1. `append`: If this flag is set to `true`, the contents of the source file will be appended to the destination file.
   - Default: `false` (the destination file will be overwritten).

2. `newLine`: If this flag is set to `true` and the `append` flag is enabled, a newline character will be inserted before appending the new content if the destination file is not empty.
   - Default: `false`.

## How to Run

### Compile the Program

1. **Ensure you have a C++ compiler** (e.g., `g++`).

2. **Compile the program** using the following command:

   ```bash
   g++ -o file_copy_append file_copy_append.cpp


Example Usage
Default behavior (overwrite destination file):

bash
Copy
./file_copy_append
This will copy the content from file1.txt to file2.txt and overwrite file2.txt.

Append content to the destination file:

bash
Copy
./file_copy_append true
This will append the content of file1.txt to file2.txt.

Append content with a newline before it:

bash
Copy
./file_copy_append true true
This will append the content of file1.txt to file2.txt and insert a newline before appending if file2.txt has existing content.


Here is an explanation of the flags used with the system calls `fstat`, `mmap`, `munmap`, and `lseek`, which are used in your code.

### 1. **`fstat` Flags**

`fstat` is used to retrieve information about a file, such as its size, permissions, and file type. The `fstat` function itself does not use flags directly, but it populates a `struct stat` with file information. Here's how it works:

```cpp
struct stat st;
fstat(src_fd, &st);
```

- **`src_fd`**: This is the file descriptor obtained from opening the file.
- **`&st`**: This is a pointer to a `struct stat` where the file information is stored.

The `struct stat` has the following relevant fields:
- **`st_size`**: The size of the file in bytes.
- **`st_mode`**: The file mode (file type and permissions).
- **`st_uid`, `st_gid`**: The user ID and group ID of the file's owner.
- **`st_atime`, `st_mtime`, `st_ctime`**: The access, modification, and status change times.

### 2. **`mmap` Flags**

`mmap` is used to map a file or device into memory. The flags control the access and behavior of the memory mapping.

```cpp
void *src_ptr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
```

Here are the flags used:

- **`PROT_READ`**: Specifies that the mapped memory region should be readable.
  - Other possible protections include:
    - `PROT_WRITE`: Allows writing to the mapped memory.
    - `PROT_EXEC`: Allows execution of instructions in the mapped memory.
    - `PROT_NONE`: Disallows access to the mapped region.

- **`MAP_PRIVATE`**: Creates a private copy of the memory-mapped file. Changes made to the mapped region will not be written back to the file, and the original file remains unchanged. This is useful for reading the file without modifying it.
  - Other options:
    - `MAP_SHARED`: Changes made to the mapped memory are reflected in the file.

- **`src_fd`**: The file descriptor of the source file, which is mapped into memory.
- **`0`**: The offset within the file to start mapping from (usually 0 if you're mapping from the beginning of the file).

### 3. **`munmap` Flags**

`munmap` is used to unmap a previously mapped memory region. It does not take flags, but its parameters control the behavior:

```cpp
munmap(src_ptr, file_size);
```

- **`src_ptr`**: The pointer to the start of the mapped memory region, which is returned by `mmap`.
- **`file_size`**: The size of the mapped memory region that is being unmapped.

There are no flags in `munmap`, but it ensures that the memory region is released.

### 4. **`lseek` Flags**

`lseek` is used to reposition the offset of the file descriptor. It allows you to set the current position in the file (i.e., where the next read or write will occur). The flags control how the offset is calculated:

```cpp
off_t current_offset = lseek(dest_fd, 0, SEEK_END);
```

Here are the flags used:

- **`SEEK_END`**: Sets the offset relative to the end of the file. The new offset will be the current size of the file, which is useful to determine the current end of the file for appending content.
  - Other options for `whence`:
    - `SEEK_SET`: Sets the offset relative to the beginning of the file (i.e., the offset is directly the `offset` parameter).
    - `SEEK_CUR`: Sets the offset relative to the current offset (i.e., adds `offset` to the current file offset).

- **`dest_fd`**: The file descriptor of the destination file.
- **`0`**: The offset argument, which is `0` when we want to set the position to the end of the file (since we're using `SEEK_END`).

### Summary of Flags

1. **`fstat`**: No specific flags for `fstat`, but it populates a `struct stat` with file metadata (e.g., size, permissions).
   
2. **`mmap`**:
   - **`PROT_READ`**: Read access to the mapped memory.
   - **`MAP_PRIVATE`**: Private memory mapping, changes are not reflected back to the file.

3. **`munmap`**: No flags, just unmaps a memory region.

4. **`lseek`**:
   - **`SEEK_END`**: Sets the file offset to the end of the file.
   - **`SEEK_SET`**, **`SEEK_CUR`**: Other options to reposition the file offset.

These system calls and their respective flags are crucial for efficiently handling files at a low level, particularly when working with large files or performing complex file manipulations like memory-mapping and file offset manipulation.



1 first we need the file discriptor to get the size of the file 

int fd = open(filePath, mode);
int fd = open(filePath, O_RDONLY) ; 
int fd = open(filePath, O_WRONLY) ; 
int fd = open(filePath, O_RDWR) ; 

struct stat sb;

A file table entry tracks the current offset referenced by all read or write operations to the file and the open mode (O_RDONLY, O_WRONLY, or O_RDWR) of the file.


2 use mmap to read the data 
char* file_in_memory = mmap(NULL , sb.size(), PROT_READ, MAP_PRIVATE,fd,0)

PROT_READ for
PROT_WRITE for


MAP_PRIVETE for 
MAP_SHARE for
0 from bigining of the file


printf(Print the file )
for(int i=0; i<sb.size; i++){
    file_in_memory = "a";
    printf("%c" , file_in_memory[i]);
}


munmap(file_in_memory, sb,st_size);
close(fd)