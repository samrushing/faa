// -*- Mode: C++; -*-

#include <cassert>
#include <memory>
#include <list>
#include "cons.h"

template<class T>
class FAA
{
private:
  struct Node;
  typedef std::shared_ptr<const Node> pNode;
  pNode _node;
  
  struct Node {
    Node (uint8_t level, pNode const & l, pNode const & r, T val) 
      : _level(level), _l(l), _r(r), _val(val) {}
    Node () {}
    uint8_t _level;
    pNode _l;
    pNode _r;    
    T _val;
  };

  struct Remover {
    pNode _heir;
    pNode _item;
    T _removed;
    T _swap;
    bool _found;
  };

  typedef ConsList<pNode> path_t;

  class iterator {

  private:
    path_t _path;

  public:

    // these typedefs provide enough magic to allow
    //   this iterator to work with STL.
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef const T& reference;
    typedef const T* pointer;
    typedef std::input_iterator_tag iterator_category;

    iterator (path_t & path) : _path(path) {}
    iterator() {}

    bool
    operator!=(const iterator & other)
    {
      return _path != other._path;
    }

    bool
    operator==(const iterator & other)
    {
      return _path == other._path;
    }

    const iterator &
    operator++()
    {
      path_t p = _path;
      if (!p) {
	return *this;
      } else {
	pNode n = car (p);
	if (n->_r->_level) {
	  n = n->_r;
	  p = cons (n, p);
	  while (n->_l->_level > 0) {
	    n = n->_l;
	    p = cons (n, p);
	  }
	  _path = p;
	  return *this;
	} else {
	  pNode child = n;
	  while (true) {
	    if (cdr(p)) {
	      p = cdr(p);
	      n = car(p);
	      if (child == n->_l) {
		_path = p;
		return *this;
	      }
	      child = n;
	    } else {
	      path_t p;
	      _path = p;
	      return *this;
	    }
	  }
	  _path = p;
	  return *this;
	}
      }
    }

    // Note: operator-- would be a mirror image of operator++, swapping all the
    //   rights and lefts.  A proper iterator implementation would do rbegin/rend/etc.
    //   Also, note that "end()--" is supposed to give you the last element.

    const T & operator*()
    {
      return car(_path)->_val;
    }


  };

  static pNode _nil;

  // tricky making the sentinel that points at itself.
  static
  pNode
  make_nil()
  {
    Node * nil = new Node;
    nil->_level = 0;
    nil->_l = std::shared_ptr<const Node>(nil);
    nil->_r = std::shared_ptr<const Node>(nil);
    return nil->_l;
  } 

  static
  pNode
  make (uint8_t level, pNode const & l, pNode const & r, T val)
  {
    return std::make_shared<const Node>(level, l, r, val);
  }

  explicit FAA (pNode const & node) : _node(node) {}

  static pNode
  skew (pNode const & n)
  {
    if (n->_level != 0) {
      if (n->_l->_level == n->_level) {
	return make (
	  n->_level, 
	  n->_l->_l,
	  skew (set_l (n, n->_l->_r)),
	  n->_l->_val
	);
      } else {
	return set_r (n, skew (n->_r));
      }
    } else {
      return n;
    }
  }

  static pNode
  split (pNode n)
  {
    if ((n->_level != 0) && (n->_r->_r->_level == n->_level)) {
      return make (
	n->_r->_level + 1,
	set_r (n, n->_r->_l),
	split (n->_r->_r),
	n->_r->_val
      );
    } else {
      return n;
    }
  }

  static 
  pNode 
  insert0 (pNode n, T val0)
  {
    if (n->_level == 0) {
      return make (1, _nil, _nil, val0);
    } else if (n->_val > val0) {
      return split (skew (set_l (n, insert0 (n->_l, val0))));
    } else {
      return split (skew (set_r (n, insert0 (n->_r, val0))));
    }
  }


  void
  verify0 (pNode n)
  {
    if (n->_level != 0) {
      verify0 (n->_l);
      assert (n->_l->_level != n->_level);
      assert (!((n->_level == n->_r->_level) && (n->_level == n->_r->_r->_level)));
      verify0 (n->_r);
    }
  }

  // emulate mutability.
  // [xxx might it be possible to have non-const Node, then modify it, then wrap it in const later?
  //   might cut down on all the likely copies generated here...]
  static pNode set_level (pNode n, uint8_t level) {return make (level, n->_l, n->_r, n->_val);}
  static pNode set_l     (pNode n, pNode l)       {return make (n->_level, l, n->_r, n->_val);}
  static pNode set_r     (pNode n, pNode r)       {return make (n->_level, n->_l, r, n->_val);}
  static pNode set_val   (pNode n, T & val)       {return make (n->_level, n->_l, n->_r, val);}

  static
  const pNode
  remove0 (Remover & r, pNode root, T & key)
  {
    pNode root0;

    // --- descend ---
    if (root->_level != 0) {
      r._heir = root;
      if (root->_val >= key) {
	// this is set on each right turn
	r._item = root;
	root0 = set_l (root, remove0 (r, root->_l, key));
	if (root->_val == key) {
	  // do the swap
	  r._removed = root->_val;
	  root0 = set_val (root0, r._swap);
	}
      } else {
	root0 = set_r (root, remove0 (r, root->_r, key));
      }
    } else {
      root0 = root;
    }
    // --- ascend ---
    if (root == r._heir) {
      // at the bottom, remove
      if (r._item && r._item->_val == key) {
	r._swap = root0->_val;
	r._found = true;
	// here we differ from AA's algorithm
	if (root0->_r->_level == 0) {
	  return root0->_l;
	} else {
	  return root0->_r;
	}
      } else {
	return root0;
      }
    } else {
      // not at the bottom, check balance
      if ((   root0->_l->_level < (root0->_level - 1))
	  || (root0->_r->_level < (root0->_level - 1))) {
	root0 = set_level (root0, root0->_level - 1);
	if (root0->_r->_level > root0->_level) {
	  // equivalent to root0.r.level = root0.level
	  root0 = set_r (root0, set_level (root->_r, root0->_level));
	}
	root0 = split (skew (root0));
	return root0;
      } else {
	return root0;
      }
    }
  }

  static
  iterator
  find0 (pNode n, const T & x, path_t & p)
  {
    if (n->_level == 0) {
      return iterator();
    } else {
      p = cons (n, p);
      if (x < n->_val) {
	return find0 (n->_l, x, p);
      } else if (n->_val < x) {
	return find0 (n->_r, x, p);
      } else {
	return iterator (p);
      }
    }
  }

public:

  FAA() : _node (_nil) 
  {}

  FAA (uint8_t level, FAA const & l, FAA const & r, T val)
    : _node (std::make_shared<const Node>(level, l._node, r._node, val))
  {}

  // populate an FAA from iterators.
  template<class I>
  FAA (I b, I e) 
  {
    FAA t;
    for_each(b, e, [&t](T const & v){
	t = t.insert(v);
      });
    _node = t._node;
  }
  bool is_empty() const { return _node == _nil; }

  T
  val() const
  {
    assert (!is_empty());
    return _node->_val;
  }

  uint8_t level() const {return _node->_level;}
  FAA left() const {return FAA (_node->_l);}
  FAA right() const {return FAA (_node->_r);}

  bool member(T x) const
  {
    if (is_empty()) {
      return false;
    } else {
      T y = val();
      if (x < y) {
	return left().member(x);
      } else if (y < x) {
	return right().member(x);
      } else {
      return true;
      }
    }
  }

  FAA insert (T val0)  const
  {
    return FAA (insert0 (_node, val0));
  }

  void
  to_list(std::list<T> & a)
  {
    if (!is_empty()) {
      left().to_list (a);
      a.push_back(val());
      right().to_list (a);
    }
  }

  // I think this would traditionally return an iterator. TBD.
  FAA remove (T val0, bool & found, T & removed) const
  {
    found = false;
    Remover r;
    pNode result = remove0 (r, _node, val0);
    if (r._found) {
      removed = r._removed;
      found = true;
      return FAA (result);
    } else {
      return *this;
    }
  }

  void
  verify()
  {
    verify0 (_node);
  }

  void
  dump (int d=1) 
  {
    if (level() > 0) {
      left().dump (d+1);
      for (int i=0; i < d; i++) {
	fprintf (stderr, "  ");
      }
      fprintf (stderr, "%4d %d\n", level(), val());
      right().dump (d+1);
    }
  }

  iterator
  begin()
  {
    pNode n = _node;
    path_t p;
    while (n->_level > 0) {
      p = cons (n, p);
      n = n->_l;
    }
    return iterator (p);
  }

  iterator
  end()
  {
    path_t p;
    return iterator(p);
  }

  iterator
  find (const T & x)
  {
    path_t null_path;
    return find0 (_node, x, null_path);
  }


};

template<typename T>
typename FAA<T>::pNode FAA<T>::_nil = FAA<T>::make_nil();

