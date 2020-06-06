/**
 * @file BillTest.cpp
 *
 * @author dankeenan 
 * @date 6/4/20
 */

#include <gtest/gtest.h>
#include "../../src/lib/Bill.h"

namespace splitbill_test {

static const float RESULT_ERROR_MARGIN = 0.01;

class BillTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Unsplit, taxed
    line_unsplit_taxed_.name = "Unsplit, taxed";
    line_unsplit_taxed_.amount = 30.95;
    line_unsplit_taxed_.split = false;
    line_unsplit_taxed_.tax_rate = 0.07;
    bill_.AddLine(line_unsplit_taxed_);

    // Unsplit, untaxed
    line_unsplit_untaxed_.name = "Unsplit, untaxed";
    line_unsplit_untaxed_.amount = 30.95;
    line_unsplit_untaxed_.split = false;
    line_unsplit_untaxed_.tax_rate = 0;
    bill_.AddLine(line_unsplit_untaxed_);

    // Split, taxed
    line_split_taxed_.name = "Split, taxed";
    line_split_taxed_.amount = 40.95;
    line_split_taxed_.split = true;
    line_split_taxed_.tax_rate = 0.07;
    bill_.AddLine(line_split_taxed_);

    // Split, untaxed
    line_split_untaxed_.name = "Split, untaxed";
    line_split_untaxed_.amount = 40.95;
    line_split_untaxed_.split = true;
    line_split_untaxed_.tax_rate = 0;
    bill_.AddLine(line_split_untaxed_);
  }

  splitbill::BillLine line_unsplit_taxed_;
  splitbill::BillLine line_unsplit_untaxed_;
  splitbill::BillLine line_split_taxed_;
  splitbill::BillLine line_split_untaxed_;

  splitbill::Bill bill_;
};

/**
 * Lines are added properly
 */
TEST_F(BillTest, LinesAdded) {
  splitbill::Bill bill;

  ASSERT_EQ(bill.GetLines().size(), 0) << "New bill is not empty";
  for (unsigned int i = 0; i < 3; i++) {
    splitbill::BillLine line{"Test Line" + std::to_string(i)};
    bill.AddLine(line);
    ASSERT_EQ(bill.GetLines().size(), i + 1) << "Bill line not added";
    ASSERT_EQ(bill.GetLines().at(i), line) << "Added line is not the same";
  }
}

/**
 * Lines are removed preserving order
 */
TEST_F(BillTest, LinesRemoved) {
  splitbill::Bill bill;

  // Add lines
  splitbill::BillLine line_1{"Test Line 1"};
  bill.AddLine(line_1);
  splitbill::BillLine line_2{"Test Line 2"};
  bill.AddLine(line_2);
  splitbill::BillLine line_3{"Test Line 3"};
  bill.AddLine(line_3);

  bill.RemoveLine(line_2);
  ASSERT_EQ(bill.GetLines().size(), 2) << "Bill line not removed";
  ASSERT_EQ(bill.GetLines().at(0), line_1) << "Bill line at start reordered";
  ASSERT_EQ(bill.GetLines().at(1), line_3) << "Bill line at end reordered";
}

/**
 * Totals calculated properly
 */
TEST_F(BillTest, Total) {
  splitbill::SplitBill totals = bill_.Total();

  // Hand-calculated results
  ASSERT_NEAR(totals.GetGeneralTotal(), 64.07, RESULT_ERROR_MARGIN) << "General total calculated incorrectly";
  ASSERT_NEAR(totals.GetUsageTotal(), 84.77, RESULT_ERROR_MARGIN) << "Usage total calculated incorrectly";
  ASSERT_NEAR(totals.GetTotal(), 148.84, RESULT_ERROR_MARGIN) << "Total calculated incorrectly";
}

/**
 * Bill split properly
 */
TEST_F(BillTest, Split) {
  const std::string person_one_week = "Person present one week";
  const splitbill::PersonPeriod persion_period_one_week(person_one_week, "2020-1-8", "2020-1-14");
  const std::string person_two_weeks = "Person present two weeks";
  const splitbill::PersonPeriod person_period_two_weeks_1(person_two_weeks, "2020-1-7", "2020-1-12");
  const splitbill::PersonPeriod person_period_two_weeks_2(person_two_weeks, "2020-1-21", "2020-1-27");
  const std::string person_absent = "Person absent";
  const std::vector<std::string> people{person_one_week, person_two_weeks, person_absent};
  const std::vector<splitbill::PersonPeriod> periods{
      persion_period_one_week, person_period_two_weeks_1, person_period_two_weeks_2
  };
  const std::vector<splitbill::BillPortion> portion_list = bill_.Split("2020-1-1", "2020-1-31", periods, people);
  ASSERT_EQ(portion_list.size(), 3) << "Not all people have bill portions";

  // Need to extract the results to compare them
  std::unordered_map<std::string, splitbill::BillPortion> portions;
  for (const auto &portion : portion_list) {
    portions.insert({portion.GetName(), portion});
  }
  // Assert all people are in the results
  ASSERT_NO_THROW((void) portions.at(person_one_week)) << "Bill missing person (one week)";
  ASSERT_NO_THROW((void) portions.at(person_two_weeks)) << "Bill missing person (two weeks)";
  ASSERT_NO_THROW((void) portions.at(person_absent)) << "Bill missing absent person";

  // Hand-calculated results
  EXPECT_NEAR(portions.at(person_one_week).GetTotal()
                  + portions.at(person_two_weeks).GetTotal()
                  + portions.at(person_absent).GetTotal(),
              148.84, RESULT_ERROR_MARGIN)
            << "Split total not equal to actual total";
  EXPECT_NEAR(portions.at(person_one_week).GetUsageTotal(), 26.89, RESULT_ERROR_MARGIN)
            << "Person (one week) usage calculated incorrectly";
  EXPECT_NEAR(portions.at(person_one_week).GetGeneralTotal(), 21.36, RESULT_ERROR_MARGIN)
            << "Person (one week) general calculated incorrectly";
  EXPECT_NEAR(portions.at(person_one_week).GetTotal(), 48.25, RESULT_ERROR_MARGIN)
            << "Person (one week) total calculated incorrectly";
  EXPECT_NEAR(portions.at(person_two_weeks).GetUsageTotal(), 43.30, RESULT_ERROR_MARGIN)
            << "Person (two weeks) usage calculated incorrectly";
  EXPECT_NEAR(portions.at(person_two_weeks).GetGeneralTotal(), 21.36, RESULT_ERROR_MARGIN)
            << "Person (two weeks) general calculated incorrectly";
  EXPECT_NEAR(portions.at(person_two_weeks).GetTotal(), 64.65, RESULT_ERROR_MARGIN)
            << "Person (two weeks) total calculated incorrectly";
  EXPECT_NEAR(portions.at(person_absent).GetUsageTotal(), 14.58, RESULT_ERROR_MARGIN)
            << "Person (absent) usage calculated incorrectly";
  EXPECT_NEAR(portions.at(person_absent).GetGeneralTotal(), 21.36, RESULT_ERROR_MARGIN)
            << "Person (absent) general calculated incorrectly";
  EXPECT_NEAR(portions.at(person_absent).GetTotal(), 35.94, RESULT_ERROR_MARGIN)
            << "Person (absent) total calculated incorrectly";
}

TEST_F(BillTest, IsValid) {
  splitbill::ValidationError error;

  // Lines add up
  bill_.SetTotalAmount(148.84);
  EXPECT_TRUE(bill_.IsValid(error));
  EXPECT_EQ(error, splitbill::ValidationError::VALID);

  // Lines don't add up
  bill_.SetTotalAmount(150);
  EXPECT_FALSE(bill_.IsValid(error));
  EXPECT_EQ(error, splitbill::ValidationError::LINE_SUM_NOT_TOTAL);
}

}
