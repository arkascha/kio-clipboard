/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <stdlib.h>
#include <unistd.h>
#include <KUrl>
#include <KMimeType>
#include <kdebug.h>
#include <kdeversion.h>
#include "christian-reiner.info/exception.h"
#include "kio_clipboard_protocol.h"
#include "clipboards/clipboard_frontend.h"

// Kdebug::Block is only defined from KDE-4.6.0 on
// we wrap it cause this appears to be the only requirement for KDE-4.6
#if KDE_IS_VERSION(4,6,0)
#define MY_KDEBUG_BLOCK(token) KDebug::Block myBlock(token);
#else
#define MY_KDEBUG_BLOCK(token) kDebug()<<token;
#endif

using namespace KIO;
using namespace KIO_CLIPBOARD;

/**
 * This constructor is responsible to present all available clipboards by means of internal presentation.
 * Therefore he tries auto detection as offered by the specialized protocols
 * and reads those clipboards from the configuration that were specified manually
 * TODO: read manually specified clipboards from configuration
 */
KIOClipboardProtocol::KIOClipboardProtocol( const QByteArray &_pool, const QByteArray &_app )
  : ForwardingSlaveBase ( "clipboard", _pool, _app )
{
  MY_KDEBUG_BLOCK ( "<slave setup>" );
  kDebug();
  try
  {
    QList<const ClipboardFrontend*> _clipboards = ClipboardFrontend::detectClipboards ( );
    // register each detected clipboard
    foreach ( const ClipboardFrontend* _entry, _clipboards )
    {
      const KUrl _url (QString("clipboard:/%1").arg(_entry->name()));
      kDebug() << QString("registering clipboard of type '%1' at url %2").arg(_entry->type()).arg(_url.prettyUrl());
      m_nodes.insert ( _url, _entry );
    }
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

/**
 * Any cleanup required comes here, currently nothing...
 */
KIOClipboardProtocol::~KIOClipboardProtocol()
{
  MY_KDEBUG_BLOCK ( "<slave shutdown>" );
  kDebug();
  try
  {
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

/**
 * Generates a plain UDSEntry object that describes this protocol itself, that is its base folder.
 */
const UDSEntry KIOClipboardProtocol::toUDSEntry ()
{
  kDebug();
  UDSEntry _entry;
  _entry.clear();
  _entry.insert( UDSEntry::UDS_NAME,              QString::fromLatin1("."));
  _entry.insert( UDSEntry::UDS_FILE_TYPE,         S_IFDIR);
  _entry.insert( UDSEntry::UDS_ACCESS,            0700);
  _entry.insert( UDSEntry::UDS_MIME_TYPE,         QString::fromLatin1("inode/directory"));
  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME, KDateTime::currentLocalDateTime().toTime_t() );
  return _entry;
} // KIOClipboardProtocol::toUDSEntry

/**
 * Generates a list of UDSEntries that describe all nodes (clipboards) as being available
 */
const UDSEntryList KIOClipboardProtocol::toUDSEntryList ()
{
  UDSEntryList _entries;
  foreach ( const ClipboardFrontend* _entry, m_nodes )
//  foreach ( const ClipboardFrontend* const& _entry, m_nodes.values() )
    _entries << _entry->toUDSEntry();
  kDebug() << "listing" << _entries.count() << "entries";
  return _entries;
} // KIOClipboardProtocol::toUDSEntryList

/**
 * convenience routine to identify a node (a clipboard) when referenced by its URL
 */
const KIO_CLIPBOARD::ClipboardFrontend* KIOClipboardProtocol::findClipboardByUrl ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  if ( m_nodes.contains(url) )
    return m_nodes[url];
  throw CRI::Exception ( Error(ERR_DOES_NOT_EXIST), url.prettyUrl() );
} // KIOClipboardProtocol::findClipboardByUrl

//======================

/**
 * This assists the calling interface to redirect to the specialized slave/protocol
 * when accessing a specific clipboard amongst those offered as available by this meta slave
 * We do this by matching the URL that was called against those stored as adresses in the clipboard nodes
 */
bool KIOClipboardProtocol::rewriteUrl ( const KUrl& oldUrl, KUrl& newUrl )
{
  // convert clipboard:/klipper/02 to klipper:/02
  kDebug() << oldUrl.url();
  try
  {
    KUrl _clipboard_url;
    QStringList _tokens = KIO_CLIPBOARD::tokenizeUrl(oldUrl);
    // token holds n+1 entries (0-n), where 0 holds the _whole_ url
    switch ( _tokens.count() )
    {
      case 1: // just a plain clipboard:/ was requested
        // we should not come here...
        kDebug() << "rewriting attempt of url pointing to this object";
        throw CRI::Exception ( Error(ERR_UNSUPPORTED_ACTION), oldUrl.prettyUrl() );
        break;
      case 2: // something like clipboard:/klipper was requested
        newUrl = findClipboardByUrl(oldUrl)->url();
        break;
      case 3: // full url like clipboard:/klipper/007 was requested
        _clipboard_url = KUrl ( QString("%1:/%2").arg(_tokens[2]).arg(_tokens[3]) );
        newUrl = findClipboardByUrl(_clipboard_url)->url();
        newUrl.addPath(_tokens[2]);
        break;
      default:
        kDebug() << "rewriting attempt produced more than the expected 3 url tokens";
        throw CRI::Exception ( Error(ERR_MALFORMED_URL), oldUrl.url() );
    }
    kDebug() << "rewriting to:" << newUrl.url();
    return TRUE;
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::rewriteUrl

//==========

/**
 * We deny all attempts to delete a clipboard
 */
void KIOClipboardProtocol::del (const KUrl &url, bool isfile )
{
  MY_KDEBUG_BLOCK ( "<del>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // del() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.prettyUrl() );
    }
    else
    {
      ForwardingSlaveBase::del ( url, isfile );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::del

/**
 * We deny all attempts to get a clipboard itself
 */
void KIOClipboardProtocol::get ( const KUrl &url )
{
  MY_KDEBUG_BLOCK ( "<get>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // get() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.prettyUrl() );
    }
    else
    {
      ForwardingSlaveBase::get ( url );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::get

/**
 * We list all available clipboards as a list of UDSEntries
 */
void KIOClipboardProtocol::listDir ( const KUrl& url )
{
  MY_KDEBUG_BLOCK ( "<listDir>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      totalSize ( m_nodes.size() );
      listEntries ( toUDSEntryList() );
      finished ();
    }
    else
    {
      ForwardingSlaveBase::listDir ( url );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::listDir

/**
 * We simply claim a directory mimetype for all available clipboards
 * This makes them being presented as folders which is what we want
 * TODO: check what happens if we use something like "inode/vde-kde-network" or similar
 */
void KIOClipboardProtocol::mimetype ( const KUrl& url )
{
  MY_KDEBUG_BLOCK ( "<mimetype>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      mimeType ( "inode/directory" );
      finished ();
    }
    else
    {
      ForwardingSlaveBase::mimetype ( url );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::listDir

/**
 * We deny all attempts to create a directory on the base level.
 * This base level is reserved for clipboards only as detected or configured.
 * TODO: maybe in future versions we want to allow a creation here: something like a self-created folder acting as simple fifo, aka a primitive clipboard
 */
void KIOClipboardProtocol::mkdir (const KUrl &url, int permissions )
{
  MY_KDEBUG_BLOCK ( "<mkdir>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // mkdir() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.prettyUrl() );
    }
    else
    {
      ForwardingSlaveBase::mkdir ( url, permissions );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::put

/**
 * We deny all attempts to push an entry into this base level. 
 * This base level is reserved for clipboards only as detected or configured.
 */
void KIOClipboardProtocol::put (const KUrl &url, int permissions, JobFlags flags )
{
  MY_KDEBUG_BLOCK ( "<put>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // put() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.prettyUrl() );
    }
    else
    {
      ForwardingSlaveBase::put ( url, permissions, flags );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::put

/**
 * We answer requests for information about nodes with a UDSEntry
 * we rely on data previously provided by predetection of configuration of clipboards
 */
void KIOClipboardProtocol::stat (const KUrl &url)
{
  MY_KDEBUG_BLOCK ( "<stat>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      statEntry ( toUDSEntry() );
      finished();
    }
    else
    {
      ForwardingSlaveBase::stat ( url );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::stat

/**
 * We deny all attempts to push an entry into this base level.
 * This base level is reserved for clipboards only as detected or configured.
 */
void KIOClipboardProtocol::symlink ( const QString& target, const KUrl& dest, JobFlags flags )
{
  MY_KDEBUG_BLOCK ( "<symlink>" );
  kDebug() << dest.url();
  try
  {
    if ( QLatin1String("/")==dest.path() || dest.path().isEmpty() )
    {
      // symlink() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), dest.prettyUrl() );
    }
    else
    {
      ForwardingSlaveBase::symlink ( target, dest, flags );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::stat
