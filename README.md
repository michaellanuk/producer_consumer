# Producer Consumer

Solving the Producer-Consumer problem using POSIX threads and a shared circular queue.

## Usage

Compile with `make`

Run with 5 command-line arguments including ./main: queue_size number_of_jobs number_of_producers number_of_consumers

Example: `./main 5 10 2 2`

NB: some utilities such as `semtimedop()` are not available on MacOS (see [link](https://stackoverflow.com/questions/1405132/unix-osx-version-of-semtimedop) for workaround)
