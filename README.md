# Producer Consumer

Solving the Producer-Consumer problem using POSIX threads and a shared circular queue.

## Usage

Compile with `make`.

Run with 5 command-line arguments including ./main: queue_size number_of_jobs number_of_producers number_of_consumers.

e.g. for a queue size of 5 with 10 jobs to complete, 1 producer, and 2 consumers:
```
./main 5 10 1 1
```

NB: some utilities such as `semtimedop()` are not available on MacOS (see [link](https://stackoverflow.com/questions/1405132/unix-osx-version-of-semtimedop) for workaround)
