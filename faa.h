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

#if 0
  struct Remover {
    T _heir_val;
    FAA _item;
    T _removed;
  };
#endif

  explicit FAA (pNode const & node) : _node(node) {}

#if 0
  static FAA 
  skew (FAA const & n)
  {
    if ((n.level() != 0) and n.left().level() == n.level()) {
      return FAA (
	n.level(),
	n.left().left(),
	FAA (n.level(), n.left().right(), n.right(), n.val()),
	n.left().val()
      );
    } else {
      return n;
    }
  }
#else
  // recursive skew
  static FAA
  skew (FAA const & n)
  {
    if (n.level() != 0) {
      if (n.left().level() == n.level()) {
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
#endif

#if 0
  static FAA
  split (FAA const & n) 
  {
    if ((n.level() != 0) and n.right().right().level() == n.level()) {
      return FAA (
	n.right().level() + 1,
	FAA (n.level(), n.left(), n.right().left(), n.val()),
	n.right().right(),
	n.right().val()
      );
    } else {
      return n;
    }
  }
#else
  static FAA
  split (FAA const & n)
  {
    if ((n.level() != 0) && (n.right().right().level() == n.level())) {
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
#endif

#if 0
  // emulate mutability.
  static FAA set_l     (FAA & n, FAA & l) {return FAA (n.level(), l, n.right(), n.val());}
  static FAA set_r     (FAA & n, FAA & r) {return FAA (n.level(), n.left(), r, n.val());}
  static FAA set_level (FAA & n, uint8_t level) {return FAA (level, n.left(), n.right(), n.val());}

  static FAA
  remove0 (Remover & r, const FAA & root, T & key)
  {
    FAA root0;
    bool is_heir = false;
    if (!root.is_empty()) {
      is_heir = true;
      r._heir_val = root.val()
      if (root.val() == key) {
	// let the bottom know we found it on the way down...
	r._item = root;
	root0 = r._item;
	FAA new_left = remove0 (r, root.left(), key);
	root0 = set_l (root0, new_left);
      } else {
	root0 = FAA (root.level(), root.left(), remove0 (r, root.right(), key), root.val());
      }
    } else {
      root0 = root;
    }
    if (root == (const FAA<T>) r._heir) {
      // at the bottom, remove
      if (!r._item.is_empty() && r._item.val() == key) {
      }
    }
    return root0;
  }
#endif

public:

  FAA() {}
  FAA (uint8_t level, FAA const & l, FAA const & r, T val)
    : _node (std::make_shared<const Node>(level, l._node, r._node, val))
  {
    // the '<=' allows duplicates, otherwise we abort.
    assert (l.is_empty() || l.val() <= val);
    assert (r.is_empty() || val <= r.val());
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

#if 0
  FAA remove (T val0) const
  {
    Remover r;
    // remove0 (Remover & r, FAA const & root, T & key)
    return remove0 (r, *this, val0);
  }
#endif

private:
    pNode _node;
};

