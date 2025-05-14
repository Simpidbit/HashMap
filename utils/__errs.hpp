/**
 * @file __errs.hpp
 * @brief utils 模块的内部文件，你不应该主动包含它.
 */
#ifndef HASHMAP_UTILS___ERRS_HPP
#define HASHMAP_UTILS___ERRS_HPP

#include <exception>
#include <string>

namespace utils {

/**
 * @brief 本模块(utils)中所有可能抛出的异常都是此类型.
 */
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
