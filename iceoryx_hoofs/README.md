
# Eclipse iceoryx hoofs overview

The iceoryx hoofs (**H**andy **O**bjects **O**ptimised **F**or **S**afety) are our basic
building blocks - the foundation of iceoryx. There are a wide variety of building blocks
grouped together in categories or namespace, depending on where or how they are used.

## Categories

| Namespace                         | Short Description |
|:---------------------------------:|:------------------|
| [cxx](#cxx)                       | Since we are not allowed to use C++17 as well as the heap or exceptions we implemented constructs like `optional`, `expected` or `variant` so that we can be as modern as possible. Furthermore, you can find here constructs which are mentioned in the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) as well as STL re-implementations of container like `vector` which are relocatable an can be placed into the shared memory. |
| [concurrent](#concurrent)         | You should never use concurrent constructs like `mutex`, `semaphores`, `atomic`, etc. directly in our codebase. At the moment we still have some exceptions to this guideline but the idea is that all classes which are using them are stored under concurrent and have to undergo more tests then the usual non concurrent class. For instance we try to provide stress tests for them. This module provides classes like `fifo`, `smart_lock`, `sofi`, `trigger_queue` and much more. |
| [design_pattern](#design-pattern) | Certain code patterns which are repeating themselves all over the code are abstracted and stored in here. At the moment we only have the creation pattern which will be removed in a future release. |
| [error-handling](#error-handling) | The central error handler in iceoryx for cases when no sane further execution is possible, e.g. `nullptr` access. |
| [log](#log)                       | The logger used by iceoryx. |
| [posix_wrapper](#posix-wrapper)   | Posix constructs like shared memory, threads or semaphores are not used directly in our code base. We abstracted them so that they are following the [RAII (Resource Acquisition Is Initialization)](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) idiom and other good practices from the C++ community.   |
| [units](#units)                   | Time units for duration and string literals. |
| other                             | There are even more namespaces inside the iceoryx hoofs but they will either become obsolete in the future, will be integrated into existing names or are established as another namespace and documented here. We are unsure where they will end up in the future. |

## Structure

The following sections have a column labeled `internal` to indicate that the API
is not stable and can change anytime. You should never rely on it and there is no
support if it is used and breaks your code after an update.

The column `maybe obsolete` marks classes which can be removed anytime soon.

### CXX

This contains STL constructs which are not part of the C++14 standard as well as convenience
constructs like the `NewType`. Since the module re-implements some STL constructs,
the C++ STL coding guidelines are used for all files in this module, to help the user
to have a painless transition from the official STL types to ours.
The API should also be identical to the corresponding STL types but we have to make
exceptions here. For instance, we do not throw exceptions, try to avoid undefined behavior
and we do not use dynamic memory. In these cases we adjusted the API to our use case.

Most of the headers are providing some example code on how the
class should be used.

| class/file            | internal | description                                                                                                                                                                                                                           |
|:---------------------:|:--------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|`algorithm`            |          | Implements `min` and `max` for an arbitrary number of values of the same type. For instance `min(1,2,3,4,5);`                                                                                                                         |
|`attributes`           |          | C++17 and C++20 attributes are sometimes available through compiler extensions. The attribute macros defined in here (like `IOX_FALLTHROUGH`, `IOX_MAYBE_UNUSED` ... ) make sure that we are able to use them if the compiler supports it. |
|`convert`              |          | Converting a number into a string is easy, converting it back can be hard. You can use functions like `strtoll` but you still have to handle errors like under- and overflow, or converting invalid strings into number. Here we abstract all the error handling so that you can convert strings into numbers safely. |
|`DeadlineTimer`        |          | Polling based timer to check for an elapsed deadline.                                                                                                                                                                                 |
|`expected`             |          | Our base class used in error handling. Every function which can fail should return an expected. With this the user knows that this function can fail and that they have to do some kind of error handling. We got inspired by the [C++ expected proposal]( http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0323r7.html) and by the [rust error handling concept](https://doc.rust-lang.org/std/result/enum.Result.html).                                                                                                                                            |
|`filesystem`           |          | Implementation of C++17 filesystem features for instance `cxx::perms` to abstract file permissions                                                                                                                                    |
|`function`             |          | A stack-based `std::function` replacement based on `storable_function`                                                                                                                                                                |
|`function_ref`         |          | C++11 implementation of the next-gen C++ feature `std::function_ref` see [function_ref proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0792r2.html). It behaves like `std::function` but does not own the callable.|
|`functional_interface` |          | Constructs to easily add functional interfaces like `and_then` to object container.                                                                                                                                                   |
|`GenericRAII`          |          | This is an abstraction of the C++ RAII idiom. Sometimes you have constructs where you would like to perform a certain task on creation and then again when they are getting out of scope, this is where `GenericRAII` comes in. It is like a `std::lock_guard` or a `std::shared_ptr` but more generic. |
|`helplets`             |          | Implementations of [C++ Core Guideline](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) concepts like `not_null` are contained here. Additionally, we are providing some types to verify preconditions at compile time. Think of an int which has to be always greater 5, here we provide types like `greater_or_equal<int, 6>`.|
|`list`                 |          | Heap and exception free, relocatable implementation of `std::list`                                                                                                                                                                    |
|`NewType<T, Policies>` |          | C++11 implementation of [Haskells NewType-pattern](https://wiki.haskell.org/Newtype).                                                                                                                                                 |
|`optional`             |          | C++11 implementation of the C++17 feature `std::optional`                                                                                                                                                                             |
|`requires`             |          | Base for `Expects`/`Ensures` from the C++ Core Guideline                                                                                                                                                                              |
|`scoped_static`        |          | Helper function to limit lifetime of static or global variables to a scope                                                                                                                                                            |
|`serialization`        |          | Implements a simple serialization concept for classes based on the idea presented here [ISOCPP serialization](https://isocpp.org/wiki/faq/serialization#serialize-text-format).                                                       |
|`spinator`             | i        | Building block to realize busy waiting loops with low cpu load.                                                                                                                                                                       |
|`stack`                |          | Stack implementation with simple push/pop interface.                                                                                                                                                                                  |
|`static_storage`       | i        | Untyped aligned static storage.                                                                                                                                                                                                       |
|`storable_function`    | i        | A `std::function` alternative with configurable backend for memory storage.                                                                                                                                                           |
|`string`               |          | Heap and exception free implementation of `std::string`. Attention, since the string is stack based, std::string or char array which are assigned to this string will be truncated and zero-terminated if they exceed the string capacity. |
|`type_traits`          |          | Extended support for evaluating types on compile-time.                                                                                                                                                                                |
|`types`                |          | Declares essential building block types like `byte_t`.                                                                                                                                                                                |
|`UniqueId`             | i        | Monotonic increasing IDs within a process.                                                                                                                                                                                            |
|`unique_ptr`           |          | Provides a heap-less unique ptr implementation, unlike the STL                                                                                                                                                                        |
|`variant`              |          | C++11 implementation of the C++17 feature `std::variant`                                                                                                                                                                              |
|`variant_queue`        |          | A queue which wraps multiple variants of Queues (FiFo, SoFi, ResizeableLockFreeQueue)                                                                                                                                                 |
|`vector`               |          | Heap and exception free implementation of `std::vector`                                                                                                                                                                               |

### Concurrent

If you have to write concurrent code, never use concurrency constructs like `mutex`, `atomic`, `thread`, `semaphore`, etc. directly. Most of the use cases can be solved by using an `ActiveObject` which uses as building block our `FiFo` or a
queue which is thread-safe when combined with `smart_lock`. To learn more about active objects see [Prefer Using Active Objects Instead Of Naked Threads](https://www.drdobbs.com/parallel/prefer-using-active-objects-instead-of-n/225700095).

| class                    | internal | description |
|:------------------------:|:--------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|`FiFo`                    | i        | Single producer, single consumer lock-free FiFo                                                                                                                                             |
|`LockfreeQueue`           |          | Multi producer, multi consumer lock-free FiFo with ringbuffer like overflow handling                                                                                                        |
|`LoFFLi`                  | i        | Lock-free LIFO based index manager (lock-free free list). One building block of our memory manager. After construction it contains the indices {0 ... n} which you can acquire and release. |
|`PeriodicTask`            | i        | Periodically executes a callable specified by the template parameter in a configurable time interval.                                                                                       |
|`ResizeableLockFreeQueue` |          | Resizeable variant of the `LockfreeQueue`                                                                                                                                                   |
|`smart_lock`              | i        | Creates arbitrary thread safe constructs which then can be used like smart pointers. If some STL type should be thread safe use the smart_lock to create the thread safe version in one line. Based on some ideas presented in [Wrapping C++ Member Function Calls](https://stroustrup.com/wrapper.pdf) |
|`SoFi`                    | i        | Single producer, single consumer lock-free safely overflowing FiFo (SoFi).                                                                                                                  |
|`TACO`                    | i        | Thread Aware exChange Ownership (TACO). Solution if you would like to use `std::atomic` with data types larger than 64 bit. Wait free data synchronization mechanism between threads.       |

attribute overview of the available Queues:

| Data Structure           | Shared Memory usable  | Thread-Safe | Lock-Free | Concurrent Producers : Consumers | Bounded Capacity | Data Type Restriction | Use Case                                                                                                                |
|--------------------------|-----------------------|-------------|-----------|----------------------------------|------------------|-----------------------|-------------------------------------------------------------------------------------------------------------------------|
|`FiFo`                    | Yes                   | Yes         | Yes       | 1:1                              | Yes              | Copyable              | FIFO Data transfer                                                                                                      |
|`LockfreeQueue`           | Yes                   | Yes         | Yes       | n:m                              | Yes              | Copyable or Movable   | lock-free transfer of arbitrary data between multiple contexts in FIFO order with overflow handling (ringbuffer)        |
|`LoFFLi`                  | Yes                   | Yes         | Yes       | n:m                              | Yes              | int32                 | manage memory access, LIFO order                                                                                        |
|`smart_lock`              | Yes                   | Yes         | No        | n/a                              | n/a              | None                  | Wrapper to make classes thread-safe (by using a lock)                                                                   |
|`SoFi`                    | Yes                   | Yes         | Yes       | 1:1                              | Yes              | Trivially Copyable    | lock-free transfer of small data (e.g. pointers) between two contexts in FIFO order with overflow handling (ringbuffer) |
|`ResizeableLockFreeQueue` | Yes                   | Yes         | Yes       | n:m                              | Yes              | Copyable or Movable   | Resizeable variant of the `LockfreeQueue`                                                                               |

### Design pattern

| class               | internal | description                                                         |
|:-------------------:|:--------:|:--------------------------------------------------------------------|
|`Builder`            |          | Macro which generates a setter method useful for a builder pattern. |

### Error handling

The error handler is a central instance for collecting all errors and react to them. The `error-handling.hpp` contains a list of all error enum values. The error handler has different error levels, for more information see [error-handling.md](../doc/design/error-handling.md)

| class                   | internal | maybe obsolete | description |
|:-----------------------:|:--------:|:--------------:|:------------|
|`errorHandler`           |   |   | Free function to call the error handler with a defined error and an error level, see header file for practical example.|
|`ErrorHandler`           | i |   | error handler class only for testing purposes, should not be used directly |

### Log

For information about how to use the logger API see [error-handling.md](../doc/design/error-handling.md)

| class                   | internal | description |
|:-----------------------:|:--------:|:------------|
|`logger`                 |          |             |

### POSIX wrapper

We abstract POSIX resources following the RAII idiom and by using our [Creation](#design-pattern) pattern. Try to exclusively use these
abstractions or add a new one when using POSIX resources like semaphores, shared memory, etc.

| class                              | internal | description                                                                                                                                                                                      |
|:----------------------------------:|:--------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|`AccessController`                  | i        | Interface for Access Control Lists (ACL).                                                                                                                                                        |
|`FileLock`                          |          | File lock C++ wrapping class.                                                                                                                                                                    |
|`IpcChannel`                        | i        | Helper types used by the `MessageQueue`and the `UnixDomainSocket`.                                                                                                                               |
|`mutex`                             | i        | Mutex interface, see [ManPage pthread_mutex_lock](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html).                                                                             |
|`posix_access_rights`               |          | Rights and user management interface.                                                                                                                                                            |
|`posixCall`                         |          | Wrapper around C and POSIX function calls which performs a full error handling. Additionally, this wrapper makes sure that `EINTR` handling is performed correctly by repeating the system call. |
|`SignalGuard`                       |          | Helper class for signal handler registration.                                                                                                                                                    |
|`Semaphore`                         |          | Semaphore interface, see [ManPage sem_overview](https://man7.org/linux/man-pages/man7/sem_overview.7.html)                                                                                       |
|`shared_memory_object/Allocator`    | i        | Helper class for the `SharedMemoryObject`.                                                                                                                                                       |
|`shared_memory_object/MemoryMap`    | i        | Abstraction of `mmap`, `munmap` and helper class for the `SharedMemoryObject`.                                                                                                                   |
|`shared_memory_object/SharedMemory` | i        | Abstraction of shared memory, see [ManPage shm_overview](https://www.man7.org/linux/man-pages/man7/shm_overview.7.html) and helper class for the `SharedMemoryObject`.                           |
|`SharedMemoryObject`                | i        | Creates and maps existing shared memory into the application.                                                                                                                                    |
|`system_configuration`              | i        | Collection of free functions which acquire system information like the page-size.                                                                                                                |
|`thread`                            |          | Wrapper for pthread functions like `pthread_setname_np`.                                                                                                                                         |
|`UnixDomainSocket`                  | i        | Interface for unix domain sockets.                                                                                                                                                               |

### Units

Never use physical properties like speed or time directly as integer or float in your code.
Otherwise you encounter problems like this function `void setTimeout(int timeout)`. What is the unit of the argument, seconds? minutes? If you use `Duration` you see it directly in the code.

```cpp
void setTimeout(const Duration & timeout);

setTimeout(11_s); // 11 seconds
setTimeout(5_ms); // 5 milliseconds
```

| class               | internal | description                                                                                                                                 |
|:-------------------:|:--------:|:--------------------------------------------------------------------------------------------------------------------------------------------|
|`Duration`           | i        | Represents the unit time, is convertible to `timespec` and `timeval`. User defined literals are available for convenience and readability.  |

<center>
[Check out iceoryx_hoofs on GitHub :fontawesome-brands-github:](https://github.com/eclipse-iceoryx/iceoryx/tree/master/iceoryx_hoofs/){ .md-button } <!--NOLINT required only for the website, github URL required-->
</center>
