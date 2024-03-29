import argparse
import os
import sys
import urllib.request
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from string import Template
from typing import Dict


@dataclass
class CurrencyInfo:
    iso_4217_code: str
    minor_units: int


def read_currency_info(data: ET.ElementTree) -> Dict[str, CurrencyInfo]:
    """
    Parse the XML file from the ISO.
    :param data:
    :return: A dict keyed by currency code
    """
    root = data.getroot()
    currency_info = {}
    for currency_data in root.find('CcyTbl').findall('CcyNtry'):
        try:
            code = currency_data.find('Ccy').text
            minor_units = int(currency_data.find('CcyMnrUnts').text)
            currency_info[code] = CurrencyInfo(iso_4217_code=code,
                                               minor_units=minor_units)
        except ValueError:
            # There's a lot of odd data, including places without currency e.g. Antarctica.
            continue
        except AttributeError:
            continue

    return currency_info


def generate(currency_info: Dict[str, CurrencyInfo]) -> str:
    """
    Generate the Currency.h file.
    :param currency_info:
    :return:
    """
    template = Template('''
/**
 * @file
 * This file is automatically generated by generate_currency_map.py.
 * Manual modifications will be lost!
 */

#ifndef SPLITBILL_INCLUDE_LIB_CURRENCY_H
#define SPLITBILL_INCLUDE_LIB_CURRENCY_H

#include <cmath>
#include <string>
#include <unordered_map>

namespace splitbill {

class Currency {
 public:
  struct Info {
    std::string iso_4217_code;
    unsigned int minor_units;
    
    [[nodiscard]] unsigned int multiplier() const {
      return std::pow(10, minor_units);
    }
    
    [[nodiscard]] double error_margin() const {
      return 1.0 / std::pow(10, minor_units + 1);
    }

    [[nodiscard]] bool operator==(const Info &rhs) const {
      return iso_4217_code == rhs.iso_4217_code;
    }

    [[nodiscard]] bool operator!=(const Info &rhs) const {
      return !(rhs == *this);
    }
  };

  enum class Code {
    $currency_codes
  };

  static inline const Info &Get(Code code) {
    return kCurrencyInfo.at(code);
  }

  static inline const Info &Get(const std::string &code) {
    return Get(kCodeMap.at(code));
  }

 private:
  static inline const std::unordered_map<Code, Info> kCurrencyInfo{
      $currency_info
  };
  static inline const std::unordered_map<std::string, Code> kCodeMap{
      $code_map_codes
  };
};

} // splitbill

#endif //SPLITBILL_INCLUDE_LIB_CURRENCY_H

    '''.strip() + os.linesep)
    currency_code_enums = (',' + os.linesep + (' ' * 4)).join(currency_info.keys())
    currency_info_members = (',' + os.linesep + (' ' * 6)).join(
        [
            '{{Code::{code}, {{"{code}", {minor_units}}}}}'.format(
                code=currency.iso_4217_code, minor_units=currency.minor_units)
            for currency in currency_info.values()
        ]
    )
    code_map_members = (',' + os.linesep + (' ' * 6)).join(
        [
            '{{"{code}", Code::{code}}}'.format(code=currency_code)
            for currency_code in currency_info.keys()
        ]
    )

    return template.substitute({
        'currency_codes': currency_code_enums,
        'currency_info': currency_info_members,
        'code_map_codes': code_map_members,
    })


def verify(data: ET.ElementTree) -> bool:
    """
    Verify the XML file is well formed.
    :param data:
    :return:
    """
    root = data.getroot()
    if root.tag != 'ISO_4217':
        print('This is not a ISO 4217 file.', file=sys.stderr)
        return False
    elif len(root.findall('CcyTbl')) != 1:
        print('This file does not contain a table or it is in the wrong format.', file=sys.stderr)
        return False
    elif len(root.find('CcyTbl').findall('CcyNtry')) == 0:
        print('This file contains no countries.', file=sys.stderr)
        return False

    return True


def get_iso_data() -> ET.ElementTree:
    """
    Download the XML file from the internet.
    :return:
    """
    with urllib.request.urlopen(
            'https://www.six-group.com/dam/download/financial-information/data-center/iso-currrency/lists/list-one.xml') as r:
        return ET.parse(r)


if __name__ == '__main__':
    argparser = argparse.ArgumentParser(description='Generate the currency code header file')
    argparser.add_argument('--from-file', '-i', required=False,
                           help='Use the given file instead of fetching it from the internet')
    argparser.add_argument('--out', '-o', type=argparse.FileType('wt', encoding='UTF-8'), default=sys.stdout,
                           help='Path to output file (defaults to STDOUT)')
    args = argparser.parse_args()

    if not args.from_file:
        data = get_iso_data()
    else:
        with open(args.from_file, 'rt', encoding='UTF-8') as source_file:
            data = ET.parse(source_file)

    if not verify(data):
        print('The data is not valid.  Cannot continue.', file=sys.stderr)
        exit(1)
    currency_info = read_currency_info(data)
    args.out.write(generate(currency_info))
    exit(0)
