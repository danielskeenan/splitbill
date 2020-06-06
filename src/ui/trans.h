/**
 * @file trans.h
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#ifndef SPLITBILL_SRC_UI_TRANS_H_
#define SPLITBILL_SRC_UI_TRANS_H_

#include <boost/locale.hpp>
#include <QString>

#define _(...) QString::fromStdString(boost::locale::translate(__VA_ARGS__).str())

#endif //SPLITBILL_SRC_UI_TRANS_H_
