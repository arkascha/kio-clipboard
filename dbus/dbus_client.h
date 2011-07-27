/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $HeadURL$
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef DBUS_CLIENT_H
#define DBUS_CLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtDBus/QDBusConnection>
#include <kio/global.h>
#include <KUrl>

// ToDo: move dbus clients into a separate thread

using namespace KIO;
namespace KIO_CLIPBOARD
{

  class DBusClient
    : public QObject
  {
    Q_OBJECT
    public:
      DBusClient ( QObject* _parent=0 );
      ~DBusClient ( );
    private slots:
      void cleanup ( );
    protected slots:
    public slots:
    private:
    protected:
      QList<QVariant> m_result;
      QDBusInterface* m_interface;
    public:
      QList<QVariant> result ( );
      int             resultSize ( );
      void      setupInterface ( const QString& service, const QString& path, const QString& interface );
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
