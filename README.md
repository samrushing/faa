
Persistent Binary Search Tree
=============================

This is a functional data structure a la Okasaki's "Functional Data Structures".

Okasaki's version of Red-Black Trees was inspirational, but he
famously left the ``delete()`` method as an exercise for the reader.
I've found an [implementation of delete in Racket/Scheme](http://matt.might.net/articles/red-black-delete/),
but it uses pattern matching and is fairly complex.

Arne Anderson invented a simplified variant of red-black trees, now
called 'AA Trees', that trades slightly lower performance for a
massively simplified implementation.

Julienne Walker has an *excellent*
[website](http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_andersson.aspx)
detailing several data structures, and a very nice exposition of AA
trees.  Using that reference I was able to make a functional AA tree
for Cython/Python in 2014, but all versions of AA trees are in a very
imperative style, so it's a bit tricky to translate that to a pure variant.

Recently I learned that C++11's new ``shared_ptr`` can be used to
implement functional data structures without *too* much pain, and
found Bartosz Milewski's C++ version of Okasaki's
[data structure collection](https://github.com/BartoszMilewski/Okasaki).

Using his ``RBTree`` as a take-off point, I have made this hybrid of
my functional AA tree.  It's a work in progress, but I think it may
actually be functional.

