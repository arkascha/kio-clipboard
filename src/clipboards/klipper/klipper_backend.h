/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KLIPPER_BACKEND_H
#define KLIPPER_BACKEND_H

#include "clients/dbus/dbus_client.h"
#include "clipboards/clipboard_backend.h"

class QDBusInterface;

using namespace KIO;
namespace KIO_CLIPBOARD
{

  class KlipperBackend
    : public ClipboardBackend
    , public DBusClient
  {
    Q_OBJECT
    public:
      KlipperBackend ( QObject* parent=0 );
      ~KlipperBackend ( );
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

#endif // KLIPPER_BACKEND_H