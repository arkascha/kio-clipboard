/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KIO_CLIPBOARD_WRAPPER_H
#define KIO_CLIPBOARD_WRAPPER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <kio/global.h>
#include <kio/jobclasses.h>
#include <kio/udsentry.h>
#include "christian-reiner.info/regex.h"
#include "wrapper/kio_node_wrapper.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{
  const QStringList tokenizeUrl ( const KUrl& url);

  enum ClipboardType  { KLIPPER };

  class KIOClipboardWrapper
  {
    private:
      const KUrl           m_url;
      const QString        m_name;
    protected:
    public:
      QMap<QString,const KIONodeWrapper*> m_nodes;
    protected:
      const KIONodeWrapper* findNodeByUrl ( const KUrl& url );
      void refreshNodes ( );
      void clearNodes ( );
    public:
      static QList<const KIOClipboardWrapper*> detectClipboards ( );
      KIOClipboardWrapper ( const KUrl& url, const QString& name );
      ~KIOClipboardWrapper ( );
      virtual const ClipboardType type     ( ) const = 0;
      virtual const QString       protocol ( ) const = 0;
      inline const KUrl&    url  ( ) const { return this->m_url; };
      inline const QString& name ( ) const { return this->m_name; };
      const UDSEntry     toUDSEntry     ( ) const;
      const UDSEntryList toUDSEntryList ( ) const;
      virtual QString     getClipboardEntry   ( ) = 0;
      virtual QStringList getClipboardEntries ( ) = 0;
      virtual void        pushEntry ( const QString& entry ) = 0;
      virtual void        delEntry  ( const KUrl& url      ) = 0;
  }; // class KIOClipboardWrapper

} // namespace KIO_CLIPBOARD

#endif // KIO_CLIPBOARD_WRAPPER_H