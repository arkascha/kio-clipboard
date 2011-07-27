#include <stdlib.h>
#include <unistd.h>
#include <KUrl>
#include <KMimeType>
#include <kdebug.h>
#include "kio_clipboard_protocol.h"
#include "christian-reiner.info/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

KIOClipboardProtocol::KIOClipboardProtocol( const QByteArray &_pool, const QByteArray &_app )
  : ForwardingSlaveBase ( "clipboard", _pool, _app )
{
  KDebug::Block myBlock( "<slave setup>" );
  kDebug();
  try
  {
    QMap<ClipboardType,QString> _clipboards = KIOClipboardWrapper::detectClipboards();
    // register each detected clipboard
    QMap<ClipboardType,QString>::iterator _entry;
    for ( _entry = _clipboards.begin(); _entry != _clipboards.end(); _entry++ )
    {
      KUrl _url (QString("clipboard:/%1").arg(_entry.value()));
      kDebug() << QString("registering clipboard of type '%1' at url %2").arg(_entry.key()).arg(_url.prettyUrl());
      registerClipboard ( _entry.key(), _url );
    }
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

KIOClipboardProtocol::~KIOClipboardProtocol()
{
  KDebug::Block myBlock( "<slave shutdown>" );
  kDebug();
  try
  {
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

void KIOClipboardProtocol::registerClipboard ( ClipboardType type, const KUrl& base )
{
  kDebug();
  UDSEntry _entry;
  _entry.insert( UDSEntry::UDS_NAME,         base.fileName() );
  _entry.insert( UDSEntry::UDS_DISPLAY_NAME, base.fileName() );
  _entry.insert( UDSEntry::UDS_MIME_TYPE,    "inode/directory");
  _entry.insert( UDSEntry::UDS_URL,          base.url() );
  _entry.insert( UDSEntry::UDS_ACCESS,       S_IRUSR | S_IRGRP | S_IROTH );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,    S_IFDIR );
//  _entry.insert( UDSEntry::UDS_EXTRA,        "" );
//  _entry.insert( UDSEntry::UDS_EXTRA+1,      "1" );
  m_clipboards.insert ( base, _entry );
} // KIOClipboardProtocol::registerClipboard

const UDSEntry KIOClipboardProtocol::rootUDSEntry ()
{
  kDebug();
  UDSEntry _entry;
  _entry.clear();
  _entry.insert( UDSEntry::UDS_NAME,         QString::fromLatin1("."));
  _entry.insert( UDSEntry::UDS_FILE_TYPE,    S_IFDIR);
  _entry.insert( UDSEntry::UDS_ACCESS,       0700);
  _entry.insert( UDSEntry::UDS_MIME_TYPE,    QString::fromLatin1("inode/directory"));
  return _entry;
} // KIOClipboardProtocol::rootUDSEntry

const UDSEntryList KIOClipboardProtocol::listOfUDSEntries ()
{
  kDebug();
  UDSEntryList _entries;
  foreach ( UDSEntry _entry, m_clipboards.values() )
    _entries << _entry;
  return _entries;
} // KIOClipboardProtocol::listOfUDSEntries

//======================

bool KIOClipboardProtocol::rewriteUrl   ( const KUrl& oldUrl, KUrl& newUrl )
{
  // convert clipboard:/klipper/02 to klipper:/02
  kDebug() << oldUrl.url();
  try
  {
    QStringList tokens = KIO_CLIPBOARD::tokenizeUrl(oldUrl);
    // token holds n+1 entries (0-n), where 0 holds the _whole_ url
    switch ( tokens.count() )
    {
      case 1: // just a plain clipboard:/ was requested
        // we should not come here...
        kDebug() << "rewriting attempt of url pointing to this object";
        throw CRI::Exception ( Error(ERR_UNSUPPORTED_ACTION), oldUrl.url() );
        break;
      case 2: // something like clipboard:/klipper was requested
//        newUrl = KUrl(QString("%1:/").arg(tokens[1]));
        newUrl.setScheme(tokens[1]);
        newUrl.addPath("/");
        break;
      case 3: // full url like clipboard:/klipper/007 was requested
//        newUrl = KUrl(QString("%1:").arg(tokens[1])).addPath(tokens[2]);
        newUrl.setScheme(tokens[1]);
        newUrl.addPath(tokens[2]);
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
  KDebug::Block myBlock( "<del>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // del() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.url() );
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
  KDebug::Block myBlock( "<get>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // get() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.url() );
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
  KDebug::Block myBlock( "<listDir>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      totalSize ( m_clipboards.size() );
      listEntries ( listOfUDSEntries() );
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
  KDebug::Block myBlock( "<mimetype>" );
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
  KDebug::Block myBlock( "<mkdir>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // mkdir() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.url() );
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
  KDebug::Block myBlock( "<put>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      // put() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), url.url() );
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
  KDebug::Block myBlock( "<stat>" );
  kDebug() << url.url();
  try
  {
    if ( QLatin1String("/")==url.path() || url.path().isEmpty() )
    {
      statEntry ( rootUDSEntry() );
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
  KDebug::Block myBlock( "<symlink>" );
  kDebug() << dest.url();
  try
  {
    if ( QLatin1String("/")==dest.path() || dest.path().isEmpty() )
    {
      // symlink() on the root entry ?!?
      throw CRI::Exception ( Error(ERR_SERVICE_NOT_AVAILABLE), dest.url() );
    }
    else
    {
      ForwardingSlaveBase::symlink ( target, dest, flags );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOClipboardProtocol::stat
