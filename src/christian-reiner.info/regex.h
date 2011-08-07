/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef CRI_REGEX_H
#define CRI_REGEX_H

#include <QMap>
#include <QString>

// TODO: turn this 'thing' into a head-only library

namespace CRI
{
  typedef QMap<QString,QRegExp> regExPool;
  regExPool loadRegExPool();
}

#endif