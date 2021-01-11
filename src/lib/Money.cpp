/**
 * @file Money.cpp
 *
 * @author dankeenan
 * @date 1/11/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "Money.h"
#include <cmath>
#include <stdexcept>

namespace splitbill {

Money::Money(const double &value, Currency::Info currency) :
    currency_(std::move(currency)), value_(value) {}

double Money::GetValue() const {
  const unsigned int multiplier = currency_.multiplier();
  return std::round((value_ * multiplier).convert_to<double>()) / multiplier;
}

const Currency::Info &Money::GetCurrency() const {
  return currency_;
}

bool Money::operator==(const Money &rhs) const {
  return currency_ == rhs.currency_ &&
      value_ == rhs.value_;
}

bool Money::operator==(double rhs) const {
  return value_ == rhs;
}

bool Money::operator!=(const Money &rhs) const {
  return !(rhs == *this);
}

bool Money::operator!=(double rhs) const {
  return value_ != rhs;
}

bool Money::operator<(const Money &rhs) const {
  if (currency_ != rhs.currency_) {
    throw std::invalid_argument("Can only compare money with the same currency");
  }
  return value_ < rhs.value_;
}

bool Money::operator<(double rhs) const {
  return value_ < rhs;
}

bool Money::operator>(const Money &rhs) const {
  return rhs < *this;
}

bool Money::operator>(double rhs) const {
  return value_ > rhs;
}

bool Money::operator<=(const Money &rhs) const {
  return !(rhs < *this);
}

bool Money::operator<=(double rhs) const {
  return value_ <= rhs;
}

bool Money::operator>=(const Money &rhs) const {
  return !(*this < rhs);
}

bool Money::operator>=(double rhs) const {
  return value_ >= rhs;
}

#define MONEY_OP(op) \
  Money Money::operator op(const Money &rhs) const { \
    if (currency_ != rhs.currency_) { \
      throw std::invalid_argument("Can only operate on money with the same currency"); \
    } \
    return Money(value_ op rhs.value_, currency_); \
  } \
  \
  Money Money::operator op(double rhs) const { \
    return Money(value_ op rhs, currency_); \
  } \


MONEY_OP(+)

MONEY_OP(-)

MONEY_OP(*)

MONEY_OP(/)

#undef MONEY_OP

} // splitbill
