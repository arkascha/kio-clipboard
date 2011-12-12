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
#include <kmimetype.h>
#include <klocalizedstring.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kio/filejob.h>
#include <kdebug.h>
#include <kdeversion.h>
#include "kio_klipper_protocol.h"
#include "clipboard/clipboard_frontend.h"
#include "utility/exception.h"

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
 * The standard constructor, nothing special here.
 * A fresh object is handled to the generic interface class this class derives from.
 * That object is initialized by refreshing it's nodes collection and destroyed again locally in the destructor. 
 */
KIOKlipperProtocol::KIOKlipperProtocol( const QByteArray &pool, const QByteArray &app, QObject* parent )
  : QObject ( parent )
  , KIOProtocol ( pool, app, new KlipperFrontend(KUrl("klipper:/"),"klipper") )
{
  MY_KDEBUG_BLOCK ( "<slave setup>" );
  try
  {
    // initialize the wrappers nodes
    m_clipboard->refreshNodes ( );
  }
  catch ( Exception &e ) { error ( e.getCode(), e.getText() ); }
}

/**
 * The destructor cleans up the specialized clipboard wrapper object instantiated in the constructor. 
 */
KIOKlipperProtocol::~KIOKlipperProtocol()
{
  MY_KDEBUG_BLOCK ( "<slave shutdown>" );
  try
  {
    delete m_clipboard;
  }
  catch ( Exception &e )
  {
    error ( e.getCode(), e.getText() );
  }
}

//==========

/**
 * Generates a plain UDSEntry object that describes this protocol itself, that is its base folder. 
 */
const UDSEntry KIOKlipperProtocol::toUDSEntry ()
{
  kDebug() << m_clipboard->type();
  UDSEntry _entry;
  _entry.clear();
  _entry.insert( UDSEntry::UDS_NAME,              m_clipboard->protocol() );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,         S_IFDIR );
  _entry.insert( UDSEntry::UDS_ACCESS,            0700 );
  _entry.insert( UDSEntry::UDS_MIME_TYPE,         QString::fromLatin1("inode/directory") );
  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME, KDateTime::currentLocalDateTime().toTime_t() );
  return _entry;
} // KIOKlipperProtocol::toUDSEntry

/**
 * Generates a list of UDSEntries that describe all nodes (clipboard entries) as present in the clipboard wrapper
 */
const UDSEntryList KIOKlipperProtocol::toUDSEntryList ()
{
  kDebug() << "listing" << m_clipboard->countNodes() << "entries";
  return m_clipboard->toUDSEntryList ( );
} // KIOKlipperProtocol::toUDSEntryList

//======================

/**
 * This implements the direct and fast copy actions that bypass the get()&put() combination if defined.
 * There are actually 3 cases of copy requests that differ:
 * 1.) 'klipper:/...' => 'klipper:/...', we deny this, since the names of clipboard entries cannot be influenced anyway
 * 2.) 'file:/...' => 'klipper:/...': we grant the request and create a new entry
 * 3.) 'klipper:/...' => 'file:/...': we deny the request, the calling interface will use get()&put() instead which works better
 */
void KIOKlipperProtocol::copy ( const KUrl& src, const KUrl& dest, int permissions, JobFlags flags )
{
  MY_KDEBUG_BLOCK ( "<copy>" );
  kDebug() << src.prettyUrl() << dest.prettyUrl() << permissions << flags;
  kDebug() << QString("copy from '%1' to '%2' requested").arg(src.scheme()).arg(dest.scheme());
  try
  {
    // there are different ways what this method might be used foreach
    if ( src.scheme()==m_clipboard->protocol() && dest.scheme()==m_clipboard->protocol() )
    {
      // klipper:/... >> klipper:/...
      kDebug() << "trivial internal copy request, whyever, aborting";
      throw Exception ( Error(ERR_SLAVE_DEFINED), "Names of clipboard entries are generated in a generic manner, you have no control over that. \nTherefore this action does not make sense" );
    }
    if ( src.scheme()=="file" && dest.scheme()==m_clipboard->protocol() )
    {
      // file:/... >> klipper:/...
      kDebug() << "handling source file managament directly";
      copyFromFile ( src, dest );
      finished ( );
      return;
    }
    if ( src.scheme()==m_clipboard->protocol() && dest.scheme()=="file" )
    {
      // klipper:/... >> file:/...
      kDebug() << "handling destiny file managament indirectly via get() & put()";
      throw Exception ( Error(ERR_UNSUPPORTED_ACTION), dest.prettyUrl() );
      return;
    }
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::copy

/**
 * Request to remove an entry from the clipboad.
 * We let the clipboard wrapper handle the real work. 
 */
void KIOKlipperProtocol::del ( const KUrl& url, bool isfile )
{
  // note: isfile signals if a directory or a file is meant to be deleted
  // this does make little sense for a clipboard, since it is just a string anyway
  MY_KDEBUG_BLOCK ( "<del>" );
  kDebug() << url.prettyUrl ( ) << isfile;
  try
  {
    // remove entry from clipboard
    m_clipboard->delEntry ( url );
    finished();
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::del

/**
 * Request to get an entry from the clipboad.
 * We differ 3 cases of what we actually deliver:
 * 1.) in case of human readable entries we deliver the content of the entry as payload
 * 2.) in case of references to files we redirect the interface to the file itself
 * 3.) in case if urls we redirect to interface to the url itself
 * This saves us from handling all sorts of file and url handling which is implemented in specialized protocols anyway
 */
void KIOKlipperProtocol::get ( const KUrl& url )
{
  MY_KDEBUG_BLOCK ( "<get>" );
  kDebug() << url.prettyUrl ( ) ;
  KUrl _url;
  try
  {
    // send data, depending on the semantics of the payload
    const NodeWrapper* _entry = m_clipboard->findNodeByUrl ( url );
    switch ( _entry->semantics() )
    {
      case KIO_CLIPBOARD::NodeWrapper::S_EMPTY:
      case KIO_CLIPBOARD::NodeWrapper::S_TEXT:
      case KIO_CLIPBOARD::NodeWrapper::S_CODE:
        mimeType ( _entry->mimetype()->name() );
        data     ( m_clipboard->getClipboardEntry(_entry->index()).toUtf8() );
        data     ( QByteArray() );
        finished ( );
        return;
      case KIO_CLIPBOARD::NodeWrapper::S_FILE:
      case KIO_CLIPBOARD::NodeWrapper::S_DIR:
        _url = KUrl(_entry->path() );
        kDebug() << "redirecting to:" << _url.prettyUrl ( );
        redirection ( _url );
        finished ( );
        return;
      case KIO_CLIPBOARD::NodeWrapper::S_URL:
        _url = KUrl(_entry->url() );
        kDebug() << "redirecting to:" << _url.prettyUrl ( );
        redirection ( _url );
        finished ( );
        return;
      default:
        throw Exception ( Error(ERR_INTERNAL_SERVER), url.prettyUrl() );
    }
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::get

/**
 * Lists all clipboard entries as present in the clipboard wrapper. 
 */
void KIOKlipperProtocol::listDir ( const KUrl& url )
{
  MY_KDEBUG_BLOCK ( "<listDir>" );
  kDebug() << url.prettyUrl ( );
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
    m_clipboard->refreshNodes ( );
    totalSize ( m_clipboard->countNodes() );
    listEntries ( toUDSEntryList() );
    finished ( );
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::listDir

/**
 * Delivers the mimetype of a clipboard entry.
 * Again, as in get() we differ 3 cases:
 * 1.) for human readable entries we deliver the mimetype is predetected by the clipboard wrapper
 * 2.) for entries holding refrences to files we redirect the interface to consider that file directly
 * 3.) for urls we redirect to those urls, since the specialized protocol implementations are certainly better in handling this
 */
void KIOKlipperProtocol::mimetype ( const KUrl& url )
{
  MY_KDEBUG_BLOCK ( "<mimetype>" );
  kDebug() << url.prettyUrl ( );
  try
  {
    // find the matching node entry
    const NodeWrapper* _entry = m_clipboard->findNodeByUrl ( url );
    KUrl _target;
    switch ( _entry->semantics() )
    {
      case KIO_CLIPBOARD::NodeWrapper::S_EMPTY:
      case KIO_CLIPBOARD::NodeWrapper::S_TEXT:
      case KIO_CLIPBOARD::NodeWrapper::S_CODE:
        mimeType ( _entry->mimetype()->name() );
        finished();
        return;
      case KIO_CLIPBOARD::NodeWrapper::S_FILE:
      case KIO_CLIPBOARD::NodeWrapper::S_DIR:
        _target = KUrl(_entry->path() );
        kDebug() << "redirecting to:" << _target.prettyUrl ( );
        redirection ( _target );
        finished ( );
        return;
      case KIO_CLIPBOARD::NodeWrapper::S_URL:
        _target = KUrl(_entry->url() );
        kDebug() << "redirecting to:" << _target.prettyUrl ( );
        redirection ( _target );
        finished ( );
        return;
      default:
        throw Exception ( Error(ERR_INTERNAL_SERVER), url.prettyUrl() );
    } // switch
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::mimetype

/**
 * creation of directories on a clipboard do not make sense: clipboards are no hierarchical file systems.
 * therefore we deny the request as a not supported action
 */
void KIOKlipperProtocol::mkdir ( const KUrl& url, int permissions )
{
  // FIXME: this currently works recursive:
  // all files contained in the directory are copied one by one
  // FIXME: just like in put(): the resulting url is clipboard:/-specific, that makes no sense
  // note: permissions and flags (OVERWRITE) dont make sense for a local clipboard
  MY_KDEBUG_BLOCK ( "<mkdir>" );
  kDebug() << url.prettyUrl ( ) << permissions;
  try
  {
    m_clipboard->pushEntry ( url.url() );
  /* ok: an ugly hack, looks strange, butappears to be working:
   * after linking w directory (we CANNOT copy a directory) we do NOT emit finished(), but throw an error instead.
   * This appears to prevent a recursive copy request of all the diretories content which is what we want.
   * so far I could not see any negative outcomes, since the directory itself HAS been linked, which is what we want too.
   */
    if ( KMimeType::findByPath(url.path())->is("inode/directory") )
    {
      kDebug() << "silently terminating recursive copying of a directory after linking it";
      throw Exception ( Error(ERR_UNSUPPORTED_ACTION), url.prettyUrl() );
    }
    finished();
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::mkdir

/**
 * request to push some entry onto the clipboard
 * unlike copy() this does not get any source url but the content as data instead
 * in case of human readable content we have no problems and simply copy the content to the clipboard
 * in other cases we want to create a reference to the source instead
 */
void KIOKlipperProtocol::put ( const KUrl& url, int permissions, KIO::JobFlags flags )
{
  // FIXME: the resulting url is clipboard:/-specific, that makes no sense
  // FIXME: it has to be source specific instead ! and a full path, not just a file name
  // note: permissions and flags (OVERWRITE) dont make sense for a local clipboard
  MY_KDEBUG_BLOCK ( "<put>" );
  kDebug() << url.prettyUrl ( ) << permissions << flags;
  try
  {
    // we want to push an entry to the clipboard, but this is the _content_ of the url, not the name
    int _ret_val;
    QByteArray _buffer, _payload;
    do
    {
      dataReq();
      int _ret_val = readData ( _buffer );
      if  ( _ret_val<0 )
        throw Exception ( Error(ERR_COULD_NOT_READ), url.prettyUrl() ); // FIXME: show source file instead of target file
      _payload += _buffer;
    } while ( _ret_val!=0 && m_clipboard->limit()>_payload.size() ); // a return value of 0 (zero) means: no more data
    m_clipboard->pushEntry ( QString(_payload) );
    finished ( );
  }
  catch ( Exception &e ) { error ( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::put

/**
 * request for the description of a single entry in the clipboard
 * we rely on the node description as collected by the specialized clipboard wrapper
 * for human readably entries we simple pass that information turned into an UDSEntry
 * for other cases, file and url references we redirect the interface to those instead
 */
void KIOKlipperProtocol::stat( const KUrl& url )
{
  MY_KDEBUG_BLOCK ( "<stat>" );
  kDebug() << url.prettyUrl ( );
  try
  {
    if ( url.path().isEmpty() )
    {
      KUrl _new_url = url;
      _new_url.setPath("/");
      kDebug() << "redirecting to:" << _new_url.prettyUrl ( );
      redirection ( _new_url );
      finished ( );
      return;
    }
    if ( QLatin1String("/")==url.path() )
    {
      // root elementŝ
      kDebug() << "generating root entry";
      statEntry ( toUDSEntry() );
      finished ( );
      return;
    }
    else
    {
      // non-root element
      const NodeWrapper* _entry = m_clipboard->findNodeByUrl ( url );
      switch ( _entry->semantics() )
      {
        case KIO_CLIPBOARD::NodeWrapper::S_EMPTY:
        case KIO_CLIPBOARD::NodeWrapper::S_TEXT:
        case KIO_CLIPBOARD::NodeWrapper::S_CODE:
          statEntry ( _entry->toUDSEntry() );
          finished ( );
          return;
        case KIO_CLIPBOARD::NodeWrapper::S_FILE:
        case KIO_CLIPBOARD::NodeWrapper::S_DIR:
          kDebug() << "redirecting to:" << KUrl(_entry->path()).prettyUrl ( );
          redirection ( KUrl(_entry->path()) );
          finished ( );
          return;
        case KIO_CLIPBOARD::NodeWrapper::S_URL:
          kDebug() << "redirecting to:" << _entry->prettyUrl ( );
          redirection ( _entry->url() );
          finished ( );
          return;
        default:
          throw Exception ( Error(ERR_INTERNAL_SERVER), url.prettyUrl() );
      } // switch
    }
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::stat

/**
 * (sorry, not really a clue how this works currently)
 * TODO FIXME do-something!
 */
void KIOKlipperProtocol::symlink ( const QString& target, const KUrl& dest, KIO::JobFlags flags )
{
  MY_KDEBUG_BLOCK ( "<symlink>" );
  kDebug() << target << dest.prettyUrl() << flags;
  try
  {
    KUrl _url ( target );
    if ( _url.isLocalFile() )
      m_clipboard->pushEntry ( _url.path() );
    else
      m_clipboard->pushEntry ( _url.url() );
//      pushEntry ( _url.prettyUrl() );
    finished();
  }
  catch ( Exception &e ) { error( e.getCode(), e.getText() ); }
} // KIOKlipperProtocol::symlink

#include "kio_klipper_protocol.moc"
