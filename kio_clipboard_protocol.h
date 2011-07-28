/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KIO_CLIPBOARD_PROTOCOL_H
#define KIO_CLIPBOARD_PROTOCOL_H

#include <QtCore/QMutex>
#include <kio/global.h>
#include <kio/forwardingslavebase.h>
#include "kio/udsentry.h"

#include "wrapper/kio_clipboard_wrapper.h"
#include "klipper/kio_clipboard_wrapper_klipper.h"

namespace KIO_CLIPBOARD
{
  static const int     C_mappingNameCardinality = 50;
  static const QString C_mappingNamePattern     = "%1[%2]:%3";

  typedef QMap<KUrl, UDSEntry> ClipboardPool;

  class KIOClipboardProtocol
    : public ForwardingSlaveBase
  {
    private:
      ClipboardPool m_clipboards;
    protected:
      void registerClipboard ( ClipboardType type, const KUrl& base );
      const UDSEntry rootUDSEntry ();
      const UDSEntryList listOfUDSEntries ();
      const UDSEntry& findClipboardByUrl ( const KUrl& url );
    public:
      KIOClipboardProtocol ( const QByteArray &pool, const QByteArray &app );
      virtual ~KIOClipboardProtocol();
    public:
      bool rewriteUrl ( const KUrl& oldUrl, KUrl& newUrl );
      void del        ( const KUrl& url, bool isfile );
      void get        ( const KUrl& url );
      void listDir    ( const KUrl& url );
      void mimetype   ( const KUrl& url );
      void mkdir      ( const KUrl& url, int permissions );
      void put        ( const KUrl& url, int permissions, JobFlags flags );
      void stat       ( const KUrl& url );
      void symlink    ( const QString& target, const KUrl& dest, JobFlags flags );
  }; // class KIOClipboardProtocol

} // namespace KIO_CLIPBOARD

#endif // KIO_CLIPBOARD_PROTOCOL_H
