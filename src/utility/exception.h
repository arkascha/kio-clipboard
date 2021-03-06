/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file Declaratin of class Exception
 * This is a header-only library, no additional implementation file exists, thus no linking is required.
 * @see Exception
 * @author Christian Reiner
 */

#ifndef UTILITY_EXCEPTION_H
#define UTILITY_EXCEPTION_H

#include <QtCore>
#include <kio/global.h>
#include <kio/job.h>

using namespace KIO;
namespace KIO_CLIPBOARD
{
  /*!
   * class Exception
   * @brief Convenience wrapper for standard exception
   * This class serves as a header-only library (no object file to be linked). 
   * @author Christian Reiner
   */
  class Exception
    : public QtConcurrent::Exception
  {
    public:
      inline Exception ( Error _code, const QString &_text=0 ) : code(_code),        text(_text) { };
      inline Exception ( int   _code, const QString &_text=0 ) : code(Error(_code)), text(_text) { };
      inline Exception ( KJob* _job ) : code(Error(_job->error())), text(_job->errorString()) { delete _job; };
      inline ~Exception () throw() {};
      inline Exception* clone () const { return new Exception(*this); };
      inline void       raise () const { throw *this; };
      inline void       debug () const
      {
        kDebug() << QString("ERROR %1: %2").arg(code).arg(text);
        kDebug() << "Backtrace:\n" << kBacktrace();
      };
    private:
      const Error   code;
      const QString text;
    public:
      inline const Error    getCode () { return this->code; };
      inline const QString& getText () { return this->text; };
  }; // class Exception

}; // namespace CRE

#endif // UTILITY_EXCEPTION_H
