## All #include statements used in the Network-Lab repository:

Here's a comprehensive list of all unique `#include` directives used across the C files:

### System/Standard Headers:
1. **`<stdio.h>`** - Standard input/output
2. **`<stdlib.h>`** - Standard library (memory allocation, exit, etc.)
3. **`<string.h>`** - String manipulation functions
4. **`<unistd.h>`** - POSIX API (close, read, write, etc.)

### Network Headers:
5. **`<sys/socket.h>`** - Socket API
6. **`<arpa/inet.h>`** - Internet address operations (inet_addr, htons, etc.)
7. **`<netinet/in.h>`** - Internet protocol definitions
8. **`<sys/types.h>`** - Data types used in system calls

### File Control Headers:
9. **`<fcntl.h>`** - File control options

### Advanced I/O Headers:
10. **`<sys/select.h>`** - Select mechanism for I/O multiplexing

**Summary:** The repository uses **10 unique header files**, primarily focused on network programming (sockets, TCP/UDP) and standard C library functions. These are typical headers for implementing network communication protocols in C.
