/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KLIPPER_FRONTEND_H
#define KLIPPER_FRONTEND_H

#include "clipboard/klipper/klipper_backend.h"
#include "clipboard/clipboard_frontend.h"
#include "clipboard/clipboard_backend.h"

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
  class KlipperFrontend
      : public ClipboardFrontend
  {
    private:
    protected:
      ClipboardBackend* m_backend;
    public:
      static QList<const ClipboardFrontend*> detectClipboards ( DBusClient& dbus );
      KlipperFrontend ( const KUrl& url, const QString& name );
      ~KlipperFrontend ( );
      inline const ClipboardType type     ( ) const { return ClipboardType(KLIPPER); };
      inline const QString       protocol ( ) const { return QString::fromLatin1("klipper"); };
      inline const int           limit    ( ) const { return 32*1024; }; // TODO: 32kB: this _seems_ to be a hard limit, is there a configuration for this somewhere ?
      QString     getClipboardEntry   ( );
      QString     getClipboardEntry   ( int index );
      QStringList getClipboardEntries ( );
      void pushEntry ( const QString& entry );
      void delEntry  ( const KUrl& url );
  }; // class ClipboardWrapperKlipper

} // namespace KIO_CLIPBOARD

#endif // KLIPPER_FRONTEND_H