// -*- Mode: C++ -*-

#include "faa.h"
#include <stdio.h>
#include <stdint.h>
#include <random>
#include <list>

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
  FAA<int> t0;
  auto t1 = t0.insert (34);
  auto t2 = t1.insert (19);
  fprintf (stderr, "t1 %d\n", t1.val());
  fprintf (stderr, "t2 %d\n", t2.val());

  std::mt19937 generator (3141);

  for (int i=0; i < 10; i++) {
    int n = generator();
    t2 = t2.insert (n);
    fprintf (stderr, "\n--- %d\n", n);
    t2.dump();
  }
  
  //t2.dump();

  fprintf (stderr, "\ndone.\n");

  std::list<int> nl;
  t2.to_list (nl);
  
  dump (t2);

  //t2.dump();

  auto t3 = FAA<int> (nl.begin(), nl.end());
  //dump (t3);
  //t3.dump();

  auto t4 = t2.remove (19);
  //t4.dump();
  dump (t4);

  auto t5 = t4.remove (34);
  //t5.dump();
  dump (t5);


  std::mt19937 g2 (3141);

  for (int i=0; i < 10; i++) {
    int n = g2();
    t5 = t5.remove (n);
    dump (t5);
  }

}
