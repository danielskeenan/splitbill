/**
 * @file Bill.cpp
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#include "Bill.h"

namespace splitbill {

SplitBill Bill::Total() {
  if (lines_.size() == 0) {
    // Empty bill
    return SplitBill(0, 0);
  }

  // Get the lines that refer to usage and those that don't.
  BillLineList usage_lines;
  BillLineList general_lines;
  SortLinesBySplit(lines_, usage_lines, general_lines);

  // Sanity check to ensure there's something to work with for the rest of the process.
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
  const BillLineList all_lines = ApplyTax(lines_);
  usage_lines = ApplyTax(usage_lines);
  general_lines = ApplyTax(general_lines);

  // Tote the lines
  const std::vector<Money> all_amounts = GetAmounts(all_lines);
  const std::vector<Money> usage_amounts = GetAmounts(usage_lines);
  const std::vector<Money> general_amounts = GetAmounts(general_lines);
  const Money all_total = Accumulate(all_amounts);
  const Money usage_total = Accumulate(usage_amounts);
  const Money general_total = Accumulate(general_amounts);

  SplitBill split_bill(usage_total, general_total);

  return split_bill;
}

#define FOR_DAY_IN_PERIOD(day_var, period) for (gregorian::date day_var = period.begin(); day_var <= period.last(); day_var += gregorian::date_duration(1))

std::vector<splitbill::BillPortion> Bill::Split(const gregorian::date_period &period,
                                                const std::vector<PersonPeriod> &person_periods,
                                                const std::vector<std::string> &people) {
  if (people.empty()) {
    return std::vector<splitbill::BillPortion>();
  }

  SplitBill totals = Total();
  const Money usage_part = totals.GetMoneyUsageTotal() / period.length().days();

  // First pass: Determine how many parts each day must be split into.
  std::map<gregorian::date, unsigned int> day_parts;
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
  const Money everyone_usage = everyone_usage_days * (usage_part / people.size());

  // Second pass: divide the amount into chunks for each day, then divide those chunks into parts for
  // each user present on that day.  The end result of this is that presence on a given day costs a
  // certain amount.
  std::map<gregorian::date, Money> day_usage_amounts;
  FOR_DAY_IN_PERIOD(day, period) {
    day_usage_amounts.insert({day, usage_part / day_parts.at(day)});
  }

  // Third pass: total each user's contribution.
  const Money general_chunk = totals.GetMoneyGeneralTotal() / people.size();
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
        person_usage += day_usage_amounts.at(day);
      }
    }
    portions.emplace_back(person, person_usage, general_chunk);
  }

  return portions;
}

bool Bill::IsValid(ValidationError &error) {
  // Check line total equals bill total, with tax applied
  SplitBill totals = Total();
  if (std::abs(totals.GetTotal() - GetTotalAmount()) >= 0.01) {
    error = ValidationError::LINE_SUM_NOT_TOTAL;
    return false;
  }

  error = ValidationError::VALID;
  return true;
}

std::vector<Money> Bill::GetAmounts(const BillLineList &lines) {
  std::vector<Money> amounts;
  amounts.reserve(lines.size());
  for (const auto &line : lines) {
    amounts.push_back(line.amount);
  }

  return amounts;
}

std::vector<BillLine> Bill::ApplyTax(const BillLineList &lines) {
  std::vector<BillLine> taxed_lines;
  taxed_lines.reserve(lines.size());
  for (const auto &line : lines) {
    BillLine taxed_line(line);
    taxed_line.amount = line.amount * (line.tax_rate + 1);
    taxed_lines.push_back(taxed_line);
  }

  return taxed_lines;
}

void Bill::SortLinesBySplit(const BillLineList &lines, BillLineList &split_lines, BillLineList &not_split_lines) {
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

Money Bill::Accumulate(const std::vector<Money> &items) {
  // Use Kahan summation to compensate for lost precision
  Money sum = 0.0;
  Money c = 0.0;
  for (const auto &item : items) {
    const Money y = item - c;
    const Money t = sum + y;
    c = (t - sum) - y;
    sum = t;
  }

  return sum;
}

} // splitbill
