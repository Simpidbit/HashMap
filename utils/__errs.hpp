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
    /**
     * @brief 默认信息为 "utils::utils_exception.".
     */
    utils_exception() { this->detail_str = "utils::utils_exception."; }

    /**
     * @brief 给定信息的异常(右值版本).
     * @param str 异常信息.
     */
    utils_exception(std::string &&str) {
      this->detail_str = std::move(str);
    }

    /**
     * @brief 给定信息的异常(左值版本).
     * @param str 异常信息.
     */
    utils_exception(const std::string &str) {
      this->detail_str = str;
    }

    const char *what () const throw () {
      return this->detail_str.c_str();
    }
};

} // namespace utils

#endif    // HASHMAP_UTILS___ERRS_HPP
