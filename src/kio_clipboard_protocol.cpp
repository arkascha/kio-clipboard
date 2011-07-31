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
#include <kdeversion.h>
#include <kdebug.h>
#include "christian-reiner.info/exception.h"
#include "kio_clipboard_protocol.h"
#include "wrapper/kio_clipboard_wrapper.h"

// Kdebug::Block is only defined from KDE-4.6.0 on
// we wrap it cause this appears to be the only requirement for KDE-4.6
#if KDE_IS_VERSION(4,6,0)
#define MY_KDEBUG_BLOCK(token) KDebug::Block myBlock(token);
#else
#define MY_KDEBUG_BLOCK(token) kDebug()<<token;
#endif

using namespace KIO;
using namespace KIO_CLIPBOARD;

KIOClipboardProtocol::KIOClipboardProtocol( const QByteArray &_pool, const QByteArray &_app )
  : ForwardingSlaveBase ( "clipboard", _pool, _app )
{
  MY_KDEBUG_BLOCK ( "<slave setup>" );
  kDebug();
  try
  {
    QList<const KIOClipboardWrapper*> _clipboards = KIOClipboardWrapper::detectClipboards();
    // register each detected clipboard
    foreach ( const KIOClipboardWrapper* _entry, _clipboards )
    {
      const KUrl _url (QString("clipboard:/%1").arg(_entry->name()));
      kDebug() << QString("registering clipboard of type '%1' at url %2").arg(_entry->type()).arg(_url.prettyUrl());
      m_nodes.insert ( _url, _entry );
    }
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

KIOClipboardProtocol::~KIOClipboardProtocol()
{
  MY_KDEBUG_BLOCK ( "<slave shutdown>" );
  kDebug();
  try
  {
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

const UDSEntry KIOClipboardProtocol::toUDSEntry ()
{
  kDebug();
  UDSEntry _entry;
  _entry.clear();
  _entry.insert( UDSEntry::UDS_NAME,         QString::fromLatin1("."));
  _entry.insert( UDSEntry::UDS_FILE_TYPE,    S_IFDIR);
  _entry.insert( UDSEntry::UDS_ACCESS,       0700);
  _entry.insert( UDSEntry::UDS_MIME_TYPE,    QString::fromLatin1("inode/directory"));
  return _entry;
} // KIOClipboardProtocol::toUDSEntry

const UDSEntryList KIOClipboardProtocol::toUDSEntryList ()
{
  UDSEntryList _entries;
  foreach ( const KIOClipboardWrapper* _entry, m_nodes )
//  foreach ( const KIOClipboardWrapper* const& _entry, m_nodes.values() )
    _entries << _entry->toUDSEntry();
  kDebug() << "listing" << _entries.count() << "entries";
  return _entries;
} // KIOClipboardProtocol::toUDSEntryList

const KIO_CLIPBOARD::KIOClipboardWrapper* KIOClipboardProtocol::findClipboardByUrl ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  if ( m_nodes.contains(url) )
    return m_nodes[url];
  throw CRI::Exception ( Error(ERR_DOES_NOT_EXIST), url.prettyUrl() );
} // KIOClipboardProtocol::findClipboardByUrl

//======================

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
