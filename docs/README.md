# utils
Some headers with various utility stuff.

Include utils.hpp.
Other files are not supposed to be included directly.

**FAIR WARNING**

I'm a Windows guy, and I always use Visual Studio with the latest C++ version forced via
`std:c++latest`. There's no guarantee any of the things below will
compile anywhere else. Just keep this in mind.

Supporting things other than VS on Windows would be nice, but I don't
really have time for that. The latest clang on godbolt seems to be
able to chew most of the things I try to throw at it.

In any case, your compiler needs to at least understand `C++ 23`.

## Overview

The repo contains several unrelated "library" headers with different
things I find convenient to keep in one easily-accessible place.

They have nothing in common, and, quite frankly, the selection is
arbitrary and rather eclectic.

My other repos depend on these, but I don't know if there's a point
in using them on their own.

## Convenience macros

There are some macros which help type less.
Yeah, I know, macros are evil and blah-blah-blah.

### For classes

`FROM_CONST` helps write `const` overloads for class methods
```
struct c
{
  int f_;

  const int& get() const { return f_; }
  int& get() { return FROM_CONST(get); }
};
```
The benefit of this is not obvious with simple examples, but it helps
reduce code duplication a lot.

`CLASS_XXXX` is a family of macros which generate special class members.

They are needed to avoid doing this:
```
struct c
{
  c() = delete;
  c(const c&) = delete;
  c& operator=(const c&) = delete;
  c(c&&) = default;
  c& operator=(c&&) = default;
};
```
This is a lot of typing. This is much easier:
```
struct c
{
  CLASS_SPECIALS_NODEFAULT_NOCOPY(c);
};
```
There are macros whose names end in `_CUSTOM`. Those leave the default ctor up to you:
```
struct c
{
  c()
  {
    some_weird_logic();
  }

  c(const c&) = defautl;
  c& operator=(const c&) = default;
  c(c&&) = default;
  c& operator=(c&&) = default;
};
```
Everything is defaulted, but the ctor is implemented by hand.
This calls for:
```
struct c
{
  CLASS_SPECIALS_ALL_CUSTOM(c);
  
  c()
  {
    some_weird_logic();
  }
};
```

### For debugging

`BREAK_ON` is handy if you'd like to set a custom programmatic breakpoint

Just do something along these lines:
```
void f(int a)
{
  BREAK_ON(a < 0);
}
```
It will break if the provided condition is true.
static_asserts to false in release mode, so only use in debug.

`UTILS_ASSERT` speaks for itself. It will break if the condition is false
and tell you where in your code it was triggered.
It will even print the call stack for you.

### Utility stuff

`SCOPE_GUARD` and `VALUE_GUARD` will generate scope guards and give them
unique names without spelling everything related to them explicitly (I hate typing, you know).

`TYPE_TO_ID_ASSOCIATION` specialises templates which associate types with ids (see the note on type support below)

## Parts of the "library"

### Common for all

There are some functions and concepts in `common.hpp` and `dependencies.hpp` wich are just handy
to have around. Take a look at these files and read the comments there.

### Assert

Not much to say here. There's a function called `assertion` which checks
conditions and complains if they are false.

### Clock

Just a wrapper for some `chrono` stuff. This is a simple `clock` class
which counts time and allows you to find out how much of it has passed since the last reset.

### Colour

Again, a wrapper. It allows constructing colours from RGBA and manipulating them.

### Generic maths

`math.hpp` and `trig.hpp` contain some `constexpr` capable math functions.
There are also ordinary fractions in `ratio.hpp`

### Linear algebra -ish

`vector.hpp` and `matrix.hpp` are algebraic vectors and matrices.
Everything is `constexpr`-able.

### Random

Yet another wrapper for various standard random-number algorithms.

### Strings

`strings.hpp` has functions for string manipulation (trimming, mostly),
and hashed string representation.

### Multi-iter

Aah, this one deserves a little explanation.
Let's say, you have two `std::vector`s you'd like to traverse synchronously in order.
Don't ask me why, but I've had a need to do this not long ago.

The multi-iter thing allows you to perform such a task in an easy-peasy way.
It will take a number of iterable collections from you and give you an iterable object.
By iterating this object, you can traverse these collections for as long as none of them
have reached the end. That is, you'll get as many iterations as there are elments in
the shortest collection.

What I mean:
```
std::array a1{ 1, 2, 3 };
std::array a2{ 1, 2, 3, 4, 5, 6 };
std::array a3{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
for(auto&&[e1, e2, e3] : utils::make_iterators(a1, a2, a3))
{
  // 3 iterations sice a1 is the shortest one,
  // and it has 3 elements
}
```

### Scope guards

There are two of them. One saves a value of some variable and reverts it back
upon exiting the current scope. The other calls a function in its dtor.

### System handler

This is for setting system `terminate` and `new` handlers.
Can chain multiple of them to be called one after the other.

### Type support

`type_support.hpp` is about associating types with ids.
Useful to cast stuff. Take a look at
[this file](/ut_tests/src/type_support.cpp)
It will explain things better than I would hope to.

