#include <stdexcept>

class BadWeakPtr : std::runtime_error {
 public:
  BadWeakPtr() : std::runtime_error("BadWeakPtr") {
  }
};

// YOUR CODE ...