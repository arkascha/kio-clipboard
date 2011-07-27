/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $HeadURL$
 * $Author$
 * $Revision$
 * $Date$
 */
#include <stdlib.h>
#include <unistd.h>
#include <KUrl>
#include <KMimeType>
#include <kdeversion.h>
#include <kmimetype.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kio/filejob.h>
#include <kdebug.h>
#include "kio_klipper_protocol.h"
#include "christian-reiner.info/exception.h"

// Kdebug::Block is only defined from KDE-4.6.0 on
// we wrap it cause this appears to be the only requirement for KDE-4.6
#if KDE_IS_VERSION(4,6,0)
#define MY_KDEBUG_BLOCK(token) KDebug::Block myBlock(token);
#else
#define MY_KDEBUG_BLOCK(token) kDebug()<<token;
#endif

using namespace KIO;
using namespace KIO_CLIPBOARD;

KIOKlipperProtocol::KIOKlipperProtocol( const QByteArray &_pool, const QByteArray &_app, QObject* parent )
  : QObject ( parent )
  , SlaveBase ( "klipper", _pool, _app )
  , KIOClipboardWrapperKlipper ( KUrl("klipper:/") )
{
//  KDebug::Block myBlock( "<slave setup>" );
  MY_KDEBUG_BLOCK( "<slave setup>" );
  kDebug() << "constructing protocol";
  try
  {
    // initialize the wrappers pool content
    refreshPool();
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
}

KIOKlipperProtocol::~KIOKlipperProtocol()
{
  KDebug::Block myBlock( "<slave shutdown>" );
  kDebug() << "destructing protocol";
  try
  {
  }
  catch ( CRI::Exception &e )
  {
    error ( e.getCode(), e.getText() );
  }
}

//==========

const UDSEntry KIOKlipperProtocol::rootUDSEntry ()
{
  kDebug() << type();
  UDSEntry _entry;
  _entry.clear();
  _entry.insert( UDSEntry::UDS_NAME,      QString::fromLatin1("."));
  _entry.insert( UDSEntry::UDS_FILE_TYPE, S_IFDIR);
  _entry.insert( UDSEntry::UDS_ACCESS,    0744);
  _entry.insert( UDSEntry::UDS_MIME_TYPE, QString::fromLatin1("inode/directory"));
  return _entry;
} // KIOKlipperProtocol::rootUDSEntry

//======================

void KIOKlipperProtocol::del ( const KUrl& url, bool isfile )
{
  // note: isfile signals if a directory or a file is meant to be deleted
  // this does make little sense for a clipboard, since it is just a string anyway
  KDebug::Block myBlock( "<del>" );
  kDebug() << url.prettyUrl() << isfile;
  try
  {
    // remove entry from clipboard
    delEntry ( url );
    finished();
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::del

void KIOKlipperProtocol::get ( const KUrl& url )
{
  KDebug::Block myBlock( "<get>" );
  kDebug() << url.prettyUrl() ;
  KUrl _url;
  try
  {
    // find the matching entry
    UDSEntry _entry = findEntryByUrl(url);
    // send data, depending on the semantic type of the payload
    switch ( _entry.numberValue(UDSEntry::UDS_EXTRA+2) )
    {
      case T_STRING:
      case T_CODE:
        mimeType ( _entry.stringValue(UDSEntry::UDS_MIME_TYPE) );
        data     ( _entry.stringValue(UDSEntry::UDS_EXTRA).toUtf8() );
        data     ( QByteArray() );
        finished();
        return;
      case T_FILE:
      case T_DIR:
        _url = KUrl(_entry.stringValue(UDSEntry::UDS_LOCAL_PATH));
        kDebug() << "redirecting to:" << _url;
        redirection ( _url );
        finished();
        return;
      case T_URL:
        _url = KUrl(_entry.stringValue(UDSEntry::UDS_URL));
        kDebug() << "redirecting to:" << _url;
        redirection ( _url );
        finished();
        return;
      default:
        throw CRI::Exception ( Error(ERR_INTERNAL_SERVER), url.prettyUrl() );
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::get

void KIOKlipperProtocol::listDir ( const KUrl& url )
{
  KDebug::Block myBlock( "<listDir>" );
  kDebug() << url.prettyUrl();
  try
  {
    if ( url.path().isEmpty() )
    {
      KUrl _new_url = url;
      _new_url.setPath("/");
      redirection ( _new_url );
      finished ( );
      return;
    }
    refreshPool();
    totalSize ( m_pool.size() );
    listEntries ( m_pool );
    finished ();
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::listDir

void KIOKlipperProtocol::mimetype ( const KUrl& url )
{
  KDebug::Block myBlock( "<mimetype>" );
  kDebug() << url.prettyUrl();
  try
  {
    // find the matching pool entry
    UDSEntry _entry = findEntryByUrl(url);
    if ( 1==0 && _entry.contains(UDSEntry::UDS_MIME_TYPE) )
    {
      mimeType ( _entry.stringValue(UDSEntry::UDS_MIME_TYPE) );
      finished();
      return;
    }
    else
    {
      KUrl _url;
      switch ( _entry.numberValue(UDSEntry::UDS_EXTRA+2) )
      {
        case T_STRING:
        case T_CODE:
          mimeType ( "text/plain" );
          finished();
          return;
        case T_FILE:
        case T_DIR:
          _url = KUrl(_entry.stringValue(UDSEntry::UDS_LOCAL_PATH));
          kDebug() << "redirecting to:" << _url;
          redirection ( _url );
          finished ( );
          return;
        case T_URL:
          _url = KUrl(_entry.stringValue(UDSEntry::UDS_URL));
          kDebug() << "redirecting to:" << _url;
          redirection ( _url );
          finished ( );
          return;
        default:
          throw CRI::Exception ( Error(ERR_INTERNAL_SERVER), url.prettyUrl() );
      } // switch
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::mimetype

void KIOKlipperProtocol::mkdir ( const KUrl& url, int permissions )
{
  // ToDo: this currently works recursive:
  // all files contained in the directory are copied one by one
  // ToDo: just like in put(): the resulting url is clipboard:/-specific, that makes no sense
  // note: permissions and flags (OVERWRITE) dont make sense for a local clipboard
  KDebug::Block myBlock( "<mkdir>" );
  kDebug() << url.prettyUrl() << permissions;
  try
  {
    pushEntry ( url.url() );
    finished();
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::mkdir

void KIOKlipperProtocol::put ( const KUrl& url, int permissions, KIO::JobFlags flags )
{
  // ToDo: the resulting url is clipboard:/-specific, that makes no sense
  // ToDo: it has to be source specific instead ! and a full path, not just a file name
  // note: permissions and flags (OVERWRITE) dont make sense for a local clipboard
  KDebug::Block myBlock( "<put>" );
  kDebug() << url.prettyUrl() << permissions << flags;
  try
  {
    // we want to push an entry to the clipboard, but this is the _content_ of the url, not the name
    int _ret_val;
    int _max_buffer_length = 32*1024; // TODO: turn into something like a global define or configuration value
    QByteArray _buffer, _payload;
    do
    {
      dataReq();
      int _ret_val = readData ( _buffer );
      if  ( _ret_val<0 )
        throw CRI::Exception ( Error(ERR_COULD_NOT_READ), url.prettyUrl() ); // TODO: show source file instead of target file
      _payload += _buffer;
    } while ( _ret_val!=0 && _max_buffer_length>_payload.size() ); // a return value of 0 (zero) means: no more data
    pushEntry ( QString(_payload) );
    finished ( );
  }
  catch ( CRI::Exception &e ) { error ( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::put

void KIOKlipperProtocol::stat( const KUrl& url )
{
  KDebug::Block myBlock( "<stat>" );
  kDebug() << url.prettyUrl();
  try
  {
    if ( url.path().isEmpty() )
    {
      KUrl _new_url = url;
      _new_url.setPath("/");
      kDebug() << "redirecting to:" << _new_url;
      redirection ( _new_url );
      finished ( );
      return;
    }
    if ( QLatin1String("/")==url.path() )
    {
      kDebug() << "generating root entry";
      statEntry ( rootUDSEntry() );
      finished ( );
      return;
    }
    else
    {
      //non-root element
      UDSEntry _entry = findEntryByUrl ( url );
      switch ( _entry.numberValue(UDSEntry::UDS_EXTRA+2) )
      {
        case T_STRING:
        case T_CODE:
          statEntry ( _entry );
          finished ( );
          return;
        case T_FILE:
        case T_DIR:
          redirection ( KUrl(_entry.stringValue(UDSEntry::UDS_URL)) );
          finished();
          return;
        case T_URL:
          redirection ( KUrl(_entry.stringValue(UDSEntry::UDS_URL)) );
          finished ( );
          return;
        default:
          throw CRI::Exception ( Error(ERR_INTERNAL_SERVER), url.prettyUrl() );
      } // switch
    }
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::stat

void KIOKlipperProtocol::symlink ( const QString& target, const KUrl& dest, KIO::JobFlags flags )
{
  KDebug::Block myBlock( "<symlink>" );
  kDebug() << target << dest.prettyUrl() << flags;
  try
  {
    KUrl _url ( target );
    if ( _url.isLocalFile() )
      pushEntry ( _url.path() );
    else
//      pushEntry ( _url.url() );
      pushEntry ( _url.prettyUrl() );
    finished();
  }
  catch ( CRI::Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::symlink

#include "kio_klipper_protocol.moc"
