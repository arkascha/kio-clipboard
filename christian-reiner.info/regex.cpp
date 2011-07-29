/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <QRegExp>
#include <kdebug.h>
#include <klocalizedstring.h>
#include "christian-reiner.info/exception.h"
#include "christian-reiner.info/regex.h"

CRI::regExPool CRI::loadRegExPool()
{
  regExPool _pool;
  _pool["protocol"] = QRegExp("[a-z]{3,9}");
  _pool["user"]     = QRegExp("[^@]+@{1}");
  _pool["domain"]   = QRegExp("([-a-z0-9]+.)+[a-z]{2,6}");
  _pool["port"]     = QRegExp("[0-9]+");
  _pool["path"]     = QRegExp("(/[^/]+)+/?");
  _pool["query"]    = QRegExp("([&?][a-z0-9_]=[^&])*");
  _pool["anchor"]   = QRegExp("\\w+");
  _pool["uri"]      = QRegExp ( QString("^((%1):/{1,3})(%2)?(%3)(:%4)?(%5(%6)?(#%7)?)?$")
                                        .arg(_pool["protocol"].pattern())
                                        .arg(_pool["user"].pattern())
                                        .arg(_pool["domain"].pattern())
                                        .arg(_pool["port"].pattern())
                                        .arg(_pool["path"].pattern())
                                        .arg(_pool["query"].pattern())
                                        .arg(_pool["anchor"].pattern()) );
  // validate each regex
  foreach ( const QString& _key, _pool.keys() )
    if ( ! _pool[_key].isValid() )
    {
      const char* _err = "failed to load regex engine because of invalid rexex '%1': %2";
      kDebug() << QString(_err).arg(_key).arg(_pool[_key].pattern());
      throw CRI::Exception ( Error(ERR_SLAVE_DEFINED),
                             i18n(_err).arg(_key).arg(_pool[_key].pattern()) );
    }
  return _pool;
} // CRI::loadRegExPool
