/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file
 * Declares the generic class ClipboardFrontend.
 * @see ClipboardFrontend
 * @author Christian Reiner
 */

#ifndef CLIPBOARD_FRONTEND_H
#define CLIPBOARD_FRONTEND_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <kio/global.h>
#include <kio/jobclasses.h>
#include <kio/udsentry.h>
#include "utility/regex.h"
#include "clipboard/klipper/klipper_backend.h"
#include "node/node_wrapper.h"
#include "node/node_list.h"

class KSharedDataCache;

using namespace KIO;
namespace KIO_CLIPBOARD
{
  /*!
   * tokenizeUrl
   * @brief Simple convenience function that breaks a given URL into three tokens
   * @author: Christian Reiner
   */
  const QStringList tokenizeUrl ( const KUrl& url);

  /*!
   * ClipboardType
   * @brief Technical type of a clipboard
   * An enumeration of clipboard types as known, handled and implemented. 
   * Each type of clipboard has it's own specific ways of how to be handled. 
   * Therefore it is very important to clearly identify that type upon usage. 
   * - KLIPPER: local clipboard application used as a standard in KDE4 desktops
   * @author: Christian Reiner
   */
  enum ClipboardType  { KLIPPER };

  /*!
   * class ClipboardFrontend
   * @brief This class acts as a proxy layer between frontend and backend.
   * - frontend: the slave/protocol offering the backend inside the KIO system
   * - backend: the clipboard itself as accessible by some interface
   * The access to the clipboard strictly depends on the interface that is used to access features and content. 
   * This class acts as a wrapper to make the clipboard accessible in a generic way, so it is some kind of application proxy. 
   * @author: Christian Reiner
   */
  class ClipboardFrontend
  {
    private:
      const KUrl    m_url;
      const QString m_name;
    protected:
      int               m_mappingNameCardinality;
      const int         m_mappingNameLength;
      const QString&    m_mappingNamePattern;
      ClipboardBackend* m_backend;
      KSharedDataCache* m_cache;
      NodeList*         m_nodes;
    public:
      static QList<const ClipboardFrontend*> detectClipboards ( );
      ClipboardFrontend ( const KUrl& url, const QString& name );
      ~ClipboardFrontend ( );
      virtual const ClipboardType type     ( ) const = 0;
      virtual const QString       protocol ( ) const = 0;
      virtual const int           limit    ( ) const = 0;
      inline const KUrl&    url                    ( ) const { return this->m_url; };
      inline const QString& name                   ( ) const { return this->m_name; };
      inline const int      mappingNameCardinality ( ) const { return m_mappingNameCardinality; };
      inline const int      mappingNameLength      ( ) const { return m_mappingNameLength; };
      inline const QString& mappingNamePattern     ( ) const { return m_mappingNamePattern; };
      inline int countNodes ( ) { return m_nodes->size(); };
      const NodeWrapper*    findNodeByUrl  ( const KUrl& url );
      const UDSEntry        toUDSEntry     ( ) const;
      const UDSEntryList    toUDSEntryList ( ) const;
      virtual QString       getClipboardEntry   ( ) = 0;
      virtual QString       getClipboardEntry   ( int index ) = 0;
      virtual QStringList   getClipboardEntries ( ) = 0;
      virtual void          pushEntry ( const QString& entry ) = 0;
      virtual void          delEntry  ( const KUrl& url      ) = 0;
      void refreshNodes ( );
      void clearNodes ( );
  }; // class ClipboardFrontend

} // namespace KIO_CLIPBOARD

#endif // CLIPBOARD_FRONTEND_H