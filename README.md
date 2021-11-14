# Memory Allocators: a group assignment

## Submitting your assignment 

Submit your Github repository directly or a ZIP file of your Github repository to Gradescope **Assignment 4**.  **Only one person needs to submit, but they MUST add their teammate when they submit.** Do not wait until the last minute to test how to properly submit your assignment. In fact, **try a submission as soon as you accept the assignment on GitHub Classroom.** 


## Assignment Strategy

As with most assignments, it can be useful to compile, save, and test **often**. If you get stuck, try making small .c programs and running experiments and testing your assumptions. Use `assert` statements to check that your assumptions hold and to make programs fail early with useful information if they don't.

# Introduction - Memory Allocator 1

For this assignment, you and your partner will be writing your own memory allocator.  Writing a custom memory allocator is something you might do, especially if you work on any performance sensitive system (games, graphics, quantitative finance, embedded devices or any application you want to run fast!). [Malloc](https://linux.die.net/man/3/malloc) and [Free](https://linux.die.net/man/3/free) are general purpose functions written to manage memory in the average use case quite well, but they can always be optimized for a given workload.  That said, a lot of smart people have worked on making malloc/free quite performant over a wide range of workloads.  Optimization aside, you might write an allocator to add in cool debugging features, and swap it in as needed.

For this assignment, you will implement a portion of a custom memory allocator for the C language. You will write your own versions of:

- [malloc](https://linux.die.net/man/3/malloc)
- [calloc](https://linux.die.net/man/3/calloc)
- [free](https://linux.die.net/man/3/free)

## Memory Allocator 1

This assignment will be the first of two memory allocators you will create this term.

## Design Decisions

Your objective will be to create three functions in [mymalloc.c](./mymalloc.c)

1. `mymalloc`
2. `mycalloc`
3. `myfree`

Please read through the following design decisions to help guide you. This is some of the thought process a designer of such a system may go through. There are more concrete specifications following.

### Decision 1 - How to request memory?

Remember that malloc, calloc, and free are all working with memory allocated on the heap. We can request memory from the operating system using a system call such as [sbrk](https://linux.die.net/man/2/sbrk). There exist other ways to request memory, such as [mmap](https://linux.die.net/man/3/mmap), which you  will  use for your next memory allocator assignment.
(void*)((char*)next_block + sizeof(struct block_desc));
For this assignment, ***all memory requets will use the `sbrk` system call.***

### Decision 2 - How to organize our memory?

Once you have retrieved memory, we need to keep track of it. That means that everytime a user uses your 'malloc' or 'calloc' functions, you will want to know where that memory exists and how big it is. Thus, we want to keep track of all of the memory we request in a convenient data structure that can dynamically expand.

So think about: *What data structure could I use?*

### Decision 3 - What else will I need?

You may define any helping data structures and functions that assist you in this task. This means you might even have a global variable or two to assist with your implementation. Depending on what data structure you decide to store all of the requested memory in, it may be useful to have additional helper functions for traversing your data structure and finding free blocks/requesting blocks for example.

### Decision 4 - How will I efficiently reuse memory already allocated on the heap?

Programs may frequently allocate and then free memory throughout the program's execution. It can thus become very inefficient to keep expanding the heap segment of our process. Instead, we try to reuse blocks as efficiently as possible. That is, if I have allocated a block of memory of 8 bytes, and that 8 bytes gets freed, the next time I call malloc I can use the previous 8 bytes without having to make another call to `sbrk`. There exist at least two relatively straightforward algorithms for allocating memory, Best-Fit and First-Fit.  Both are described in detail, along with other very useful supporting information in [OSTEP Chapter 17](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-freespace.pdf):

1. Best-fit - This tries to find an 'optimal size' of a previously freed block to place memory in. 
2. First-fit - This finds the first block of memory that will fit the request. 

Each strategy has a trade-off of performance vs fragmentation. For example, it may take longer to find an optimal sized block, but that will likely lead to less fragmentation. 

For this assignment, a best-fit allocator can earn you all of the marks, while a first-fit allocator can earn you *almost* all of the marks.

## Concrete Specification for your allocator.

Here are the default specifications to put everyone on equal footing. You are welcome to diverge if you think you can build something more optimal (*document this both in the code and in [group-info.md](./group-info.md)*), but get this basic allocator with the specifications below to work first!

1. Do not modify [malloc.h](./malloc.h).
2. Use a linked list data structure. See [OSTEP Chapter 17](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-freespace.pdf).
	- This data structure will keep track of the blocks that you have allocated within your `malloc` function. 
	- You should have a global variable that serves as the "head" or "first block of memory" in your linked list.
	- You should have some notion of a 'block' of memory in your program.
	- An example is provided here with some fields that may be useful:
    ```c
    typedef struct block {
      size_t size;        // How many bytes beyond this block have been allocated in the heap
      struct block *next; // Where is the next block in your linked list
      int free;           // Is this memory free, i.e., available to give away?
      int debug;          // (optional) Perhaps you can embed other information--remember,
                          // you are the boss!
    } block_t;
    ```
2. You will want to keep track of how big this block structure is. A little trick is to use the preprocessor so you can simply use BLOCK_SIZE in your code.
   ```c
   #define BLOCK_SIZE sizeof(block_t)
   ```
3. Use the [sbrk](https://linux.die.net/man/2/sbrk) system call. Your version of `malloc` (`mymalloc` or its helper functions) will use `sbrk`.  Understand what `sbrk(0)` and `sbrk(10)` do before you start. 
4. Your free function sets a block of memory to be free, bye setting `free` in `block_t` to non-zero. Consider how memory is laid out in the heap  t make sure you are only accessing your struct. Here is a simple diagram: 
  ```
  |block|----actual memory----|block|--------actual memory-------|block|--actual memory--|

  ^ Here is where your struct lives, this is what you want to update.
  ```
5. Your malloc function remember is returning the actual memory
  ```
  |block|----actual memory----|block|--------actual memory-------|block|--actual memory--|
  
         ^ Here is what you'll return the the programmer as their memory.
  ```
6. For full points, your `calloc` function (`mycalloc`) should make a call to your `malloc` function (`mymalloc`). 
	- Remember what is different between `calloc` and `malloc`.
7. Allocators
	- A first-fit memory allocator looks for the first block available in the linked list of memory. Remind yourself what the trade-off is between the other allocators (e.g. compare to a 'best-fit' allocator).
	- A best-fit allocator (which will earn full marks on this assignment) scans through the entire list and finds the optimal position to minimize external fragmentation.
		- You may have a heuristic that selects blocks that are up to 4 bytes bigger than the request.
		- Otherwise, if there is no optimal block, you must request more memory with `sbrk`.
8. Your malloc function(`mymalloc`) must print out "Malloc %zu bytes\n" using the provided `debug_printf` function (which is used just like `printf`; see [debug.h](debug.h))
9. Your calloc function(`mycalloc`) must print out "Calloc %zu bytes\n" using the provided `debug_printf` function (Yes, a proper implementation will print "Malloc ..." followed by "Calloc ...")
10. Your free function (`myfree`) must print out "Freed %zu bytes\n" (using `debug_printf`)
11. With these print outs, you can see if they match the original programs.
12. We will examine your code to confirm you do not use the C library `malloc`/`calloc`/`free` from `stdlib.h`. You should only be using syscalls such as `sbrk` to request memory from the operating system.

## How to test the assignment

We have included a Makefile to make your life easier. Here's a quick overview of the possible targets:

- `make` - compile mymalloc.c to object file, `mymalloc.o`
- `make test` - compile and run tests in the tests directory with `mymalloc`.
- `make demo` - compile and run tests in the tests directory with standard `malloc`.
- `make help` - print available targets


The tests in the tests directory are a subset of what the autograder tests.  Passing all the tests does not guarantee a perfect assignment, but it will give you some confidence your implementation is working.    It would be wise to write additional tests to exercise your implementation.


# Deliverables
1. [mymalloc.c](./mymalloc.c)
2. Update [group-info.md](./group-info.md). 

Remember to submit your Github repository directly or a ZIP file of your Github repository to Gradescope **Assignment 4**.  **Only one person needs to submit, but they MUST add their teammate when they submit.** Do not wait until the last minute to test how to properly submit your assignment. 

# Rubric

- 20% on good programming style
  - Basics: meaningful purpose statements; explanation of arguments and return values
  - Explicitly stated assumptions
  - Correct use of types (e.g., not assigning -1 to an unsigned)
  - Short, understandable functions (generally, < 50 lines) 
  - Consistent indentation and use of whitespace
  - Minimal use of global variables
  - Explanatory comments for complex blocks of code
- 80% for a working complete memory allocator (`mymalloc`/`mycalloc`/`myfree`)
  - 75% You implement a working *first-fit* allocator that does not leak memory.
  - 80% You implement a working *best-fit* allocator that does not leak memory.
  - (If you implement both, let us know which one to grade.)
 

    
# Resources to help
- Advanced Material: https://www.youtube.com/watch?v=kSWfushlvB8 ("How to Write a Custom Allocator" for C++)
- The book Programming in C (K&R book) has a nice description of memory allocators.

# F.A.Q.

- Q: Do I have to reduce the heap ever?
- A: No, you do not need to ever make calls to `sbrk(-10)` for example.
 
- Q: Can I use `valgrind` to check for memory leaks in my program?
- A: Likely not, because you are implementing your own allocator. By default, `valgrind` is not able to reliably track calls to `sbrk` (or `mmap`). This is not to say that using valgrind gives no information, but it won't give you the same level of detail as if the code had used the system allocator.  

- Q: So if I cannot use `valgrind`, what can I do?
- A1: One suggestion is to keep track of how many total bytes you allocate and how many you mark as free as global variables.
- A2: A second suggestion is that you could use some of gcc's compile-time interpositioning tricks to add a function that is automatically called at the end of the program. This function would traverse your linked list structure and check to see if any of the memory in your 'block' structure are marked as unfreed.

- Q: How will I know my tool is working?
- A1: This is a pretty general question, but a tool like `strace` can be helpful. `strace` can be run on the tests once you have compiled them. `strace` reports how many calls you have made to `brk` for example, and you can test your assumptions to see if that system call is being made too often (i.e. not reusing blocks that have been previously allocated and could be used).  `strace` will also confirm that you are NOT using the system malloc or free. 
- A2: You could also run your allocator with your linked list or other data structures and see if it passes the given unit tests.
- A3: Write more unit tests to exercise your implemetation, e.g., mallocs of random sizes, interleaved mallocs and frees, large numbers of mallocs and frees, walking the free list, edge cases...

# Going Further

- You do not have to align your memory, but you may choose to (we will only run test on nicely sized memory blocks).
- Blocks of memory typically are aligned. That is, they are aligned to some address boundary (every 8, 16, 32, 64, etc.). In order to make our memory allocator more simple, we can force alignment of every block. Here is a trick using a `union` from Cexerpt.pdf in Lab 8.

```c
typedef long Align; // Alignment to long size

union header{
  struct{
    union header *ptr;  // next block in free list
    unsigned size;      // Size of this block
  } s;
  Align x;  // Force alignment of blocks to our boundary.
            // Note this field is never used, but space is allocated for it.
            // Convince yourself this 'trick' works and why.
};
```


Other Notes:

* In order to avoid fragmentation, most allocators combine blocks that are adjacent(on both sides) into bigger ones. They may then split those bigger blocks as need as well. You could write some helper functions to assist with this, and have a more optimal memory allocator. 
	- **Splitting and coalescing of blocks is NOT required for this assignment.**
* The `malloc` implementation is actually open source, so you can take a look at it if you are curious.

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

Not applicable, as we will do more with allocators later!
#   c s 3 6 5 0 - a 4  
 