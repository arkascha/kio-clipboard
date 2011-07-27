/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $HeadURL$
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KIO_KLIPPER_PROTOCOL_H
#define KIO_KLIPPER_PROTOCOL_H

#include <QtCore/QMutex>
#include <kio/global.h>
#include <kio/slavebase.h>
#include "kio/udsentry.h"
#include "wrapper/kio_clipboard_wrapper.h"
#include "klipper/kio_clipboard_wrapper_klipper.h"

namespace KIO_CLIPBOARD
{
  class KIOKlipperProtocol
    : public QObject
    , public SlaveBase
    , protected KIOClipboardWrapperKlipper
  {
    Q_OBJECT
    private:
    protected:
      const UDSEntry rootUDSEntry ( );
    public:
      KIOKlipperProtocol ( const QByteArray &pool, const QByteArray &app, QObject* parent=0 );
      virtual ~KIOKlipperProtocol();
    public:
      //void copy     ( const KUrl& src, const KUrl& dest, int permissions, JobFlags flags );
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