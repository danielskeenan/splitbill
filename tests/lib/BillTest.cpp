/**
 * @file BillTest.cpp
 *
 * @author dankeenan 
 * @date 6/4/20
 */

#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <lib/Bill.h>

using namespace splitbill;

static const double kResultErrorMargin = Currency::Get(splitbill::Currency::Code::USD).error_margin();

class BillTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Unsplit, taxed
    line_unsplit_taxed_.name = "Unsplit, taxed";
    line_unsplit_taxed_.amount = Money(30.95, Currency::Code::USD);
    line_unsplit_taxed_.split = false;
    line_unsplit_taxed_.tax_rate = 0.07;
    bill_.AddLine(line_unsplit_taxed_);

    // Unsplit, untaxed
    line_unsplit_untaxed_.name = "Unsplit, untaxed";
    line_unsplit_untaxed_.amount = Money(30.95, Currency::Code::USD);
    line_unsplit_untaxed_.split = false;
    line_unsplit_untaxed_.tax_rate = 0;
    bill_.AddLine(line_unsplit_untaxed_);

    // Split, taxed
    line_split_taxed_.name = "Split, taxed";
    line_split_taxed_.amount = Money(40.95, Currency::Code::USD);
    line_split_taxed_.split = true;
    line_split_taxed_.tax_rate = 0.07;
    bill_.AddLine(line_split_taxed_);

    // Split, untaxed
    line_split_untaxed_.name = "Split, untaxed";
    line_split_untaxed_.amount = Money(40.95, Currency::Code::USD);
    line_split_untaxed_.split = true;
    line_split_untaxed_.tax_rate = 0;
    bill_.AddLine(line_split_untaxed_);
  }

  BillLine line_unsplit_taxed_;
  BillLine line_unsplit_untaxed_;
  BillLine line_split_taxed_;
  BillLine line_split_untaxed_;

  Bill bill_;
};

/**
 * Lines are added properly
 */
TEST_F(BillTest, LinesAdded) {
  Bill bill;

  ASSERT_EQ(bill.GetLines().size(), 0) << "New bill is not empty";
  for (unsigned int i = 0; i < 3; i++) {
    BillLine line;
    line.name = "Test Line" + std::to_string(i);
    bill.AddLine(line);
    ASSERT_EQ(bill.GetLines().size(), i + 1) << "Bill line not added";
    ASSERT_EQ(bill.GetLines().at(i), line) << "Added line is not the same";
  }
}

/**
 * Lines are removed preserving order
 */
TEST_F(BillTest, LinesRemoved) {
  Bill bill;

  // Add lines
  BillLine line_1;
  line_1.name = "Test Line 1";;
  bill.AddLine(line_1);
  BillLine line_2;
  line_2.name = "Test Line 2";;
  bill.AddLine(line_2);
  BillLine line_3;
  line_3.name = "Test Line 3";;
  bill.AddLine(line_3);

  bill.RemoveLine(line_2);
  ASSERT_EQ(bill.GetLines().size(), 2) << "Bill line not removed";
  ASSERT_EQ(bill.GetLines().at(0), line_1) << "Bill line at start reordered";
  ASSERT_EQ(bill.GetLines().at(1), line_3) << "Bill line at end reordered";
}

/**
 * Lines are updated properly
 */
TEST_F(BillTest, LinesUpdated) {
  Bill bill;

  // Add lines
  BillLine line_1;
  line_1.name = "Test Line 1";;
  line_1.amount = Money(51.00, Currency::Code::USD);
  bill.AddLine(line_1);
  BillLine line_2;
  line_2.name = "Test Line 2";;
  line_2.amount = Money(52.00, Currency::Code::USD);
  bill.AddLine(line_2);
  BillLine line_3;
  line_3.name = "Test Line 3";;
  line_3.amount = Money(53.00, Currency::Code::USD);
  bill.AddLine(line_3);

  ASSERT_EQ(bill.GetLine(1), line_2) << "Line not inserted correctly";
  line_2.amount = Money(42.00, Currency::Code::USD);
  bill.UpdateLine(1, line_2);
  EXPECT_EQ(bill.GetLine(0).amount, 51.00) << "Wrong line updated";
  EXPECT_EQ(bill.GetLine(1).amount, 42.00) << "Line not updated";
  EXPECT_EQ(bill.GetLine(2).amount, 53.00) << "Wrong line updated";
}

/**
 * Totals calculated properly
 */
TEST_F(BillTest, Total) {
  SplitBill totals = bill_.Total();

  // Hand-calculated results
  ASSERT_NEAR(totals.GetGeneralTotal().GetValue(), 64.07, kResultErrorMargin)
                << "General total calculated incorrectly";
  ASSERT_NEAR(totals.GetUsageTotal().GetValue(), 84.77, kResultErrorMargin) << "Usage total calculated incorrectly";
  ASSERT_NEAR(totals.GetTotal().GetValue(), 148.83, kResultErrorMargin) << "Total calculated incorrectly";
}

/**
 * Bill split properly
 */
TEST_F(BillTest, Split) {
  const std::string person_one_week = "Person present one week";
  const PersonPeriod person_period_one_week(person_one_week, "2020-1-8", "2020-1-14");
  const std::string person_two_weeks = "Person present two weeks";
  const PersonPeriod person_period_two_weeks_1(person_two_weeks, "2020-1-7", "2020-1-12");
  const PersonPeriod person_period_two_weeks_2(person_two_weeks, "2020-1-21", "2020-1-27");
  const std::string person_absent = "Person absent";
  const std::vector<std::string> people{person_one_week, person_two_weeks, person_absent};
  const std::vector<PersonPeriod> periods{
      person_period_one_week, person_period_two_weeks_1, person_period_two_weeks_2
  };
  const std::vector<BillPortion> portion_list = bill_.Split("2020-1-1", "2020-1-31", periods, people);
  ASSERT_EQ(portion_list.size(), 3) << "Not all people have bill portions";

  // Need to extract the results to compare them
  std::unordered_map<std::string, BillPortion> portions;
  for (const auto &portion : portion_list) {
    portions.insert({portion.GetName(), portion});
  }
  // Assert all people are in the results
  ASSERT_NO_THROW((void) portions.at(person_one_week)) << "Bill missing person (one week)";
  ASSERT_NO_THROW((void) portions.at(person_two_weeks)) << "Bill missing person (two weeks)";
  ASSERT_NO_THROW((void) portions.at(person_absent)) << "Bill missing absent person";

  // Hand-calculated results
  EXPECT_NEAR((portions.at(person_one_week).GetTotal()
      + portions.at(person_two_weeks).GetTotal()
      + portions.at(person_absent).GetTotal()).GetValue(),
              148.83, kResultErrorMargin)
            << "Split total not equal to actual total";
  EXPECT_NEAR(portions.at(person_one_week).GetUsageTotal().GetValue(), 26.89, kResultErrorMargin)
            << "Person (one week) usage calculated incorrectly";
  EXPECT_NEAR(portions.at(person_one_week).GetGeneralTotal().GetValue(), 21.36, kResultErrorMargin)
            << "Person (one week) general calculated incorrectly";
  EXPECT_NEAR(portions.at(person_one_week).GetTotal().GetValue(), 48.24, kResultErrorMargin)
            << "Person (one week) total calculated incorrectly";
  EXPECT_NEAR(portions.at(person_two_weeks).GetUsageTotal().GetValue(), 43.29, kResultErrorMargin)
            << "Person (two weeks) usage calculated incorrectly";
  EXPECT_NEAR(portions.at(person_two_weeks).GetGeneralTotal().GetValue(), 21.36, kResultErrorMargin)
            << "Person (two weeks) general calculated incorrectly";
  EXPECT_NEAR(portions.at(person_two_weeks).GetTotal().GetValue(), 64.65, kResultErrorMargin)
            << "Person (two weeks) total calculated incorrectly";
  EXPECT_NEAR(portions.at(person_absent).GetUsageTotal().GetValue(), 14.58, kResultErrorMargin)
            << "Person (absent) usage calculated incorrectly";
  EXPECT_NEAR(portions.at(person_absent).GetGeneralTotal().GetValue(), 21.36, kResultErrorMargin)
            << "Person (absent) general calculated incorrectly";
  EXPECT_NEAR(portions.at(person_absent).GetTotal().GetValue(), 35.94, kResultErrorMargin)
            << "Person (absent) total calculated incorrectly";
}

TEST_F(BillTest, IsValid) {
  ValidationError error;

  // Lines add up
  bill_.SetTotalAmount(Money(148.83, Currency::Code::USD));
  EXPECT_TRUE(bill_.IsValid(error));
  EXPECT_EQ(error, ValidationError::kValid);

  // Lines don't add up
  bill_.SetTotalAmount(Money(150.00, Currency::Code::USD));
  EXPECT_FALSE(bill_.IsValid(error));
  EXPECT_EQ(error, ValidationError::kLineSumNotTotal);
}
