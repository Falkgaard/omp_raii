# omp_raii
QoL extension to the OpenMP run-time library.

# Usage

Code like:

```cpp
omp_lock_t locks[N] {};

for ( int i=0; i<N; ++i ) {
   omp_init_lock( &locks[i] );
}

#pragma omp parallel
{
   // ...
   omp_set_lock( &locks[x] ); // blocking wait
   // ...
   omp_unset_lock( &locks[x] );
}

#pragma omp parallel
{
   // ...
   if ( omp_test_lock( &locks[x] ) ) {
      // ...
      omp_unset_lock( &locks[x] );
   }
   else {
      // ...
   }
   // ...
}

for ( int i=0; i<N; ++i ) {
   omp_destroy_lock( &locks[i] );
}
```

becomes:

```cpp
omp_lock locks[N] {};

#pragma omp parallel
{
   // ...
   auto pass = locks[x].wait_for_pass(); // blocking wait
   // ...
}

#pragma omp parallel
{
   // ...
   if ( auto maybe_pass = locks[x].request_pass() ) {
      // ...
   }
   else {
      // ...
   }
   // ...
}
```

Since the lifetimes of the classes added by this header are scoped, it can sometimes be wise to wrap the passes inside a `{...}` block with just the logic that relies on the lock, which might reduce the lock occupancy time.

# TODO

1. Contemplate renaming type to omp_raii_lock

2. Add similar RAII wrappers for OpenMP's nested locks.

3. Rename `wait_for_pass` to `await_pass`?
