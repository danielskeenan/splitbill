/**
 * @file Bill.cpp
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#include <numeric>
#include "Bill.h"

namespace splitbill {

SplitBill Bill::Total() {
  if (lines_.empty()) {
    // Empty bill
    return SplitBill(Money(0, total_amount_.GetCurrency()), Money(0, total_amount_.GetCurrency()));
  }

  // Get the lines that refer to usage and those that don't.
  std::vector<BillLine> usage_lines;
  std::vector<BillLine> general_lines;
  SortLinesBySplit(lines_, usage_lines, general_lines);

  // Sanity check to ensure there's something to work with for the rest of the process.
  // These lines have amount and tax rate 0 so they won't affect calculations
  if (usage_lines.empty()) {
    BillLine dummy_usage;
    dummy_usage.split = true;
    usage_lines.push_back(dummy_usage);
  }
  if (general_lines.empty()) {
    BillLine dummy_general;
    dummy_general.split = false;
    general_lines.push_back(dummy_general);
  }

  // Apply tax
  const std::vector<BillLine> all_lines = ApplyTax(lines_);
  usage_lines = ApplyTax(usage_lines);
  general_lines = ApplyTax(general_lines);

  // Tote the lines
  const std::vector<Money> all_amounts = GetAmounts(all_lines);
  const std::vector<Money> usage_amounts = GetAmounts(usage_lines);
  const std::vector<Money> general_amounts = GetAmounts(general_lines);
  const Money all_total =
      std::accumulate(all_amounts.cbegin(), all_amounts.cend(), Money(0, total_amount_.GetCurrency()));
  const Money usage_total =
      std::accumulate(usage_amounts.cbegin(), usage_amounts.cend(), Money(0, total_amount_.GetCurrency()));
  const Money general_total =
      std::accumulate(general_amounts.cbegin(), general_amounts.cend(), Money(0, total_amount_.GetCurrency()));

  return SplitBill(usage_total, general_total);
}

#define FOR_DAY_IN_PERIOD(day_var, period) for (boost::gregorian::date day_var = period.begin(); day_var <= period.last(); day_var += boost::gregorian::date_duration(1))

std::vector<splitbill::BillPortion> Bill::Split(const boost::gregorian::date_period &period,
                                                const std::vector<PersonPeriod> &person_periods,
                                                const std::vector<std::string> &people) {
  if (people.empty()) {
    return std::vector<splitbill::BillPortion>();
  }

  SplitBill totals = Total();
  const Money usage_part = totals.GetUsageTotal() / period.length().days();

  // First pass: Determine how many parts each day must be split into.
  std::map<boost::gregorian::date, unsigned int> day_parts;
  unsigned int everyone_usage_days = 0;
  FOR_DAY_IN_PERIOD(day, period) {
    unsigned int day_part_count = 0;
    for (const auto &person_period : person_periods) {
      if (person_period.GetPeriod().contains(day)) {
        day_part_count++;
      }
    }
    if (day_part_count == 0) {
      // No people were set for this period, so assume everyone
      day_part_count = people.size();
      everyone_usage_days++;
    }
    day_parts.insert({day, day_part_count});
  }
  // Handle days where no person was present
  const Money everyone_usage = (usage_part / people.size()) * everyone_usage_days;

  // Second pass: divide the amount into chunks for each day, then divide those chunks into parts for
  // each user present on that day.  The end result of this is that presence on a given day costs a
  // certain amount.
  std::map<boost::gregorian::date, Money> day_usage_amounts;
  FOR_DAY_IN_PERIOD(day, period) {
    day_usage_amounts.insert({day, usage_part / day_parts.at(day)});
  }

  // Third pass: total each user's contribution.
  const Money general_chunk = totals.GetGeneralTotal() / people.size();
  std::vector<splitbill::BillPortion> portions;
  portions.reserve(people.size());
  for (const auto &person : people) {
    Money person_usage = everyone_usage;
    for (const auto &person_period : person_periods) {
      if (person_period.GetName() != person) {
        continue;
      }
      FOR_DAY_IN_PERIOD(day, period) {
        if (!person_period.GetPeriod().contains(day)) {
          continue;
        }
        person_usage = person_usage + day_usage_amounts.at(day);
      }
    }
    portions.emplace_back(person, person_usage, general_chunk);
  }

  return portions;
}

bool Bill::IsValid(ValidationError &error) {
  // Check line total equals bill total, with tax applied
  SplitBill totals = Total();
  if (std::abs((totals.GetTotal() - GetTotalAmount()).GetValue()) >= total_amount_.GetCurrency().error_margin()) {
    error = ValidationError::kLineSumNotTotal;
    return false;
  }

  error = ValidationError::kValid;
  return true;
}

std::vector<Money> Bill::GetAmounts(const std::vector<BillLine> &lines) {
  std::vector<Money> amounts;
  amounts.reserve(lines.size());
  for (const auto &line : lines) {
    amounts.push_back(line.amount);
  }

  return amounts;
}

std::vector<BillLine> Bill::ApplyTax(const std::vector<BillLine> &lines) {
  std::vector<BillLine> taxed_lines;
  taxed_lines.reserve(lines.size());
  for (const auto &line : lines) {
    BillLine taxed_line(line);
    taxed_line.amount = line.amount * (line.tax_rate + 1);
    taxed_lines.push_back(taxed_line);
  }

  return taxed_lines;
}

void Bill::SortLinesBySplit(const std::vector<BillLine> &lines,
                            std::vector<BillLine> &split_lines,
                            std::vector<BillLine> &not_split_lines) {
  split_lines.clear();
  not_split_lines.clear();

  for (const auto &line : lines) {
    if (line.split) {
      split_lines.push_back(line);
    } else {
      not_split_lines.push_back(line);
    }
  }
}

} // splitbill
