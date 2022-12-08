# Small Buffer Optimization

This submodule implements small buffer optimizations for certain container classes. Memory is allocated up front on the stack initially and falls back to heap allocation when required.

## Notes
- use `cmake .. -DDEBUG=ON` or `cmake .. -DDEBUG=OFF` to disable or enable additional memory-related debugging checks. **Using `valgrind` with `-DDEBUG=ON` will lead to errors** as `valgrind` is cannot work with the overloaded global `new` and `delete` operators
