/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef DBUS_CLIENT_KLIPPER_H
#define DBUS_CLIENT_KLIPPER_H

#include "dbus/dbus_client.h"

class QDBusInterface;

using namespace KIO;
namespace KIO_CLIPBOARD
{

  class DBusClientKlipper
    : public DBusClient
  {
    Q_OBJECT
    public:
      DBusClientKlipper ( QObject* parent=0 );
      ~DBusClientKlipper ();
    public slots:
      void        clearClipboardContents  ();
      void        clearClipboardHistory   ();
      QString     getClipboardContents    ();
      QStringList getClipboardHistoryMenu ();
      QString     getClipboardHistoryItem ( int index );
      void        setClipboardContents    ( const QString& _entry );
      void        setClipboardHistory     ( const QStringList& _entries );
  }; // class DBusWrapperKlipper

} // namespace KIO_CLIPBOARD

#endif // DBUS_CLIENT_KLIPPER_H