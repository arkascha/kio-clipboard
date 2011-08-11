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

  /**
   * This class implements a wrapper around the 'klipper' appplication
   * 'klipper' is the standard clipboard application as used inside KDE4 desktop sessions
   * Communication is done via klippers DBus interface which appears to be pretty stable
   * So all required actions are mapped onto DBus calls, the replies are represented as internal structures again
   * This interface is not really 'random-access', but good enough for our purpose
   * The actual communication via DBus is handled by a DBus client defined externally
   */
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
      inline const QString       protocol ( ) const { return QString::fromLatin1("klipper"); };
      inline const int           limit    ( ) const { return 32*1024; }; // TODO: 32kB: this _seems_ to be a hard limit, is there a configuration for this somewhere ?
      QString     getClipboardEntry   ( );
      QStringList getClipboardEntries ( );
      void pushEntry ( const QString& entry );
      void delEntry  ( const KUrl& url );
  }; // class ClipboardWrapperKlipper

} // namespace KIO_CLIPBOARD

#endif // KIO_CLIPBOARD_WRAPPER_KLIPPER_H