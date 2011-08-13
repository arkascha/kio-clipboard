/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <QCryptographicHash>
#include <kdebug.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <klocalizedstring.h>
#include <kdeversion.h>
#include "christian-reiner.info/exception.h"
#include "kio_clipboard_protocol.h"
#include "wrapper/node_wrapper.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/**
 * This constructs a node object that describes exactly one single entry on a clipboard in a passive and constant way.
 * No manipulations are offered, the purpose is to offer convenient methods to access data about such an entry.
 * - few synthactical configuration settings are defined first
 * - all basic data is collected and stored in private members
 * - primitive rules are used decide upon a few basic interpretations of the type of content in an entry
 */
KIONodeWrapper::KIONodeWrapper ( KIOClipboardWrapper* const clipboard,  int index, const QString& payload )
  : m_clipboard ( clipboard )
{
  kDebug() << index;
  m_index = index;
  // construct a valid file name, even for a payload that is a path or url
  m_name  = QString(QCryptographicHash::hash(payload.toUtf8(),QCryptographicHash::Md5).toHex());
  // mark first entry in the list as the newest by using an overlay
  if ( 1==index )
    m_overlays << "emblem-new";
  // decide about the sematics ("meaning") of the content
  if ( payload.trimmed().isEmpty() )
  {
    m_semantics = KIO_CLIPBOARD::S_EMPTY;
    m_payload   = payload;
  }
  else if ( m_regEx["uri"].exactMatch(payload.trimmed()) )
  {
    m_semantics = KIO_CLIPBOARD::S_URL;
    m_payload   = payload.trimmed ( );
    m_url       = KUrl ( m_payload );
    m_link      = KUrl ( m_payload );
    if ( m_url.isLocalFile() )
      m_path      = m_url.path ( );
  }
  else if ( m_regEx["path"].exactMatch(payload.trimmed()) )
  {
    m_semantics = KIO_CLIPBOARD::S_FILE;
    m_payload   = payload.trimmed ( );
    m_url       = KUrl ( m_payload );
    m_link      = KUrl ( m_payload );
    m_path      = m_payload;
  }
  else
  {
    m_semantics = KIO_CLIPBOARD::S_TEXT;
    m_payload   = payload; // no trimming, keep extra whitespaces, important for code
  }
  switch ( m_semantics )
  {
    case KIO_CLIPBOARD::S_EMPTY:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::mimeType("text/plain");
      m_overlays << "emblem-special";
      break;
    case KIO_CLIPBOARD::S_TEXT:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::findByContent(payload.toUtf8());
      // check if we can refine the sematics to something more specific
      // TODO: find some more generic way as an alternative to this list of test for recognized mimetypes
      if (  ("text/x-"==m_mimetype->name().left(7))
          ||(m_mimetype->is("text/css"))
          ||(m_mimetype->is("text/html"))
          ||(m_mimetype->is("text/sgml"))
          ||(m_mimetype->is("text/xml")) )
        m_semantics = KIO_CLIPBOARD::S_CODE;
      break;
    case KIO_CLIPBOARD::S_CODE:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::findByContent(payload.toUtf8());
      break;
    case KIO_CLIPBOARD::S_FILE:
//      m_type     = S_IFREG;
      m_type     = S_IFLNK;
      m_mimetype = KMimeType::findByUrl(m_url);
      m_overlays << "emblem-symbolic-link";
      break;
    case KIO_CLIPBOARD::S_DIR:
      m_type     = S_IFDIR;
      m_mimetype = KMimeType::mimeType("inode/directory");
      m_overlays << "emblem-symbolic-link";
      break;
    case KIO_CLIPBOARD::S_LINK:
      m_type     = S_IFLNK;
      m_mimetype = KMimeType::findByUrl(m_url);
      m_overlays << "emblem-link";
      break;
    case KIO_CLIPBOARD::S_URL:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::findByUrl(m_url);
//      m_mimetype = NetAccess::mimetype ( m_url, NULL ); // far too expensive for remote files
//      m_mimetype = KMimeType::mimetype("application/octet-stream");
      m_overlays << "emblem-link";
      break;
    default:
      m_type     = S_IFMT;
      m_mimetype = KMimeType::mimeType("application/octet-stream");
  }
} // KIONodeWrapper::KIONodeWrapper

/**
 * Currently no cleanup to be done, since all such nodes have a passive character
 */
KIONodeWrapper::~KIONodeWrapper()
{
  kDebug();
} // KIONodeWrapper::~KIONodeWrapper

//==========

/**
 * we define a numerical index for every clipboard entry
 * this index names the entries position in the clipboard, since all clipboards work as a fifo
 * however when using this numerical index as part of names we have to take care of alphabetic sorting
 * therefore we construct a prettyIndex that is prepended with leading zeros so that all entries have a prettyIndex of same length
 */
QString KIONodeWrapper::prettyIndex ( ) const
{
  QString _pretty = QString("%1").arg(m_index,m_clipboard->mappingNameCardinality(),10,QChar('0'));
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyIndex

/**
 * we usually store the whole and unchanged payload an entry holds
 * only exceptions are file refrences and URLs where we trim a little here and there
 * however, when use the payload as part of an entries 'name' at a few places to make the entry recognizable
 * in those situations we have to crop the payload since names shoulds not get too long
 */
QString KIONodeWrapper::prettyPayload ( ) const
{
  QString _payload = m_payload.simplified();
  int _trim_length = (m_clipboard->mappingNameLength()<15) ? 15 : m_clipboard->mappingNameLength()-5;
  if ( _trim_length<_payload.length() )
    _payload = QString("%1[...]").arg(_payload.left(_trim_length-5));
  kDebug() << _payload;
  return _payload;
} // KIONodeWrapper::prettyPayload

/**
 * returns the human readable description of a mimetype
 */
QString KIONodeWrapper::prettyMimetype ( ) const
{
  kDebug() << m_mimetype->comment();
  return m_mimetype->comment();
} // KIONodeWrapper::prettyMimetype

/**
 * since the internal 'semantics', the meaning of an entries content is very important
 * for how to handle such an entry we want to share this information with the user
 * for this purpose we translate a (numeric) semantics marker into a human readable string
 */
QString KIONodeWrapper::prettySemantics ( ) const
{
  QString _pretty;
  switch ( m_semantics )
  {
    case KIO_CLIPBOARD::S_EMPTY: _pretty = i18n ( "Empty" );     break;
    case KIO_CLIPBOARD::S_TEXT:  _pretty = i18n ( "Text" );      break;
    case KIO_CLIPBOARD::S_CODE:  _pretty = i18n ( "Code" );      break;
    case KIO_CLIPBOARD::S_FILE:  _pretty = i18n ( "File" );      break;
    case KIO_CLIPBOARD::S_DIR:   _pretty = i18n ( "Directory" ); break;
    case KIO_CLIPBOARD::S_LINK:  _pretty = i18n ( "Link");       break;
    case KIO_CLIPBOARD::S_URL:   _pretty = i18n ( "URL" );       break;
    default:                     _pretty = i18n ( "???" );
  } // switch
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettySemantics

/**
 * each node must be represented by a name when offering it to the user
 * however entries usually do not have ordinary file names on a clipboard
 * they are identified either by a simple index, a (temporary) position or maybe something like a title
 * for our purpose we require a simple file name, therefore we generate one
 * the name is based on the passive data collected about a node and turned into something 'pretty'
 */
QString KIONodeWrapper::prettyName ( ) const
{
  // we construct something like this: "007(String): Es war einmal vor langer, langer Zeit [...]"
  QString _pretty = m_clipboard->mappingNamePattern()
                    // a leading numerical index, cardinality depends of the size of the set of nodes
                    .arg( prettyIndex() )
                    // linguistic type of content, like TEXT or CODE or URL
                    .arg( prettySemantics() )
                    // the whole alpha-numerical content
                    .arg( prettyPayload() );
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyName

/**
 * just a wrapper to present a URL an entry might point to in a human-readable form
 */
QString KIONodeWrapper::prettyUrl ( ) const
{
  QString _pretty = m_url.prettyUrl();
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyUrl

//==========

/**
 * keeping each node in a self defined and convenient object is fine for internal handling
 * however we have to translate such an object to present the entry to the outside world
 * this is done by translating an object to a constant UDSEntry as understood by the underlying KIO system
 */
UDSEntry KIONodeWrapper::toUDSEntry ( ) const
{
  kDebug() << prettyName() << "(" << m_index << ")";
  UDSEntry _entry;
  //_entry.insert( UDSEntry::UDS_NAME,               prettyIndex() );
  _entry.insert( UDSEntry::UDS_NAME,               name() );
  _entry.insert( UDSEntry::UDS_DISPLAY_NAME,       prettyName() );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,          m_type );
  _entry.insert( UDSEntry::UDS_MIME_TYPE,          m_mimetype->name() );
#if KDE_IS_VERSION(4,5,0)
  _entry.insert( UDSEntry::UDS_DISPLAY_TYPE,       m_mimetype->comment() );
#endif
  _entry.insert( UDSEntry::UDS_SIZE,               size() );
  _entry.insert( UDSEntry::UDS_ACCESS,             S_IRUSR | S_IRGRP | S_IROTH );
//  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME,  utime(path, &myutimbuf);
  if ( !m_path.isEmpty() )
    _entry.insert( UDSEntry::UDS_LOCAL_PATH,         m_path );
  if ( ! m_url.isEmpty() )
    _entry.insert( UDSEntry::UDS_TARGET_URL,         m_url.url() );
//  if ( ! m_link.isEmpty() )
//    _entry.insert( UDSEntry::UDS_LINK_DEST,          m_link.url() );
  if ( ! m_icon.isEmpty() )
    _entry.insert( UDSEntry::UDS_ICON_NAME,          m_icon );
#if KDE_IS_VERSION(4,5,0)
  if ( ! m_overlays.isEmpty() )
    _entry.insert( UDSEntry::UDS_ICON_OVERLAY_NAMES, m_overlays.join(",") );
#endif
  // some intense debugging output...
  QList<uint> _tags = _entry.listFields ( );
  kDebug() << "list of defined UDS entry tags for entry" << prettyIndex() << ":";
  foreach ( uint _tag, _tags )
    switch ( _tag )
    {
      case UDSEntry::UDS_NAME:               kDebug() << "UDS_NAME:"               << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_LOCAL_PATH:         kDebug() << "UDS_LOCAL_PATH:"         << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_URL:                kDebug() << "UDS_URL:"                << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_TARGET_URL:         kDebug() << "UDS_TARGET_URL:"         << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_LINK_DEST:          kDebug() << "UDS_LINK_DEST:"          << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_SIZE:               kDebug() << "UDS_SIZE:"               << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_FILE_TYPE:          kDebug() << "UDS_FILE_TYPE:"          << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_MIME_TYPE:          kDebug() << "UDS_MIME_TYPE:"          << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_GUESSED_MIME_TYPE:  kDebug() << "UDS_GUESSED_MIME_TYPE:"  << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_DISPLAY_NAME:       kDebug() << "UDS_DISPLAY_NAME:"       << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_ACCESS:             kDebug() << "UDS_ACCESS:"             << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_ICON_NAME:          kDebug() << "UDS_ICON_NAME:"          << _entry.stringValue(_tag); break;
#if KDE_IS_VERSION(4,5,0)
      case UDSEntry::UDS_ICON_OVERLAY_NAMES: kDebug() << "UDS_ICON_OVERLAY_NAMES:" << _entry.stringValue(_tag); break;
#endif
      default:                               kDebug() << "UDS_<UNKNOWN>:"          << _tag;
    } // switch

  return _entry;
} // KIONodeWrapper::toUDSEntry
