/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file
 * Implements the methods of the generic class ClipboardFrontend.
 * @see ClipboardFrontend
 * @author Christian Reiner
 */

#include <math.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmimetype.h>
#include <kio/netaccess.h>
#include <kshareddatacache.h>
#include <kdatetime.h>
#include "utility/exception.h"
#include "protocol/kio_clipboard_protocol.h"
#include "clipboard/clipboard_frontend.h"
#include "clipboard/klipper/klipper_frontend.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/*!
 * KIO_CLIPBOARD::tokenizeUrl
 * @brief A simple convenience function that breaks a given URL in tokens
 * @param url url to be tokenized
 * @return list of string tokens
 * @author Christian Reiner
 */
const QStringList KIO_CLIPBOARD::tokenizeUrl ( const KUrl& url )
{
  QRegExp exp ("^([a-z0-9_]+):/([^/]+)?(/.+)?$");
  if ( exp.exactMatch(url.url()) )
  {
    QStringList tokens = exp.capturedTexts();
    // remove entry 0, holding the _whole_ matched string
    if ( ! tokens.isEmpty() )
      tokens.removeFirst();
    kDebug() << tokens.size() << tokens;
    return tokens;
  }
  throw Exception ( Error(ERR_MALFORMED_URL), url.url() );
} // ClipboardFrontend::tokenizeUrl

//==========

/*!
 * ClipboardFrontend::detectClipboards
 * @brief Autodetection of available clipboards for cases where this is possible
 * @return list of pointers to created ClipboardFrontend objects
 * - local clipboard applications:
 * - - 'klipper': detects presence on DBus
 * - remote clipboard services:
 * - - 'pastebin': test connection to the server
 * @author Christian Reiner
 */
QList<const ClipboardFrontend*> ClipboardFrontend::detectClipboards ( )
{
  kDebug();
  QList<const ClipboardFrontend*> _clipboards;
  // strategy: for clipboards available on DBus we ask org.freedesktop.DBus for such a service
  DBusClient dbus ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "" );
  _clipboards << KlipperFrontend::detectClipboards ( dbus );
  kDebug() << "detected" << _clipboards.count() << "available clipboards";
  return _clipboards;
} // ClipboardFrontend::detectClipboards

/*!
 * ClipboardFrontend::ClipboardFrontend
 * @brief Constructor of generic class ClipboardFrontend
 * @param url base url of the clipboard node
 * @param name visible name of the clipboard node
 * @author Christian Reiner
 */
ClipboardFrontend::ClipboardFrontend ( const KUrl& url, const QString& name )
  : m_url  ( url )
  , m_name ( name )
  , m_mappingNameCardinality ( KIO_CLIPBOARD::C_mappingNameCardinality ) 
  , m_mappingNameLength      ( KIO_CLIPBOARD::C_mappingNameLength )
  , m_mappingNamePattern     ( KIO_CLIPBOARD::C_mappingNamePattern )
{
  kDebug();
  m_cache = new KSharedDataCache ( QString("kio-clipboard-%1").arg(m_name), 100*1024*1024, 256 );
  m_cache->setEvictionPolicy ( KSharedDataCache::EvictOldest );
  m_nodes = new NodeList;
} // ClipboardFrontend::ClipboardFrontend

/*!
 * ClipboardFrontend::~ClipboardFrontend
 * @brief Destructor of the generic class ClipboardFrontend
 * @author Christian Reiner
 */
ClipboardFrontend::~ClipboardFrontend ( )
{
  kDebug();
  clearNodes();
  delete m_cache;
  delete m_nodes;
} // ClipboardFrontend::~ClipboardFrontend

/*!
 * ClipboardFrontend::toUDSEntry
 * @brief A clipboard itself as presented to the outside worlds (the KIO system).
 * @return UDSEntry object describing the clipboard node itself
 * @author: Christian Reiner
 */
const UDSEntry ClipboardFrontend::toUDSEntry ( ) const
{
  kDebug();
  UDSEntry _entry;
  _entry.insert( UDSEntry::UDS_NAME,              m_name );
  _entry.insert( UDSEntry::UDS_MIME_TYPE,         "inode/directory" );
  _entry.insert( UDSEntry::UDS_URL,               m_url.url() );
//  _entry.insert( UDSEntry::UDS_ACCESS,            S_IRUSR | S_IRGRP | S_IROTH );
  _entry.insert( UDSEntry::UDS_ACCESS,            0700 );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,         S_IFDIR );
//  _entry.insert( UDSEntry::UDS_CREATION_TIME,     KDateTime::currentLocalDateTime().toTime_t() );
  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME, KDateTime::currentLocalDateTime().toTime_t() );
  _entry.insert( UDSEntry::UDS_ACCESS_TIME,       KDateTime::currentLocalDateTime().toTime_t() );
  return _entry;
} // KIOClipboardProtocol::registerClipboard

/*!
 * ClipboardFrontend::toUDSEntryList
 * @brief Lists all nodes (clipboard entries) as present in this wrapper.
 * @return UDSEntryList describing the clipboards entries
 * @author: Christian Reiner
 */
const UDSEntryList ClipboardFrontend::toUDSEntryList ( ) const
{
  UDSEntryList _entries;
  foreach ( const NodeWrapper* _entry, m_nodes->toMap() )
    _entries << _entry->toUDSEntry();
  kDebug() << "listing" << _entries.count() << "entries";
  return _entries;
} // ClipboardFrontend::toUDSEntryList

/*!
 * ClipboardFrontend::refreshNodes
 * @brief Refreshes all entries contained in the clipboard. 
 * Since we buffer the clipboards entries in a map of objects we have to refresh that map from time to time. 
 * Since changes in clipboards often can only be detected by polling this has to be done quite frequent. 
 * @author: Christian Reiner
 */
void ClipboardFrontend::refreshNodes ( )
{
  kDebug();
  // ask the specialised client for the entries
  QStringList _entries = getClipboardEntries ( );
  // update global name cardinality, important to construct names with correct cardinality of their name prefix indexes
  m_mappingNameCardinality = QString("%1").arg(_entries.count()).size();
  kDebug() << QString("set mapping cardinality to %1 (length of numeric index)").arg(C_mappingNameCardinality);
  // strategy: clear the nodes before (re-) populating it
  m_nodes->clear();
  // now populate the nodes, one entry after another
  int _index = 0;
  foreach ( const QString& _entry, _entries )
  {
    NodeWrapper* _node = new NodeWrapper ( this, ++_index, _entry );
    m_nodes->insert ( _node->name(), _node );
  }
  kDebug() << "populated fresh set of nodes with" << m_nodes->size ( ) << "entries";
  // store refreshed list into shared cache
  m_cache->clear ( );
  m_cache->insert ( "nodes", m_nodes->toJSON() );
} // ClipboardFrontend::refreshNodes

/*!
 * ClipboardFrontend::clearNodes
 * @brief: Clears all entries contained in the clipboard. 
 * Removes all nodes that act as representations for clipboard entries.
 * Typically called upon cleanup of right before re-populating by reading all available entries again. 
 * @author: Christian Reiner
 */
void ClipboardFrontend::clearNodes ( )
{
  kDebug();
  foreach ( const NodeWrapper* const& _entry, m_nodes->values() )
    delete _entry;
} // ClipboardFrontend::clearNodes

/*!
 * ClipboardFrontend::findNodeByUrl
 * @brief Identification of a clipboards entry by its url
 * Since clipboard entries do not have clear and simple file names we require a failure proof identification of each entry. 
 * (Note that the index of an entry can easily change when the clipboards content is changed...)
 * So we define a unique URL for each node and match all requests lateer against this url. 
 * @author: Christian Reiner
 */
const NodeWrapper* ClipboardFrontend::findNodeByUrl ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  // note: we might have a fresh process...
  // TODO: find some way of really caching thing between different processes (KDED ? )
  if ( m_nodes->isEmpty() )
    refreshNodes ( );
  // look for the matching entry
  QString _name = url.fileName();
  if ( m_nodes->contains ( _name ) )
    return m_nodes->value ( _name );
  // no matching element found ?!?
  throw Exception ( Error(ERR_DOES_NOT_EXIST), url.prettyUrl() );
} // ClipboardFrontend::findNodeByUrl
