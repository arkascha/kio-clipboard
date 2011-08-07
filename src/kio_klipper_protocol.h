/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KIO_KLIPPER_PROTOCOL_H
#define KIO_KLIPPER_PROTOCOL_H

#include <QtCore/QMutex>
#include <QString>
#include <QMap>
#include <kio/global.h>
#include <kio/udsentry.h>
#include "kio_protocol.h"
#include "wrapper/kio_clipboard_wrapper.h"
#include "klipper/kio_clipboard_wrapper_klipper.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{
  /**
   * The central definition of a clipboard protocol that can communicate with the clipboard application 'klipper' as used in KDE4 desktops. 
   */
  class KIOKlipperProtocol
    : public QObject
    , public KIOProtocol
  {
    Q_OBJECT
    private:
    protected:
      const UDSEntry     toUDSEntry ( );
      const UDSEntryList toUDSEntryList ( );
    public:
      KIOKlipperProtocol ( const QByteArray &pool, const QByteArray &app, QObject* parent=0 );
      virtual ~KIOKlipperProtocol();
    public:
      void copy     ( const KUrl& src, const KUrl& dest, int permissions, JobFlags flags );
      void del      ( const KUrl& url, bool isfile );
      void get      ( const KUrl& url );
      void listDir  ( const KUrl& url );
      void mimetype ( const KUrl& url );
      void mkdir    ( const KUrl& url, int permissions );
      void put      ( const KUrl& url, int permissions, KIO::JobFlags flags );
      void stat     ( const KUrl& url );
      void symlink  ( const QString& target, const KUrl& dest, KIO::JobFlags flags );
//      void setModificationTime ( const KUrl& url, const QDateTime& mtime );
  }; // class KIOKlipperProtocol

} // namespace KIO_CLIPBOARD

#endif // KIO_KLIPPER_PROTOCOL_H