#ifndef HASHMAP_UTILS___ERRS_HPP
#define HASHMAP_UTILS___ERRS_HPP

#include <exception>
#include <string>

namespace utils {

class utils_exception : public std::exception {
  protected:
    std::string detail_str;

  public:
    utils_exception() = delete;
    utils_exception(std::string &&str) {
      this->detail_str = std::move(str);
    }
    utils_exception(const std::string &str) {
      this->detail_str = str;
    }

    const char *what () const throw () {
      return this->detail_str.c_str();
    }
};

} // namespace utils

#endif    // HASHMAP_UTILS___ERRS_HPP
