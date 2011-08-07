/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <math.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmimetype.h>
#include <kio/netaccess.h>
#include "christian-reiner.info/exception.h"
#include "kio_clipboard_protocol.h"
#include "kio_clipboard_wrapper.h"
#include "wrapper/kio_node_wrapper.h"
#include "klipper/kio_clipboard_wrapper_klipper.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/**
 * A simple convenience function that breaks a given URL in tokens
 * TODO: this might be better based closer on KUrl and its methods, only the path has to be broken manually
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
  throw CRI::Exception ( Error(ERR_MALFORMED_URL), url.url() );
} // KIOClipboardWrapper::tokenizeUrl

//==========

/**
 * Each representation of a clipboard is identified by its name and an URL to access it. 
 */
KIOClipboardWrapper::KIOClipboardWrapper ( const KUrl& url, const QString& name )
  : m_url      ( url )
  , m_name     ( name )
{
  kDebug();
} // KIOClipboardWrapper::KIOClipboardWrapper

/**
 * Any ceanups required go here. 
 */
KIOClipboardWrapper::~KIOClipboardWrapper()
{
  kDebug();
  clearNodes();
} // KIOClipboardWrapper::~KIOClipboardWrapper

/**
 * A clipboard itself as presented to the outside worlds (the KIO system)
 */
const UDSEntry KIOClipboardWrapper::toUDSEntry ( ) const
{
  kDebug();
  UDSEntry _entry;
  _entry.insert( UDSEntry::UDS_NAME,         m_name );
  _entry.insert( UDSEntry::UDS_MIME_TYPE,    "inode/directory" );
  _entry.insert( UDSEntry::UDS_URL,          m_url.url() );
  _entry.insert( UDSEntry::UDS_ACCESS,       S_IRUSR | S_IRGRP | S_IROTH );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,    S_IFDIR );
  return _entry;
} // KIOClipboardProtocol::registerClipboard

/**
 * A list of all nodes (clipboard entries) as present in this wrapper. 
 */
const UDSEntryList KIOClipboardWrapper::toUDSEntryList ( ) const
{
  UDSEntryList _entries;
  foreach ( const KIONodeWrapper* _entry, m_nodes )
    _entries << _entry->toUDSEntry();
  kDebug() << "listing" << _entries.count() << "entries";
  return _entries;
} // KIOClipboardWrapper::toUDSEntryList

/**
 * Since we buffer the clipboards entries in a map of objects we have to refresh that map from time to time
 * Since changes in clipboards often can only be detected by polling this has to be done quite frequent
 */
void KIOClipboardWrapper::refreshNodes ()
{
  kDebug();
  // strategy: clear the nodes before (re-) populating it
  m_nodes.clear();
  // ask the specialised client for the entries
  QStringList _entries = getClipboardEntries ();
  // update global name cardinality, important to construct names with correct cardinality of their name prefix indexes
  C_mappingNameCardinality = QString("%1").arg(_entries.count()).size();
  kDebug() << QString("set mapping cardinality to %1 (length of numeric index)").arg(C_mappingNameCardinality);
  // now populate the nodes, one entry after another
  int _index = 0;
  foreach ( const QString& _entry, _entries )
  {
    KIONodeWrapper* _node = new KIONodeWrapper ( ++_index, _entry );
    m_nodes.insert ( _node->name(), _node );
  }
  kDebug() << QString("populated fresh set of nodes with %1 entries").arg(m_nodes.size());
} // KIOClipboardWrapper::refreshNodes

/**
 * Removes all nodes that act as representations for clipboard entries.
 * Typically called upon cleanup of right before re-populating by reading all available entries again. 
 */
void KIOClipboardWrapper::clearNodes ( )
{
  kDebug();
  foreach ( const KIONodeWrapper* const& _entry, m_nodes.values() )
    delete _entry;
} // KIOClipboardWrapper::clearNodes

/**
 * Since clipboard entries do not have clear and simple file names we require a failure proof identification of each entry
 * (Note that the index of an entry can easily change when the clipboars content is changed...)
 * So we define a unique URL for each node and match all requests lateer against this url
 */
const KIONodeWrapper* KIOClipboardWrapper::findNodeByUrl ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  // note: we might have a fresh process...
  // TODO: find some way of really caching thing between different processes (KDED ? )
  if ( m_nodes.isEmpty() )
    refreshNodes ( );
  // look for the matching entry
  QString _name = url.fileName();
  if ( m_nodes.contains(_name) )
    return m_nodes[_name];
  // no matching element found ?!?
  throw CRI::Exception ( Error(ERR_DOES_NOT_EXIST), url.prettyUrl() );
} // KIOClipboardWrapper::findNodeByUrl

