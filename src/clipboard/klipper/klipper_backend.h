/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KLIPPER_BACKEND_H
#define KLIPPER_BACKEND_H

#include "client/dbus/dbus_client.h"
#include "clipboard/clipboard_backend.h"

class QDBusInterface;

using namespace KIO;
namespace KIO_CLIPBOARD
{

  /**
   * The part of the wrapper that implements the specific routines required to use the clipboard
   * Communication is done via a separate, abstract client, in this case a DBus client
   * So this class maps specific routines onto the abstract client methods
   */
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