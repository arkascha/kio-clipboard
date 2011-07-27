#ifndef CRI_REGEX_H
#define CRI_REGEX_H

#include <QMap>
#include <QString>

namespace CRI
{
  typedef QMap<QString,QRegExp> regExPool;
  regExPool loadRegExPool();
}

#endif