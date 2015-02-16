// -*- Mode: C++; -*-

#include <cassert>
#include <memory>
#include <list>

template<class T>
class FAA
{
private:
  struct Node;
  typedef std::shared_ptr<const Node> pNode;
  
  struct Node {
    Node (uint8_t level, pNode const & l, pNode const & r, T val) 
      : _level(level), _l(l), _r(r), _val(val) {}
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
  };

  explicit FAA (pNode const & node) : _node(node) {}

  static void W (char ch) {fputc (ch, stderr);}

  static FAA
  skew (FAA const & n)
  {
    if (n.level() != 0) {
      if (n.left().level() == n.level()) {
	//W ('K');
	return FAA (
	  n.level(), 
	  n.left().left(),
	  skew (FAA (n.level(), n.left().right(), n.right(), n.val())),
	  n.left().val()
	);
      } else {
	return FAA (n.level(), n.left(), skew (n.right()), n.val());
      }
    } else {
      return n;
    }
  }

  static FAA
  split (FAA const & n)
  {
    if ((n.level() != 0) && (n.right().right().level() == n.level())) {
      //W ('P');
      return FAA (
	n.right().level() + 1,
	FAA (n.level(), n.left(), n.right().left(), n.val()),
	split (n.right().right()),
	n.right().val()
      );
    } else {
      return n;
    }
  }

  // emulate mutability.
  static FAA set_l     (FAA & n, FAA & l) {return FAA (n.level(), l, n.right(), n.val());}
  static FAA set_r     (FAA & n, FAA & r) {return FAA (n.level(), n.left(), r, n.val());}
  static FAA set_level (FAA & n, uint8_t level) {return FAA (level, n.left(), n.right(), n.val());}
  static FAA set_val   (FAA & n, T val) {return FAA (n.level(), n.left(), n.right(), val);}

  static FAA
  remove0 (Remover & r, const FAA & root, T & key)
  {
    FAA root0;

    // --- descend ---
    if (!root.is_empty()) {
      //fprintf (stderr, "<%d>", root.level());
      r._heir = root._node;
      if (root.val() >= key) {
	// this is set on each right turn
	r._item = root._node;
	//W ('L');
	root0 = FAA (root.level(), remove0 (r, root.left(), key), root.right(), root.val());
	if (root.val() == key) {
	  // do the swap
	  r._removed = root.val();
	  //fprintf (stderr, "(%d)", r._removed);
	  root0 = set_val (root0, r._swap);
	}
      } else {
	//W ('R');
	root0 = FAA (root.level(), root.left(), remove0 (r, root.right(), key), root.val());
      }
    } else {
      root0 = root;
    }
    // --- ascend ---
    //if (root.level() == 1) {
    if (root._node == r._heir) {
      //W ('B');
      // at the bottom, remove
      if (r._item && r._item->_val == key) {
	//W ('S');
	r._swap = root0.val();
	//fprintf (stderr, "[%d]", r._swap);
	// here we differ from AA's algorithm
	if (root0.right().is_empty()) {
	  return root0.left();
	} else {
	  return root0.right();
	}
      } else {
	return root0;
      }
    } else {
      // not at the bottom, check balance
      if ((   root0.left().level() < (root0.level() - 1))
	  || (root0.right().level() < (root0.level() - 1))) {
	//W ('/');
	root0 = set_level (root0, root0.level() - 1);
	if (root0.right().level() > root0.level()) {
	  // equivalent to root0.r.level = root0.level
	  FAA right0 = root0.right();
	  FAA right1 = set_level (right0, root0.level());
	  root0 = set_r (root0, right1);
	}
	root0 = split (skew (root0));
	return root0;
      } else {
	//W ('=');
	return root0;
      }
    }
  }

public:

  FAA() {}
  FAA (uint8_t level, FAA const & l, FAA const & r, T val)
    : _node (std::make_shared<const Node>(level, l._node, r._node, val))
  {
    // the '<=' allows duplicates, otherwise we abort.
    //assert (l.is_empty() || l.val() <= val);
    //assert (r.is_empty() || val <= r.val());
  }
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
  bool is_empty() const { return !_node; }

  // I think with Julienne's sentinel 'nil' node, these accessors (and empty checks) 
  // would not be necessary.
  T val() const
  {
    assert (!is_empty());
    return _node->_val;
  }
  uint8_t level() const
  {
    if (is_empty()) {
      return 0;
    } else {
      return _node->_level;
    }
  }
  FAA left() const
  {
    if (is_empty()) {
      return *this;
    } else {
      return FAA (_node->_l);
    }
  }
  FAA right() const
  {
    if (is_empty()) {
      return *this;
    } else {
      return FAA (_node->_r);
    }
  }

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
    if (is_empty()) {
      return FAA (1, FAA(), FAA(), val0);
    } else if (val() > val0) {
      return split (skew (FAA (level(), left().insert (val0), right(), val())));
    } else {
      return split (skew (FAA (level(), left(), right().insert (val0), val())));
    }
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

  FAA remove (T val0) const
  {
    Remover r;
    return remove0 (r, *this, val0);
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


private:
    pNode _node;
};

