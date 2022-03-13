#include <exception>
#include <any>

namespace lox {

class Return : public std::exception {
private:
  std::any m_value;

public:
  Return(const std::any &value)
      : m_value(value) {
  }

  std::any value() const;
};

}
