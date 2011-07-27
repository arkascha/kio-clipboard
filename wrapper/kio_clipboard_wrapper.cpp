/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $HeadURL$
 * $Author$
 * $Revision$
 * $Date$
 */
#include <math.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmimetype.h>
#include <kio/netaccess.h>
#include "kio_clipboard_wrapper.h"
#include "klipper/kio_clipboard_wrapper_klipper.h"
#include "kio_clipboard_protocol.h"
#include "christian-reiner.info/exception.h"

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

const QMap<ClipboardType,QString> KIOClipboardWrapper::detectClipboards ( )
{
  kDebug();
  QMap<ClipboardType,QString> _clipboards;
  // strategy: for clipboards available on DBus we ask org.freedesktop.DBus for such a service
  DBusClient dbus;
  dbus.setupInterface ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "" );
  dbus.call ( "ListNames" );
  foreach ( QString _name, dbus.convertReturnValue(dbus.result().first(),QVariant::StringList).toStringList() )
  {
    if ( "org.kde.klipper"==_name )
      _clipboards[ClipboardType(KLIPPER)] = "klipper";
  }
  kDebug() << "detected" << _clipboards.count() << "available clipboards";
  return _clipboards;
}

//==========

QString KIOClipboardWrapper::prettyTruncateName  ( const QString& name )
{
  int _trim_length = (m_mappingNameLength<15) ? 15 : m_mappingNameLength-5;
  if ( _trim_length<name.trimmed().length() )
    return QString("%1[...]").arg(name.trimmed().left(_trim_length-5));
  else
    return name.trimmed();
} // KIOClipboardWrapper::prettyTruncateName

QString KIOClipboardWrapper::mapDetails2Display ( const EntryTokens& details )
{
  // we construct something like this: "007(String): Es war einmal vor langer, langer Zeit [...]"
  QString _label = prettyTruncateName ( details.clipboardPayload );
  // construct a url to test the type of entry: url, path, string
  QString _node = m_mappingNamePattern
          // a leading numerical index, cardinality depends of the size of the pool
          .arg( details.technicalName )
          // linguistic type of content, like TEXT or CODE or URL
          .arg( details.printableType )
          // the whole alpha-numerical content
          .arg( _label );
  kDebug() << _node;
  return _node;
} // KIOClipboardWrapper::mapDetails2Display

EntryTokens KIOClipboardWrapper::mapPayload2Details ( int index, const QString& payload )
{
  kDebug() << index << prettyTruncateName( payload );
  EntryTokens _details;
  //=====
  // a pure numeric index (internal) // TODO: not really required => remove ?
  _details.numericIndex     = index;
  //=====
  // unaltered payload, might be trimmed further down
  _details.clipboardPayload = payload;
  //=====
  // index as string, containing leading zeros as required for alphabetic sorting
  _details.technicalName = QString("%1").arg(_details.numericIndex,m_mappingNameCardinality,10,QChar('0'));
  //=====
  // decide about the sematics ("meaning") of the content
  if ( m_regEx["uri"].exactMatch(_details.clipboardPayload.trimmed()) )
  {
    _details.clipboardPayload = _details.clipboardPayload.trimmed();
    _details.urlTarget        = KUrl(_details.clipboardPayload).url();
    _details.linkTarget       = _details.urlTarget;
    _details.internalType     = Semantic(T_URL);
  }
  else if ( m_regEx["path"].exactMatch(_details.clipboardPayload) )
  {
    _details.clipboardPayload = _details.clipboardPayload.trimmed();
    _details.fileTarget       = KUrl(_details.clipboardPayload).path();
    _details.linkTarget       = _details.fileTarget;
    _details.internalType     = Semantic(T_FILE);
  }
  else
  {
    // TODO: using libmagic differ between plain string or stuff like 'code' or else
    _details.internalType = Semantic(T_STRING);
  }
  //=====
  // set the fileType as defined by UDSEntry
  switch ( _details.internalType )
  {
    case T_STRING:
      _details.fileType      = S_IFREG;
      _details.mimeType      = "text/plain";
      _details.printableType = i18n("String");
      break;
    case T_CODE:
      _details.fileType      = S_IFREG;
      _details.mimeType      = "text/plain";
      _details.printableType = i18n("Code");
      break;
    case T_FILE:
      _details.fileType      = S_IFREG;
      _details.mimeType      = KMimeType::findByUrl(_details.urlTarget)->name();
      _details.printableType = i18n("File");
      break;
    case T_LINK:
      _details.fileType      = S_IFLNK;
      _details.mimeType      = KMimeType::findByUrl(_details.urlTarget)->name();
      _details.printableType = i18n("Link");
      break;
    case T_DIR:
      _details.fileType      = S_IFDIR;
      _details.mimeType      = "inode/directory";
      _details.printableType = i18n("Directory");
      break;
    case T_URL:
//      _details.fileType      = S_IFREG;
      _details.fileType      = S_IFLNK;
//      _details.mimeType      = "application/octet-stream";
//      _details.mimeType      = KMimeType::findByUrl(_details.urlTarget)->name();
      _details.mimeType = QString();
      _details.printableType = i18n("URL");
      break;
    default:
      _details.fileType      = S_IFMT;
      _details.mimeType      = "application/octet-stream";
      _details.printableType = i18n("URL");
  }
  //=====
  // the name as displayed to the user, in a "pretty" form :-)
  _details.displayName = mapDetails2Display ( _details );
  // that's it, I guess...
  return _details;
} // KIOClipboardWrapper::mapPayload2Details

//=======================

KIOClipboardWrapper::KIOClipboardWrapper ( const ClipboardType type, const KUrl& base )
  : m_mappingNameLength  (KIO_CLIPBOARD::C_mappingNameCardinality)
  , m_mappingNamePattern (KIO_CLIPBOARD::C_mappingNamePattern)
  , m_type ( type )
  , m_base ( base )
  , m_regEx ( CRI::loadRegExPool() )
{
  kDebug() << "constructing generic clipboard wrapper";
} // KIOClipboardWrapper::KIOClipboardWrapper

KIOClipboardWrapper::~KIOClipboardWrapper()
{
  kDebug() << "destructing generic clipboard wrapper";
} // KIOClipboardWrapper::~KIOClipboardWrapper

UDSEntry KIOClipboardWrapper::createPoolEntry ( int index, QString payload )
{
  kDebug() << index << prettyTruncateName ( payload);
  // a <struct> holding all the internal details
  // internally prepare the details for later storage
  EntryTokens _details = mapPayload2Details ( index, payload );
  kDebug() << QString("additional pool entry %1[%2]: %3").arg(_details.numericIndex).arg(_details.mimeType).arg(_details.displayName);
  // fill details in an 'official' UDSEntry object
  UDSEntry _entry;
  // UDS_EXTRA: the "real" payload, unaltered
  _entry.insert( UDSEntry::UDS_EXTRA,             _details.clipboardPayload );
  // UDS_EXTRA+1: the numerical index, but formatted as string (example: "07")
  _entry.insert( UDSEntry::UDS_EXTRA+1,           _details.numericIndex );
  // UDS_EXTRA+2: the semantical payload type (example: STRING or FILE or URL)
  _entry.insert( UDSEntry::UDS_EXTRA+2,           _details.internalType );
  // UDS_NAME: numerical index, this is how the file is identified (example: clipboard:/5)
  _entry.insert( UDSEntry::UDS_NAME,              _details.technicalName );
  // UDS_URL: a path the entry points to
  if ( !_details.fileTarget.isEmpty() )
    _entry.insert( UDSEntry::UDS_LOCAL_PATH,        _details.fileTarget );
  // UDS_URL: a target URL the entry points to
  if ( ! _details.urlTarget.isEmpty() )
    _entry.insert( UDSEntry::UDS_TARGET_URL,        _details.urlTarget );
  // UDS_LINK_DEST:
  if ( ! _details.linkTarget.isEmpty() )
    _entry.insert( UDSEntry::UDS_LINK_DEST,         _details.linkTarget );
  // UDS_SIZE: file size (length of payload)
  _entry.insert( UDSEntry::UDS_SIZE,              _details.clipboardPayload.size() );
  // UDS_FILE_TYPE: (numerical) technical inode type (example: S_IFDIR)
  _entry.insert( UDSEntry::UDS_FILE_TYPE,         _details.fileType );
  // UDS_MIME_TYPE:
  if ( ! _details.mimeType.isEmpty() )
    _entry.insert( UDSEntry::UDS_MIME_TYPE,         _details.mimeType );
  // UDS_DISPLAY_NAME: the name that is shown in applications (combination of all from above)
  _entry.insert( UDSEntry::UDS_DISPLAY_NAME,      _details.displayName );
  // UDS_ACCESS
  _entry.insert( UDSEntry::UDS_ACCESS,            S_IRUSR | S_IRGRP | S_IROTH );
  // UDS_MODIFICATION_TIME
//  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME, utime(path, &myutimbuf);
  return _entry;
} // KIOClipboardWrapper::createPoolEntry

void KIOClipboardWrapper::refreshPool ()
{
  kDebug();
  // strategy: clear the pool before (re-) populating it
  m_pool.clear();
  // ask the specialised client for the entries
  QStringList entries = getClipboardEntries ();
  // update cardinality, important to construct names with correct cardinality of their name prefix indexes
  m_mappingNameCardinality = QString("%1").arg(entries.count()).size();
  kDebug() << QString("set mapping cardinality to %1 (length of numeric index)").arg(m_mappingNameCardinality);
  // now populate the pool, one entry after another
  int index=0;
  foreach ( QString entry, entries )
  {
    UDSEntry _entry = createPoolEntry ( ++index, entry );
    //signalModified ( _entry );
    m_pool << _entry;
  }
  kDebug() << QString("populated fresh pool to %1 entries").arg(m_pool.size());
} // KIOClipboardWrapper::refreshPool

const UDSEntry& KIOClipboardWrapper::findEntryByUrl ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  // note: we might have a fresh process...
  if ( m_pool.isEmpty() )
    refreshPool();
  // which entry in the pool is the requested one ?
  foreach ( const UDSEntry& _entry, m_pool )
  {
            // numerical name (like clipboard:/05)
    if (   (_entry.stringValue(UDSEntry::UDS_NAME)==url.fileName())
            // payload name (like clipboard:/test-text)
        || (_entry.stringValue(UDSEntry::UDS_EXTRA)==url.fileName()) )
      return _entry;
  }
  // no matching element found ?!?
  throw CRI::Exception ( Error(ERR_DOES_NOT_EXIST), url.prettyUrl() );
} // KIOClipboardWrapper::findEntryByUrl
