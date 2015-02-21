// -*- Mode: C++ -*-

#include "faa.h"
#include <iostream>
#include <random>
#include <list>
#include <set>

int
main (int argc, char * argv[]) 
{
  FAA<int> t0;
  auto t1 = t0.insert (34);
  auto t2 = t1.insert (19);
  std::cerr << "t1 " << t1.val() << std::endl;
  std::cerr << "t2 " << t2.val() << std::endl;

  long nitems = 1000;

  if (argc > 1) {
    nitems = atoi (argv[1]);
  }

  std::cerr << "building set...";

  std::mt19937 g0 (3141);
  std::set<int> s0;
  std::list<int> l0;

  // avoid duplicates, for now.
  for (int i=0; i < nitems; i++) {
    int n = g0();
    if (s0.find(n) == s0.end()) {
      s0.insert (n);
      l0.push_back (n);
    }
  }

  std::cerr << l0.size() << " entries...";
  std::cerr << "done.\n";
  std::cerr << "fill faa...";
  using sysclock = std::chrono::system_clock;
  std::chrono::time_point<sysclock> time0, time1;
  time0 = sysclock::now();

  for (const int & n : l0) {
    t2 = t2.insert (n);
  }
  
  time1 = sysclock::now();

  std::chrono::duration<double> elapsed_seconds = time1 - time0;
  std::cerr << elapsed_seconds.count() << std::endl;

  t2.verify();

  std::cerr << "done.\n";

  std::cerr << "iterate...";

  // verify iterator works with stl...
  std::list<int> l1 (t2.begin(), t2.end());
  int vsum=0;
  for (const int &i : t2) {
    vsum += i;
  }
  std::cerr << "done. vsum= " << vsum << std::endl;

  std::cerr << "accumulate= " << std::accumulate (t2.begin(), t2.end(), 0) << std::endl;

  std::cerr << "empty faa...";

  time0 = sysclock::now();

  bool found;
  int removed;

  for (const int & n : l0) {
    found = false;
    //std::cerr << "--------------------" << std::endl;
    //t2.dump();
    //std::cerr << "remove (" << n << ")\n";
    t2 = t2.remove (n, found, removed);
    assert (found);
    assert (removed == n);
    //t2.verify();

  }
  
  time1 = sysclock::now();

  elapsed_seconds = time1 - time0;

  std::cerr << elapsed_seconds.count() << " done.\n";

  // find

  auto probe0 = t2.find (19);
  std::cerr << "probe: " << *probe0 << std::endl;

  auto probe1 = t2.find (3498234234);
  assert (probe1 == t2.end());


  auto t3 = t2.remove (19, found, removed);
  auto t4 = t3.remove (34, found, removed);

  t4.dump();
  assert (t4.is_empty());

}
