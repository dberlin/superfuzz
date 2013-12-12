#include <stdexcept>
#include <string>
#include <unordered_map>

struct OptionBase {
  virtual void set_value(const char *str) = 0;
  virtual bool requires_argument() const = 0;
  virtual ~OptionBase();
};

std::unordered_map<std::string, OptionBase *> &get_option_map();

template <typename T> struct Option : public OptionBase {
  T value;
  Option(const char *name, T default_value = T()) : value(default_value) {
    get_option_map()[name] = this;
  }

  operator T() const { return value; }
  T &operator=(T val) {
    value = val;
    return *this;
  }

  void set_value(const char *arg);
  bool requires_argument() const;
};
template <typename T>
inline bool Option<T>::requires_argument() const {
  return true;
}
template <> inline void Option<int>::set_value(const char *arg) {
  size_t pos;
  value = std::stoi(arg, &pos);
  if (arg[pos] != '\0')
    throw std::runtime_error("stoi: illegal character in argument string\n");
}
template <> inline void Option<unsigned long>::set_value(const char *arg) {
  size_t pos;
  value = std::stoul(arg, &pos);
  if (arg[pos] != '\0')
    throw std::runtime_error("stoul: illegal character in argument string\n");
}
template <> inline bool Option<bool>::requires_argument() const { return false; }
template <> inline void Option<bool>::set_value(const char *) { value = true; }

extern void parse_options(int argc, const char *argv[]);
extern void usage(const char *program_name);

