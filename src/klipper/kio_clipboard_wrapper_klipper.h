/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KIO_CLIPBOARD_WRAPPER_KLIPPER_H
#define KIO_CLIPBOARD_WRAPPER_KLIPPER_H

#include "klipper/dbus_client_klipper.h"
#include "wrapper/kio_clipboard_wrapper.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{

  class KIOClipboardWrapperKlipper
      : public KIOClipboardWrapper
  {
    private:
      DBusClientKlipper* m_dbus;
    protected:
    public:
      KIOClipboardWrapperKlipper ( const KUrl& url, const QString& name );
      ~KIOClipboardWrapperKlipper ( );
      inline const ClipboardType type     ( ) const { return ClipboardType(KLIPPER); };
      inline const QString       protocol ( ) const { return "protocol"; };
      QString     getClipboardEntry   ( );
      QStringList getClipboardEntries ( );
      void pushEntry ( const QString& entry );
      void delEntry  ( const KUrl& url );
  }; // class ClipboardWrapperKlipper

} // namespace KIO_CLIPBOARD

#endif // KIO_CLIPBOARD_WRAPPER_KLIPPER_H