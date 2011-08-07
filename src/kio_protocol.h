/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author: arkascha $
 * $Revision: 27 $
 * $Date: 2011-07-31 18:49:38 +0200 (Sun, 31 Jul 2011) $
 */
#ifndef KIO_PROTOCOL_H
#define KIO_PROTOCOL_H

#include <QString>
#include <kio/global.h>
#include <kio/slavebase.h>
#include <kio/udsentry.h>
#include "wrapper/kio_node_wrapper.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{
  class KIOClipboardWrapper;

  /**
   * This is an abstract base common to all specialized kio-clipboard protocols.
   * It defines generic methods that this way are kept equal throuhout all specialists, therefore it acts as an interface definition.
   * The hook to the non-generic world of specific clipboards is the pointer to a clipboard object handed over to the constructor. 
   */
  class KIOProtocol
    : public SlaveBase
  {
    private:
    protected:
      KIOClipboardWrapper* const m_clipboard;
      virtual const UDSEntry     toUDSEntry ( ) = 0;
      virtual const UDSEntryList toUDSEntryList ( ) = 0;
    public:
      KIOProtocol ( const QByteArray& pool, const QByteArray& app, KIOClipboardWrapper* const clipboard );
      ~KIOProtocol ( );
    protected:
//      void copyToFile             ( const KIONodeWrapper* src, const KUrl& dest );
//      void copyToFile_Reference   ( const KIONodeWrapper* src, const KUrl& dest );
//      void copyToFile_Content     ( const KIONodeWrapper* src, const KUrl& dest );
      void copyFromFile           ( const KUrl&    src,  const KUrl& dest );
      void copyFromFile_Reference ( const KUrl&    url );
      void copyFromFile_Reference ( const QString& path );
      void copyFromFile_Content   ( const KUrl&    url );
    public:
      //virtual void copy     ( const KUrl& src, const KUrl& dest, int permissions, JobFlags flags ) = 0;
      virtual void del      ( const KUrl& url, bool isfile ) = 0;
      virtual void get      ( const KUrl& url ) = 0;
      virtual void listDir  ( const KUrl& url ) = 0;
      virtual void mimetype ( const KUrl& url ) = 0;
      virtual void mkdir    ( const KUrl& url, int permissions ) = 0;
      virtual void put      ( const KUrl& url, int permissions, KIO::JobFlags flags ) = 0;
      virtual void stat     ( const KUrl& url ) = 0;
      virtual void symlink  ( const QString& target, const KUrl& dest, KIO::JobFlags flags ) = 0;
//      virtual void setModificationTime ( const KUrl& url, const QDateTime& mtime ) = 0;
  }; // class KIOProtocol

} // namespace KIO_CLIPBOARD

#endif // KIO_PROTOCOL_H