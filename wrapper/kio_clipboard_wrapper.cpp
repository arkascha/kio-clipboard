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

QList<const KIOClipboardWrapper*> KIOClipboardWrapper::detectClipboards ( )
{
  kDebug();
  QList<const KIOClipboardWrapper*> _clipboards;
  // strategy: for clipboards available on DBus we ask org.freedesktop.DBus for such a service
  DBusClient dbus;
  dbus.setupInterface ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "" );
  dbus.call ( "ListNames" );
  const QStringList _names = dbus.convertReturnValue(dbus.result().first(),QVariant::StringList).toStringList();
  // now add entries one by one
  foreach (const QString& _name, _names )
  {
    if ( "org.kde.klipper"==_name )
    {
      kDebug() << "detected available clipboard of type 'KLIPPER' with url 'klipper:/'";
      _clipboards << new KIOClipboardWrapperKlipper ( KUrl("klipper:/"), "klipper" );
    }
  }
  kDebug() << "detected" << _clipboards.count() << "available clipboards";
  return _clipboards;
}

//==========

KIOClipboardWrapper::KIOClipboardWrapper ( const KUrl& url, const QString& name )
  : m_url      ( url )
  , m_name     ( name )
{
  kDebug();
} // KIOClipboardWrapper::KIOClipboardWrapper

KIOClipboardWrapper::~KIOClipboardWrapper()
{
  kDebug();
  clearNodes();
} // KIOClipboardWrapper::~KIOClipboardWrapper

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

const UDSEntryList KIOClipboardWrapper::toUDSEntryList ( ) const
{
  UDSEntryList _entries;
  foreach ( const KIONodeWrapper* _entry, m_nodes )
    _entries << _entry->toUDSEntry();
  kDebug() << "listing" << _entries.count() << "entries";
  return _entries;
} // KIOClipboardWrapper::toUDSEntryList

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
    m_nodes.insert ( _node->prettyIndex(), _node );
  }
  kDebug() << QString("populated fresh set of nodes with %1 entries").arg(m_nodes.size());
} // KIOClipboardWrapper::refreshNodes

void KIOClipboardWrapper::clearNodes ( )
{
  kDebug();
  foreach ( const KIONodeWrapper* const& _entry, m_nodes.values() )
    delete _entry;
} // KIOClipboardWrapper::clearNodes

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
