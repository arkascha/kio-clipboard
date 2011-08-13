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
#include <QString>
#include <QMap>
#include <kio/global.h>
#include <kio/forwardingslavebase.h>
#include <kio/udsentry.h>
#include "clipboards/klipper/klipper_frontend.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{
  static       int     C_mappingNameCardinality  = 1;
  static const int     C_mappingNameLength       = 60;
  static const QString C_mappingNamePattern      = "%1[%2]:%3";

  /**
   * This class implements something like a 'meta slave', a slave that acts as a proxy to other, specialized slaves.
   * All available clipboards, auto detected or manually specified are presented, access to any one is forwarded
   * to a specialized slave implementation (this is done by the interface, not directly by this class)
   */
  class KIOClipboardProtocol
    : public ForwardingSlaveBase
  {
    private:
      QMap<KUrl,const ClipboardFrontend*> m_nodes;
    protected:
      const UDSEntry     toUDSEntry ();
      const UDSEntryList toUDSEntryList ();
      const ClipboardFrontend* findClipboardByUrl ( const KUrl& url );
      const QList<const ClipboardFrontend*> detectClipboards ( );
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
