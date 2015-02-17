// -*- Mode: C++ -*-

#include "faa.h"
#include <stdio.h>
#include <stdint.h>
#include <random>
#include <list>
#include <set>

void
dump (FAA<int> & t)
{
  std::list<int> nl;
  t.to_list (nl);
  fprintf (stderr, "[");
  for (auto i = nl.begin(); i != nl.end(); ++i) {
    fprintf (stderr, "%d ", *i);
  }
  fprintf (stderr, "]\n");
}

int
main (int argc, char * argv[]) 
{
  fprintf (stderr, "nil._level = %d\n", FAA<int>::_nil->_level);
  fprintf (stderr, "nil._l._level = %d\n", FAA<int>::_nil->_l->_level);
  fprintf (stderr, "nil._l4._level = %d\n", FAA<int>::_nil->_l->_l->_l->_l->_level);

  FAA<int> t0;
  auto t1 = t0.insert (34);
  auto t2 = t1.insert (19);
  fprintf (stderr, "t1 %d\n", t1.val());
  fprintf (stderr, "t2 %d\n", t2.val());

  fprintf (stderr, "set...");
  std::mt19937 g0 (3141);

  std::set<int> m0;

  for (int i=0; i < 100000; i++) {
    int n = g0();
    m0.insert (n);
  }

  fprintf (stderr, "done...\n");
  fprintf (stderr, "faa...");
  std::mt19937 generator (3141);

  for (int i=0; i < 100000; i++) {
    int n = generator();
    t2 = t2.insert (n);
    //fprintf (stderr, "\n--- %d\n", n);
    //t2.dump();
    //t2.verify();
  }
  
  fprintf (stderr, "done...\n");

  bool found;
  int removed;
  auto t3 = t2.remove (19, found, removed);

}
