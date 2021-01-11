/**
 * @file Money.h
 *
 * @author dankeenan
 * @date 1/11/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef SPLITBILL_INCLUDE_LIB_MONEY_H_
#define SPLITBILL_INCLUDE_LIB_MONEY_H_

#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Currency.h"

namespace splitbill {

/**
 * Store monetary values without floating-point math
 */
class Money {
 public:
  explicit Money() = default;

  explicit Money(const double &value, Currency::Info currency);

  explicit Money(const double &value, Currency::Code currency) :
      Money(value, Currency::Get(currency)) {}

  explicit Money(const double &value, const std::string &currency) :
      Money(value, Currency::Get(currency)) {}

  [[nodiscard]] double GetValue() const;
  [[nodiscard]] const Currency::Info &GetCurrency() const;

  [[nodiscard]] bool operator==(const Money &rhs) const;
  [[nodiscard]] bool operator==(double rhs) const;
  [[nodiscard]] bool operator!=(const Money &rhs) const;
  [[nodiscard]] bool operator!=(double rhs) const;
  [[nodiscard]] bool operator<(const Money &rhs) const;
  [[nodiscard]] bool operator<(double rhs) const;
  [[nodiscard]] bool operator>(const Money &rhs) const;
  [[nodiscard]] bool operator>(double rhs) const;
  [[nodiscard]] bool operator<=(const Money &rhs) const;
  [[nodiscard]] bool operator<=(double rhs) const;
  [[nodiscard]] bool operator>=(const Money &rhs) const;
  [[nodiscard]] bool operator>=(double rhs) const;

  [[nodiscard]] Money operator+(const Money &rhs) const;
  [[nodiscard]] Money operator+(double rhs) const;
  [[nodiscard]] Money operator-(const Money &rhs) const;
  [[nodiscard]] Money operator-(double rhs) const;
  [[nodiscard]] Money operator*(const Money &rhs) const;
  [[nodiscard]] Money operator*(double rhs) const;
  [[nodiscard]] Money operator/(const Money &rhs) const;
  [[nodiscard]] Money operator/(double rhs) const;

 private:
  using Decimal = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<50>>;

  Currency::Info currency_ = Currency::Get(Currency::Code::USD);
  Decimal value_ = 0;

  explicit Money(Decimal value, Currency::Info currency) :
      currency_(std::move(currency)), value_(std::move(value)) {}
};

} // splitbill

#endif //SPLITBILL_INCLUDE_LIB_MONEY_H_
