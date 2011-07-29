/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <kdebug.h>
#include <kurl.h>
#include <kmimetype.h>
#include <klocalizedstring.h>
#include "christian-reiner.info/exception.h"
#include "kio_clipboard_protocol.h"
#include "wrapper/kio_node_wrapper.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

KIONodeWrapper::KIONodeWrapper ( int index, const QString& payload )
  : m_regEx    ( CRI::loadRegExPool() )
  , m_mappingNameCardinality (KIO_CLIPBOARD::C_mappingNameCardinality)
  , m_mappingNameLength      (KIO_CLIPBOARD::C_mappingNameLength)
  , m_mappingNamePattern     (KIO_CLIPBOARD::C_mappingNamePattern)
{
  kDebug();
  m_index     = index;
  // decide about the sematics ("meaning") of the content
  if ( m_regEx["uri"].exactMatch(payload.trimmed()) )
  {
    m_semantics =  Semantics(T_URL);
    m_payload   = payload.trimmed ( );
    m_url       = KUrl ( m_payload );
    if ( m_url.scheme()=="file" )
      m_path      = m_url.path ( );
  }
  else if ( m_regEx["path"].exactMatch(payload.trimmed()) )
  {
    m_semantics =  Semantics(T_FILE);
    m_payload   = payload.trimmed();
//    m_url       = KUrl ( m_payload );
    m_path      = m_payload;
  }
  else
  {
    // TODO: using libmagic differ between plain string or stuff like 'code' or else
    m_semantics =  Semantics(T_TEXT);
    m_payload   = payload; // no trimming, keep extra whitespaces, important for code
  }
  switch ( m_semantics )
  {
    case T_TEXT:
      m_type     = S_IFREG;
      m_mimetype = "text/plain";
      break;
    case T_CODE:
      m_type     = S_IFREG;
      m_mimetype = "text/plain";
      break;
    case T_FILE:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::findByUrl(m_url)->name();
      break;
    case T_LINK:
      m_type     = S_IFLNK;
      m_mimetype = KMimeType::findByUrl(m_url)->name();
      break;
    case T_DIR:
      m_type     = S_IFDIR;
      m_mimetype = "inode/directory";
      break;
    case T_URL:
//      m_type     = S_IFREG;
      m_type     = S_IFLNK;
      m_mimetype = "application/octet-stream";
      break;
    default:
      m_type     = S_IFMT;
      m_mimetype = "application/octet-stream";
  }
} // KIONodeWrapper::KIONodeWrapper

KIONodeWrapper::~KIONodeWrapper()
{
  kDebug();
} // KIONodeWrapper::~KIONodeWrapper

//==========

QString KIONodeWrapper::prettyIndex ( ) const
{
  QString _pretty = QString("%1").arg(m_index,m_mappingNameCardinality,10,QChar('0'));
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyIndex

QString KIONodeWrapper::prettyPayload ( ) const
{
  QString _pretty;
  int _trim_length = (m_mappingNameLength<15) ? 15 : m_mappingNameLength-5;
  if ( _trim_length<m_payload.trimmed().length() )
    _pretty = QString("%1[...]").arg(m_payload.trimmed().left(_trim_length-5));
  else
    _pretty = m_payload.trimmed();
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyPayload

QString KIONodeWrapper::prettySemantics ( ) const
{
  QString _pretty;
  switch ( m_semantics )
  {
    case Semantics(T_TEXT): _pretty = i18n ( "Text" );      break;
    case Semantics(T_CODE): _pretty = i18n ( "Code" );      break;
    case Semantics(T_FILE): _pretty = i18n ( "File" );      break;
    case Semantics(T_LINK): _pretty = i18n ( "Link");       break;
    case Semantics(T_DIR):  _pretty = i18n ( "Directory" ); break;
    case Semantics(T_URL):  _pretty = i18n ( "URL" );       break;
    default:                _pretty = i18n ( "???" );
  } // switch
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettySemantics

QString KIONodeWrapper::prettyName ( ) const
{
  // we construct something like this: "007(String): Es war einmal vor langer, langer Zeit [...]"
  QString _pretty = m_mappingNamePattern
                    // a leading numerical index, cardinality depends of the size of the set of nodes
                    .arg( prettyIndex() )
                    // linguistic type of content, like TEXT or CODE or URL
                    .arg( prettySemantics() )
                    // the whole alpha-numerical content
                    .arg( prettyPayload() );
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyName

QString KIONodeWrapper::prettyUrl ( ) const
{
  QString _pretty = m_url.prettyUrl();
  kDebug() << _pretty;
  return _pretty;
} // KIONodeWrapper::prettyUrl

//==========

UDSEntry KIONodeWrapper::toUDSEntry ( ) const
{
  kDebug() << prettyName() << "(" << m_index << ")";
  UDSEntry _entry;
  _entry.insert( UDSEntry::UDS_NAME,              prettyIndex() );
  if ( !m_path.isEmpty() )
    _entry.insert( UDSEntry::UDS_LOCAL_PATH,      m_path );
  if ( ! m_url.isEmpty() )
    _entry.insert( UDSEntry::UDS_URL,             m_url.url() );
  _entry.insert( UDSEntry::UDS_SIZE,              m_payload.size() );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,         m_type );
  if ( ! m_mimetype.isEmpty() )
    _entry.insert( UDSEntry::UDS_MIME_TYPE,       m_mimetype );
  _entry.insert( UDSEntry::UDS_DISPLAY_NAME,      prettyName() );
  _entry.insert( UDSEntry::UDS_ACCESS,            S_IRUSR | S_IRGRP | S_IROTH );
//  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME, utime(path, &myutimbuf);
  // some intense debugging...
  QList<uint> _tags = _entry.listFields ( );
  kDebug() << "list of defined UDS entry tags for entry" << prettyIndex() << ":";
  foreach ( uint _tag, _tags )
    switch ( _tag )
    {
      case UDSEntry::UDS_NAME:         kDebug() << "UDS_NAME:"         << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_LOCAL_PATH:   kDebug() << "UDS_LOCAL_PATH:"   << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_URL:          kDebug() << "UDS_URL:"          << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_TARGET_URL:   kDebug() << "UDS_TARGET_URL:"   << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_LINK_DEST:    kDebug() << "UDS_LINK_DEST:"    << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_SIZE:         kDebug() << "UDS_SIZE:"         << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_FILE_TYPE:    kDebug() << "UDS_FILE_TYPE:"    << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_MIME_TYPE:    kDebug() << "UDS_MIME_TYPE:"    << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_DISPLAY_NAME: kDebug() << "UDS_DISPLAY_NAME:" << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_ACCESS:       kDebug() << "UDS_ACCESS:"       << _entry.numberValue(_tag); break;
      default:                         kDebug() << "UDS_<UNKNOWN>:"    << _tag;
    } // switch

  return _entry;
} // KIONodeWrapper::toUDSEntry
