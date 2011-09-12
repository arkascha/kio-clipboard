/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <QObject>
#include <QCryptographicHash>
#include <QVariant>
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <qjson/qobjecthelper.h>
#include <kdebug.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <klocalizedstring.h>
#include <klocale.h>
#include <kdatetime.h>
#include "utility/exception.h"
#include "kio_clipboard_protocol.h"
#include "node/node_wrapper.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/**
 * This constructs a node object that describes exactly one single entry on a clipboard in a passive and constant way.
 * No manipulations are offered, the purpose is to offer convenient methods to access data about such an entry.
 * - few synthactical configuration settings are defined first
 * - all basic data is collected and stored in private members
 * - primitive rules are used decide upon a few basic interpretations of the type of content in an entry
 */
NodeWrapper::NodeWrapper ( ClipboardFrontend* const clipboard,  int index, const QString& payload, QObject* parent )
  : QObject ( parent )
  , m_mappingNameCardinality ( clipboard->mappingNameCardinality() )
  , m_mappingNameLength      ( clipboard->mappingNameLength() )
  , m_mappingNamePattern     ( clipboard->mappingNamePattern() )
{
  kDebug() << index;
  QString _trimmed = payload.trimmed ( );
  m_index = index;
  m_size  = payload.size();
  // we do NOT request any datetime from files or URLs, so we can just set it plain here
  // reason is that usually we read the value from a history, except when we first access the object
  m_datetime = KDateTime::currentLocalDateTime();
  // fixed access rights currently, entries of local clipboards should only be accessible from inside the session itself
  m_access = 0400;
  // construct a valid file name, even for a payload that is a path or url
  m_name  = payload2name ( payload );
  // mark first entry in the list as the newest by using an overlay
  if ( 1==index )
    m_overlays << "emblem-new";
  // decide about the sematics ("meaning") of the content
  if ( _trimmed.isEmpty() )
  {
    m_semantics = KIO_CLIPBOARD::NodeWrapper::S_EMPTY;
    m_title     = "";
  }
  else if ( m_regEx["uri"].exactMatch(_trimmed) )
  {
    m_semantics = KIO_CLIPBOARD::NodeWrapper::S_URL;
    m_title     = payload2title ( payload );
    m_url       = KUrl ( _trimmed );
    m_link      = KUrl ( _trimmed );
    if ( m_url.isLocalFile() )
      m_path      = m_url.path ( );
  }
  else if ( m_regEx["path"].exactMatch(_trimmed) )
  {
    m_semantics = KIO_CLIPBOARD::NodeWrapper::S_FILE;
    m_title     = payload2title ( payload );
    m_url       = KUrl ( _trimmed );
    m_link      = KUrl ( _trimmed );
    m_path      = _trimmed;
  }
  else
  {
    m_semantics = KIO_CLIPBOARD::NodeWrapper::S_TEXT;
    m_title     = payload2title ( payload );
  }
  switch ( m_semantics )
  {
    case KIO_CLIPBOARD::NodeWrapper::S_EMPTY:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::mimeType("text/plain");
      m_overlays << "emblem-special";
      break;
    case KIO_CLIPBOARD::NodeWrapper::S_TEXT:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::findByContent(payload.toUtf8());
      // check if we can refine the sematics to something more specific
      // TODO: find some more generic way as an alternative to this list of test for recognized mimetypes
      if (  ("text/x-"==m_mimetype->name().left(7))
          ||(m_mimetype->is("text/css"))
          ||(m_mimetype->is("text/html"))
          ||(m_mimetype->is("text/sgml"))
          ||(m_mimetype->is("text/xml")) )
        m_semantics = KIO_CLIPBOARD::NodeWrapper::S_CODE;
      break;
    case KIO_CLIPBOARD::NodeWrapper::S_CODE:
      m_type     = S_IFREG;
      m_mimetype = KMimeType::findByContent(payload.toUtf8());
      break;
    case KIO_CLIPBOARD::NodeWrapper::S_FILE:
//      m_type     = S_IFREG;
      m_type     = S_IFLNK;
      m_mimetype = KMimeType::findByUrl(m_url);
      m_overlays << "emblem-symbolic-link";
      break;
    case KIO_CLIPBOARD::NodeWrapper::S_DIR:
      m_type     = S_IFDIR;
      m_mimetype = KMimeType::mimeType("inode/directory");
      m_overlays << "emblem-symbolic-link";
      break;
    case KIO_CLIPBOARD::NodeWrapper::S_LINK:
      m_type     = S_IFLNK;
      m_mimetype = KMimeType::findByUrl(m_url);
      m_overlays << "emblem-link";
      break;
    case KIO_CLIPBOARD::NodeWrapper::S_URL:
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
} // NodeWrapper::NodeWrapper

/**
 * Converts a nodes JSON notation into a fresh NodeWrapper object
 */
NodeWrapper::NodeWrapper ( const QByteArray& json, QObject* parent )
  : QObject ( parent )
{
  kDebug();
  fromJSON ( json );
} // NodeWrapper::toJSON

/**
 * Copy constructor, required for the Q_META_OBJECT system
 */
NodeWrapper::NodeWrapper ( const NodeWrapper& node, QObject* parent )
  : QObject ( parent )
{
  kDebug();
  m_index                  = node.m_index;
  m_title                  = node.m_title;
  m_size                   = node.m_size;
  m_mimetype               = node.m_mimetype;
  m_semantics              = node.m_semantics;
  m_name                   = node.m_name;
  m_url                    = node.m_url;
  m_link                   = node.m_link;
  m_path                   = node.m_path;
  m_type                   = node.m_type;
  m_icon                   = node.m_icon;
  m_overlays               = node.m_overlays;
  m_mappingNameCardinality = node.m_mappingNameCardinality;
  m_mappingNameLength      = node.m_mappingNameLength;
  m_mappingNamePattern     = node.m_mappingNamePattern;
} // NodeWrapper::NodeWrapper

/**
 * Copy constructor, required for the Q_META_OBJECT system
 */
NodeWrapper::NodeWrapper ( QObject* parent )
  : QObject ( parent )
{
  kDebug();
} // NodeWrapper::NodeWrapper

/**
 * Currently no cleanup to be done, since all such nodes have a passive character
 */
NodeWrapper::~NodeWrapper()
{
  kDebug();
} // NodeWrapper::~NodeWrapper

//==========

/**
 * we define a numerical index for every clipboard entry
 * this index names the entries position in the clipboard, since all clipboards work as a fifo
 * however when using this numerical index as part of names we have to take care of alphabetic sorting
 * therefore we construct a prettyIndex that is prepended with leading zeros so that all entries have a prettyIndex of same length
 */
QString NodeWrapper::prettyIndex ( ) const
{
  QString _pretty = QString("%1").arg(m_index,m_mappingNameCardinality,10,QChar('0'));
  kDebug() << _pretty;
  return _pretty;
} // NodeWrapper::prettyIndex

/**
 * returns the human readable description of a mimetype
 */
QString NodeWrapper::prettyMimetype ( ) const
{
  kDebug() << m_mimetype->comment();
  return m_mimetype->comment();
} // NodeWrapper::prettyMimetype

/**
 * since the internal 'semantics', the meaning of an entries content is very important
 * for how to handle such an entry we want to share this information with the user
 * for this purpose we translate a (numeric) semantics marker into a human readable string
 */
QString NodeWrapper::prettySemantics ( ) const
{
  QString _pretty;
  switch ( m_semantics )
  {
    case KIO_CLIPBOARD::NodeWrapper::S_EMPTY: _pretty = i18n ( "Empty" );     break;
    case KIO_CLIPBOARD::NodeWrapper::S_TEXT:  _pretty = i18n ( "Text" );      break;
    case KIO_CLIPBOARD::NodeWrapper::S_CODE:  _pretty = i18n ( "Code" );      break;
    case KIO_CLIPBOARD::NodeWrapper::S_FILE:  _pretty = i18n ( "File" );      break;
    case KIO_CLIPBOARD::NodeWrapper::S_DIR:   _pretty = i18n ( "Directory" ); break;
    case KIO_CLIPBOARD::NodeWrapper::S_LINK:  _pretty = i18n ( "Link");       break;
    case KIO_CLIPBOARD::NodeWrapper::S_URL:   _pretty = i18n ( "URL" );       break;
    default:                     _pretty = i18n ( "???" );
  } // switch
  kDebug() << _pretty;
  return _pretty;
} // NodeWrapper::prettySemantics

/**
 * each node must be represented by a name when offering it to the user
 * however entries usually do not have ordinary file names on a clipboard
 * they are identified either by a simple index, a (temporary) position or maybe something like a title
 * for our purpose we require a simple file name, therefore we generate one
 * the name is based on the passive data collected about a node and turned into something 'pretty'
 */
QString NodeWrapper::prettyName ( ) const
{
  // we construct something like this: "007(String): Es war einmal vor langer, langer Zeit [...]"
  QString _pretty = m_mappingNamePattern
                    // a leading numerical index, cardinality depends of the size of the set of nodes
                    .arg( prettyIndex() )
                    // linguistic type of content, like TEXT or CODE or URL
                    .arg( prettySemantics() )
                    // the whole alpha-numerical content
                    .arg( m_title );
  kDebug() << _pretty;
  return _pretty;
} // NodeWrapper::prettyName

/**
 * just a wrapper to present a URL an entry might point to in a human-readable form
 */
QString NodeWrapper::prettyUrl ( ) const
{
  QString _pretty = m_url.prettyUrl();
  kDebug() << _pretty;
  return _pretty;
} // NodeWrapper::prettyUrl

/**
 * printable form of datetime
 */
QString NodeWrapper::prettyDatetime ( ) const
{
  QString _pretty = KGlobal::locale()->formatDateTime ( m_datetime, KLocale::LongDate );
  kDebug() << _pretty;
  return _pretty;
} // NodeWrapper::prettyDatetime

//==========

/**
 * we dont store the while payload (that might be huge, whole files)
 * for a node we are usually just interesting in something 'close' to the payload, something recognizable
 * for example we use the payload as part of an entries 'name' at a few places to make the entry recognizable
 * for those situations we have to crop the payload since names shoulds not get too long
 */
QString NodeWrapper::payload2title ( const QString& payload )
{
  QString _title = payload.simplified();
  if ( m_mappingNameLength<_title.length() )
    _title = QString("%1[...]").arg(_title.left(m_mappingNameLength-5));
  kDebug() << _title;
  return _title;
} // NodeWrapper::payload2title

QString NodeWrapper::payload2name ( const QString& payload )
{
  return QString ( QCryptographicHash::hash(payload.toUtf8(), QCryptographicHash::Md5).toHex() );
} // NodeWrapper::payload2name

/**
 * keeping each node in a self defined and convenient object is fine for internal handling
 * however we have to translate such an object to present the entry to the outside world
 * this is done by translating an object to a constant UDSEntry as understood by the underlying KIO system
 */
UDSEntry NodeWrapper::toUDSEntry ( ) const
{
  kDebug() << "[" << m_index << "]:" << prettyName();
  UDSEntry _entry;
  _entry.insert( UDSEntry::UDS_NAME,               m_name );
  _entry.insert( UDSEntry::UDS_DISPLAY_NAME,       prettyName() );
  _entry.insert( UDSEntry::UDS_FILE_TYPE,          m_type );
  _entry.insert( UDSEntry::UDS_MIME_TYPE,          m_mimetype->name() );
  _entry.insert( UDSEntry::UDS_DISPLAY_TYPE,       m_mimetype->comment() );
  _entry.insert( UDSEntry::UDS_SIZE,               m_size );
  _entry.insert( UDSEntry::UDS_ACCESS,             m_access );
  _entry.insert( UDSEntry::UDS_MODIFICATION_TIME,  m_datetime.toTime_t() );
  if ( !m_path.isEmpty() )
    _entry.insert( UDSEntry::UDS_LOCAL_PATH,         m_path );
  if ( ! m_url.isEmpty() )
    _entry.insert( UDSEntry::UDS_TARGET_URL,         m_url.url() );
//  if ( ! m_link.isEmpty() )
//    _entry.insert( UDSEntry::UDS_LINK_DEST,          m_link.url() );
  if ( ! m_icon.isEmpty() )
    _entry.insert( UDSEntry::UDS_ICON_NAME,          m_icon );
  if ( ! m_overlays.isEmpty() )
    _entry.insert( UDSEntry::UDS_ICON_OVERLAY_NAMES, m_overlays.join(",") );

  // some intense debugging output...
  QList<uint> _tags = _entry.listFields ( );
  kDebug() << "list of defined UDS entry tags for entry" << prettyIndex() << ":";
  foreach ( uint _tag, _tags )
    switch ( _tag )
    {
      case UDSEntry::UDS_NAME:               kDebug() << "UDS_NAME:"               << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_DISPLAY_NAME:       kDebug() << "UDS_DISPLAY_NAME:"       << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_FILE_TYPE:          kDebug() << "UDS_FILE_TYPE:"          << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_MIME_TYPE:          kDebug() << "UDS_MIME_TYPE:"          << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_DISPLAY_TYPE:       kDebug() << "UDS_DISPLAY_TYPE:"       << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_LOCAL_PATH:         kDebug() << "UDS_LOCAL_PATH:"         << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_URL:                kDebug() << "UDS_URL:"                << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_TARGET_URL:         kDebug() << "UDS_TARGET_URL:"         << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_LINK_DEST:          kDebug() << "UDS_LINK_DEST:"          << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_SIZE:               kDebug() << "UDS_SIZE:"               << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_GUESSED_MIME_TYPE:  kDebug() << "UDS_GUESSED_MIME_TYPE:"  << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_ACCESS:             kDebug() << "UDS_ACCESS:"             << _entry.numberValue(_tag); break;
      case UDSEntry::UDS_ICON_NAME:          kDebug() << "UDS_ICON_NAME:"          << _entry.stringValue(_tag); break;
      case UDSEntry::UDS_ICON_OVERLAY_NAMES: kDebug() << "UDS_ICON_OVERLAY_NAMES:" << _entry.stringValue(_tag); break;
      default:                               kDebug() << "UDS_<UNKNOWN>:"          << _tag;
    } // switch
  return _entry;
} // NodeWrapper::toUDSEntry

/**
 * Converts a node into JSON notation, complemented by the constructor NodeWrapper::NodeWrapper(JSON)
 */
QByteArray NodeWrapper::toJSON ( ) const
{
  kDebug() << m_name;
  QVariantMap _object = QJson::QObjectHelper::qobject2qvariant( this );
  QJson::Serializer _serializer;
  return _serializer.serialize ( _object );
} // NodeWrapper::toJSON

NodeWrapper& NodeWrapper::fromJSON ( const QByteArray& json )
{
  kDebug();
  QJson::Parser parser;
  bool ok;
  QVariant _properties = parser.parse ( json, &ok ).toMap();
  if ( ! ok )
    throw Exception ( Error(ERR_INTERNAL), "Failed to deserialize json notation of node" );
  QJson::QObjectHelper::qvariant2qobject ( _properties.toMap(), this );
//  setSemantics(1);
  return *this;
} // NodeWrapper::fromJSON