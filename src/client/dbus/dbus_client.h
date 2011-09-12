/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef DBUS_CLIENT_H
#define DBUS_CLIENT_H

#include <QString>
#include <QStringList>
#include <QtDBus/QDBusConnection>
#include <kio/global.h>
#include <KUrl>

using namespace KIO;
namespace KIO_CLIPBOARD
{

  /**
   * A generic DBus client
   * This class acts as a simple bridge between the separate object representations inside DBus and c++
   * Basic error handling is provided and a few convenience methods
   * This class is usually not used by itself in a raw manner, instead it has to be derived by a
   * specialized client implementing the real functions as implemented by the DBus service
   */
  class DBusClient
  {
    public:
      DBusClient ( const QString& service, const QString& path, const QString& interface );
      ~DBusClient ( );
    private:
    protected:
      QList<QVariant> m_result;
      QDBusInterface* m_interface;
    public:
      QList<QVariant> result ( );
      int             resultSize ( );
      QVariant& convertReturnValue ( QVariant &variant, QVariant::Type _t );
      void      call ( const QString method, const QVariant & arg1 = QVariant(),
                                             const QVariant & arg2 = QVariant(),
                                             const QVariant & arg3 = QVariant(),
                                             const QVariant & arg4 = QVariant(),
                                             const QVariant & arg5 = QVariant(),
                                             const QVariant & arg6 = QVariant(),
                                             const QVariant & arg7 = QVariant(),
                                             const QVariant & arg8 = QVariant() );
  }; // class DBusClient

} // namespace KIO_CLIPBOARD

#endif // DBUS_CLIENT_H
