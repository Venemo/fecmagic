
Welcome to fecmagic
==========

fecmagic is a header-only C++ library which implements some FEC (forward error correction)
codes and some utility functions that help developing and testing such codes.

What's in the box?
----------

Currently, we've got the following:

* A generic block code algorithm that works with binary matrix operations, and has
  a generic method of syndrome-based decoding.
* A generic convolutional encoder algorithm that is implemented using a variadic
  template and can be configured to encode any convolutional code.

And we have specialized codecs for:

* Hamming(7, 4)
* Golay(24, 12)

And for your convenience, we've also got:

* A way to work with binary matrices efficiently
* A way to print a number in binary
* A way to iterate through bit mask combinations



How to use
----------

All the algorithms are implemented in modern C++ and should compile on a C++11 compiler.
The tests use the `0b` notation for binary numbers, so you need C++14 to compile the tests.

Since this is a header-only library, you can simply copy the necessary header files, include
them and you're good to go.

Contributions
----------

### Testing and bugreports

This code is primarily tested using GCC on Linux x86. If you find bugs or issues on other compilers
or platforms, feel free to open a bug report.

Each piece of code has a small test application. Some of these (Hamming and Golay) cover all possible
inputs, and some just contain basic tests to see if stuff works. These tests will be further expanded
as needed.

### Code

If you think you can improve on our code or add other features, your contribution is welcome.
Feel free to send us a pull request. Thanks in advance! :)

### License

The code is licensed to you under the terms of the MIT license.


