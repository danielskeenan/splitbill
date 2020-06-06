/**
 * @file BillLine.h
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#ifndef SPLITBILL_SRC_LIB_BILL_H_
#define SPLITBILL_SRC_LIB_BILL_H_

#include <string>
#include <vector>
#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/debug_adaptor.hpp>

namespace splitbill {
using namespace boost;

typedef multiprecision::number<multiprecision::cpp_rational_backend> Money;

/**
 * Bill Line
 */
struct BillLine {
  std::string name = "";
  std::string description = "";
  Money tax_rate = 0;
  Money amount = 0;
  bool split = true;

  BillLine() = default;
  BillLine(const BillLine &other) = default;

  bool operator==(const BillLine &rhs) const {
    return name == rhs.name &&
        description == rhs.description &&
        tax_rate == rhs.tax_rate &&
        amount == rhs.amount &&
        split == rhs.split;
  }

  bool operator!=(const BillLine &rhs) const {
    return !(rhs == *this);
  }
};

typedef std::vector<BillLine> BillLineList;

/**
 * Bill, post split
 */
class SplitBill {
 public:
  SplitBill(const Money &usage_total, const Money &general_total)
      : usageTotal_(usage_total), generalTotal_(general_total) {}

  const Money &GetMoneyUsageTotal() const { return usageTotal_; }

  const double GetUsageTotal() const { return GetMoneyUsageTotal().convert_to<double>(); }

  const Money &GetMoneyGeneralTotal() const { return generalTotal_; }

  const double GetGeneralTotal() const { return GetMoneyGeneralTotal().convert_to<double>(); }

  const Money GetMoneyTotal() const { return GetMoneyUsageTotal() + GetMoneyGeneralTotal(); }

  const double GetTotal() const { return GetMoneyTotal().convert_to<double>(); }

 private:
  const Money usageTotal_;
  const Money generalTotal_;
};

/**
 * A person's portion of the bill
 */
class BillPortion : public SplitBill {
 public:
  explicit BillPortion(const std::string &name, const Money &usage_total, const Money &general_total) :
      SplitBill(usage_total, general_total), name_(name) {}

  const std::string GetName() const { return name_; }

 private:
  std::string name_;
};

/**
 * Associate a person with the period they were present.
 */
class PersonPeriod {
 public:
  explicit PersonPeriod() :
      name_(""),
      period_(gregorian::date_period(gregorian::day_clock::local_day(),
                                     gregorian::day_clock::local_day() + gregorian::date_duration(1))) {}

  /**
   * Create a new PersonPeriod for <name> present for <period>.
   *
   * @param name
   * @param period
   */
  explicit PersonPeriod(const std::string &name, const gregorian::date_period &period) :
      name_(name), period_(period) {}

  /**
   * Create a new PersonPeriod for <name> present from <start> to <end>, inclusive.
   * @param name
   * @param start
   * @param end
   */
  explicit PersonPeriod(const std::string &name, const std::string &start, const std::string &end) :
      PersonPeriod(name,
                   gregorian::date_period(gregorian::from_string(start),
                                          gregorian::from_string(end) + gregorian::date_duration(1))) {}

  [[nodiscard]] const std::string GetName() const { return name_; }

  void SetName(const std::string &name) { name_ = name; }

  [[nodiscard]] const gregorian::date_period GetPeriod() const { return period_; }

  void SetPeriod(const gregorian::date_period &period) { period_ = period; }

  [[nodiscard]] const std::string GetStart() const { return gregorian::to_iso_extended_string(period_.begin()); }

  void SetStart(const std::string &start) {
    period_ = gregorian::date_period(gregorian::from_string(start), period_.end());
  }

  [[nodiscard]] const std::string GetEnd() const { return gregorian::to_iso_extended_string(period_.last()); }

  void SetEnd(const std::string &end) {
    period_ = gregorian::date_period(period_.begin(), gregorian::from_string(end) + gregorian::date_duration(1));
  }

 private:
  std::string name_;
  gregorian::date_period period_;
};

/**
 * Validation errors
 */
typedef enum {
  VALID = 0,
  LINE_SUM_NOT_TOTAL,
} ValidationError;

/**
 * Bill
 */
class Bill {
 public:
  /**
   * Tote the bill
   *
   * @return
   */
  SplitBill Total();

  /**
   * Split the bill according to period.
   *
   * @param periods
   * @param people
   * @return
   */
  std::vector<splitbill::BillPortion> Split(const gregorian::date_period &period,
                                            const std::vector<PersonPeriod> &person_periods,
                                            const std::vector<std::string> &people);

  /**
   * Split the bill according to period.
   *
   * @param periods
   * @param people
   * @return
   */
  std::vector<splitbill::BillPortion> Split(const std::string &start,
                                            const std::string &end,
                                            const std::vector<PersonPeriod> &person_periods,
                                            const std::vector<std::string> &people) {
    return Split(gregorian::date_period(gregorian::from_string(start),
                                        gregorian::from_string(end) + gregorian::date_duration(1)),
                 person_periods,
                 people);
  }

  /**
   * Is the bill valid?
   * @param error
   * @return
   */
  bool IsValid(ValidationError &error);

  [[nodiscard]] const Money &GetTotalMoneyAmount() const {
    return total_amount_;
  }

  [[nodiscard]] const double GetTotalAmount() const {
    return total_amount_.convert_to<double>();
  }

  void SetTotalMoneyAmount(const Money &total_amount) {
    total_amount_ = total_amount;
  }

  void SetTotalAmount(const double &total_amount) {
    total_amount_ = total_amount;
  }

  [[nodiscard]] const BillLineList &GetLines() const {
    return lines_;
  }

  [[nodiscard]] const BillLine &GetLine(const size_t &pos) const {
    return lines_.at(pos);
  }

  [[nodiscard]] const size_t GetLineCount() const {
    return lines_.size();
  }

  void AddLine(const BillLine &line, const size_t &pos) {
//    for (const auto &test : lines_) {
//      if (test == line) {
//        return;
//      }
//    }
    lines_.insert(lines_.cbegin() + pos, line);
  }

  void AddLine(const BillLine &line) {
    lines_.push_back(line);
  }

  void RemoveLine(const size_t &pos) {
    lines_.erase(lines_.cbegin() + pos);
  }

  void RemoveLine(const BillLine &line) {
    for (size_t i = 0; i < lines_.size(); i++) {
      if (lines_.at(i) == line) {
        RemoveLine(i);
        return;
      }
    }
  }

  void UpdateLine(const size_t &pos, const BillLine &line) {
    lines_.erase(lines_.cbegin() + pos);
    lines_.insert(lines_.cbegin() + pos, line);
  }

 private:
  Money total_amount_;
  BillLineList lines_;

  static std::vector<Money> GetAmounts(const BillLineList &lines);
  static std::vector<BillLine> ApplyTax(const BillLineList &lines);
  static void SortLinesBySplit(const BillLineList &lines, BillLineList &split_lines, BillLineList &not_split_lines);
  static Money Accumulate(const std::vector<Money> &items);
};

} // splitbill

#endif //SPLITBILL_SRC_LIB_BILL_H_
