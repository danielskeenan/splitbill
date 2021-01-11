/**
 * @file BillLine.h
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#ifndef SPLITBILL_INCLUDE_LIB_BILL_H_
#define SPLITBILL_INCLUDE_LIB_BILL_H_

#include <string>
#include <vector>
#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Money.h"

namespace splitbill {

/**
 * Bill Line
 */
struct BillLine {
  std::string name;
  std::string description;
  double tax_rate = 0;
  Money amount = Money(0, Currency::Code::USD);
  bool split = true;

  explicit BillLine() = default;

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

/**
 * Bill, post split
 */
class SplitBill {
 public:
  SplitBill(const Money &usage_total, Money general_total)
      : usageTotal_(usage_total), generalTotal_(std::move(general_total)) {}

  [[nodiscard]] const Money &GetUsageTotal() const { return usageTotal_; }

  [[nodiscard]] const Money &GetGeneralTotal() const { return generalTotal_; }

  [[nodiscard]] Money GetTotal() const { return GetUsageTotal() + GetGeneralTotal(); }

 private:
  const Money usageTotal_;
  const Money generalTotal_;
};

/**
 * A person's portion of the bill
 */
class BillPortion : public SplitBill {
 public:
  explicit BillPortion(std::string name, const Money &usage_total, const Money &general_total) :
      SplitBill(usage_total, general_total), name_(std::move(name)) {}

  [[nodiscard]] const std::string &GetName() const { return name_; }

 private:
  std::string name_;
};

/**
 * Associate a person with the period they were present.
 */
class PersonPeriod {
 public:
  explicit PersonPeriod() :
  // 1 day
      period_(boost::gregorian::date_period(boost::gregorian::day_clock::local_day(),
                                            boost::gregorian::day_clock::local_day()
                                                + boost::gregorian::date_duration(1))) {}

  /**
   * Create a new PersonPeriod for <name> present for <period>.
   *
   * @param name
   * @param period
   */
  explicit PersonPeriod(std::string name, const boost::gregorian::date_period &period) :
      name_(std::move(name)), period_(period) {}

  /**
   * Create a new PersonPeriod for <name> present from <start> to <end>, inclusive.
   * @param name
   * @param start
   * @param end
   */
  explicit PersonPeriod(const std::string &name, const std::string &start, const std::string &end) :
      PersonPeriod(name,
                   boost::gregorian::date_period(boost::gregorian::from_string(start),
                                                 boost::gregorian::from_string(end)
                                                     + boost::gregorian::date_duration(1))) {}

  [[nodiscard]] const std::string &GetName() const { return name_; }

  void SetName(const std::string &name) { name_ = name; }

  [[nodiscard]] const boost::gregorian::date_period &GetPeriod() const { return period_; }

  void SetPeriod(const boost::gregorian::date_period &period) { period_ = period; }

  [[nodiscard]] std::string GetStart() const { return boost::gregorian::to_iso_extended_string(period_.begin()); }

  void SetStart(const std::string &start) {
    period_ = boost::gregorian::date_period(boost::gregorian::from_string(start), period_.end());
  }

  [[nodiscard]] std::string GetEnd() const { return boost::gregorian::to_iso_extended_string(period_.last()); }

  void SetEnd(const std::string &end) {
    period_ = boost::gregorian::date_period(period_.begin(),
                                            boost::gregorian::from_string(end) + boost::gregorian::date_duration(1));
  }

 private:
  std::string name_;
  boost::gregorian::date_period period_;
};

/**
 * Validation errors
 */
enum class ValidationError {
  kValid = 0,
  kLineSumNotTotal,
};

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
  std::vector<splitbill::BillPortion> Split(const boost::gregorian::date_period &period,
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
    const boost::date_time::period<boost::gregorian::date, boost::gregorian::date_duration>
        period(boost::gregorian::from_string(start),
               boost::gregorian::from_string(end) + boost::gregorian::date_duration(1));
    return Split(period,
                 person_periods,
                 people);
  }

  /**
   * Is the bill valid?
   * @param error
   * @return
   */
  bool IsValid(ValidationError &error);

  [[nodiscard]] const Money &GetTotalAmount() const {
    return total_amount_;
  }

  void SetTotalAmount(const Money &total_amount) {
    total_amount_ = total_amount;
  }

  [[nodiscard]] const std::vector<BillLine> &GetLines() const {
    return lines_;
  }

  [[nodiscard]] const BillLine &GetLine(const size_t &pos) const {
    return lines_.at(pos);
  }

  [[nodiscard]] size_t GetLineCount() const {
    return lines_.size();
  }

  void AddLine(const BillLine &line, const size_t &pos) {
    lines_.insert(lines_.cbegin() + pos, line);
  }

  void AddLine(const BillLine &line) {
    lines_.push_back(line);
  }

  void RemoveLine(const size_t &pos) {
    lines_.erase(lines_.cbegin() + pos);
  }

  void RemoveLine(const BillLine &line) {
    lines_.erase(std::remove(lines_.begin(), lines_.end(), line));
  }

  void UpdateLine(const size_t &pos, const BillLine &line) {
    lines_[pos] = line;
  }

 private:
  Money total_amount_;
  Currency currency_;
  std::vector<BillLine> lines_;

  static std::vector<Money> GetAmounts(const std::vector<BillLine> &lines);
  static std::vector<BillLine> ApplyTax(const std::vector<BillLine> &lines);
  static void SortLinesBySplit(const std::vector<BillLine> &lines,
                               std::vector<BillLine> &split_lines,
                               std::vector<BillLine> &not_split_lines);
};

} // splitbill

#endif //SPLITBILL_INCLUDE_LIB_BILL_H_
