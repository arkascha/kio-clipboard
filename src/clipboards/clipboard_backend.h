/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author: arkascha $
 * $Revision: 81 $
 * $Date: 2011-08-13 13:08:50 +0200 (Sat, 13 Aug 2011) $
 */
#ifndef CLIPBOARD_BACKEND_H
#define CLIPBOARD_BACKEND_H

#include <QObject>
#include "clients/dbus/dbus_client.h"

class QDBusInterface;

using namespace KIO;
namespace KIO_CLIPBOARD
{

  class ClipboardBackend
    : public QObject
  {
    Q_OBJECT
    public:
      ClipboardBackend ( QObject* parent=0 );
      ~ClipboardBackend ( );
    public slots:
      virtual void        clearClipboardContents  () = 0;
      virtual void        clearClipboardHistory   () = 0;
      virtual QString     getClipboardContents    () = 0;
      virtual QStringList getClipboardHistoryMenu () = 0;
      virtual QString     getClipboardHistoryItem ( int index ) = 0;
      virtual void        setClipboardContents    ( const QString& _entry ) = 0;
      virtual void        setClipboardHistory     ( const QStringList& _entries ) = 0;
  }; // class ClipboardBackend

} // namespace KIO_CLIPBOARD

#endif // CLIPBOARD_BACKEND_H