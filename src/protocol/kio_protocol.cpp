/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <QFile>
#include <kmimetype.h>
#include <kio/netaccess.h>
#include <kdebug.h>
#include "utility/exception.h"
#include "clipboard/klipper/klipper_frontend.h"
#include "kio_protocol.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

//==========

/**
 * The standard constructor as called by the main loop setup at slace setup.
 * Besides the typical arguments required for a slave it requires a pointer to a clipboard object.
 * That object is used as the implementation of the specialized, that is specific routines required to communicate with an existing clipboard application. 
 */
KIOProtocol::KIOProtocol ( const QByteArray& pool, const QByteArray& app, ClipboardFrontend* const clipboard )
  : SlaveBase ( clipboard->protocol().toUtf8(), pool, app )
  , m_clipboard ( clipboard )
{
  kDebug() << "constructing protocol" << clipboard->protocol();
} // KIOProtocol::KIOProtocol

KIOProtocol::~KIOProtocol ( )
{
  kDebug() << "destructing protocol";
} // KIOProtocol::~KIOProtocol

/**
 * This is the work horse that takes over when the generic code in copy() has devided wether to deny or grant this action to be undertaken.
 * copy() will only call this for the copyFromFile feature for slaves / protocols. 
 * There are two central ways to copy a file:
 * 1.) push the files content onto the clipboard, this makes sense for human readable texts, typically for programming and text processing
 * 2.) push a reference to the file onto the clipboard, this is better for all cases where the content of the file itself should not be presented to the user.
 * Consequently there are two criterias the cedision is based on:
 * a) the size of the files content is compared to the maximum entry size as defined by the clipboard wrapper object
 * b) the type of content by looking closely at the detected mimetype
 * When a reference appears to make more sense then a copy the user is prompted to acknowledge or deny that switch. 
 */
/*
void KIOProtocol::copyFile ( const KUrl& url )
{
  kDebug() << src->url().prettyUrl() << dest.prettyUrl();
  KUrl _new_dest = dest;
  switch ( src->semantics() )
  {
    case KIO_CLIPBOARD::S_EMPTY:
    case KIO_CLIPBOARD::S_TEXT:
    case KIO_CLIPBOARD::S_CODE:
      _new_dest.setFileName( src->prettyName() );
      // TODO: suggest name to user, ask for final name or CANCEL
      copyToFile_Content ( src, _new_dest );
      return;
    case KIO_CLIPBOARD::S_FILE:
      _new_dest.setFileName( src->url().fileName() );
      kDebug() << "creating file '" << _new_dest.path() << "'";
      copyToFile_Content ( src, _new_dest );
    case KIO_CLIPBOARD::S_DIR:
      _new_dest.setFileName( src->url().fileName() );
      kDebug() << "creating directory '" << _new_dest.path() << "'";
      copyToFile_Link ( src, _new_dest );
      return;
    case KIO_CLIPBOARD::S_URL:
      _new_dest.setFileName( src->url().fileName() );
      kDebug() << "creating directory '" << _new_dest.path() << "'";
      copyToFile_Content ( src, _new_dest );
      return;
    default:
      throw Exception ( Error(ERR_INTERNAL_SERVER), src->url().prettyUrl() );
  }
} // KIOProtocol::copyToFile
*/
void KIOProtocol::copyFromFile ( const KUrl& src,  const KUrl& dest )
{
  kDebug() << src << dest;
  int _choice;
  // strategy: for text based mime types copy _content_ of the file, otheriwse create a reference, which means copy the path/url
  KMimeType::Ptr _mimetype = KMimeType::findByUrl ( src, 0, TRUE ); // TODO: file mode as second argument
  if ( _mimetype->isBinaryData(src.fileName()) || ! _mimetype->is("text/plain") )
  {
    kDebug() << "file content is binary data, creating a reference instead";
    _choice = messageBox ( WarningContinueCancel,
                           i18n("For the type of file '%1' creating a reference makes more sense. Is that what you want ?",
                                src.fileName()),
                           i18n("Create reference instead ?"),
                           i18n("&Yes"),
                           i18n("&No") );
    switch ( _choice )
    {
      case 0:  throw Exception ( Error(ERR_SLAVE_DEFINED), "Failed to request confirmation from user." );
      case 2:  throw Exception ( Error(ERR_USER_CANCELED), src.prettyUrl() );
      default: kDebug() << "creating file reference instead upon user request";
    }
    copyFromFile_Reference ( src );
  }
  else if ( src.isLocalFile() && src.directory()==src.path() )
  {
    kDebug() << "file is actually a directory, suggesting a link instead";
    _choice = messageBox ( WarningContinueCancel,
                           i18n("'%1' is a directory, I suggest to create a link instead. \nIs that what you want ?",
                                src.fileName()),
                           i18n("Link directory instead ?"),
                           i18n("&Yes"),
                           i18n("&No") );
    switch ( _choice )
    {
      case 0:  throw Exception ( Error(ERR_SLAVE_DEFINED), "Failed to request confirmation from user." );
      case 2:  throw Exception ( Error(ERR_USER_CANCELED), src.prettyUrl() );
      default: kDebug() << "creating link to directory instead upon user request";
    }
    copyFromFile_Reference ( src );
  }
  else
  {
    kDebug() << "file content is text data, copying content";
    QFile _file ( src.path() );
    // check file size BEFORE all other actions
    if ( _file.size()<m_clipboard->limit() )
    {
      copyFromFile_Content ( src.path() );
    }
    else
    {
      kDebug() << "payload size exceeds target limit, offering reference creation instead";
      _choice = messageBox ( WarningContinueCancel,
                             i18n("The size of file '%1' exceeds the clipboards limit, creating a file reference instead. \nIs that what you want ?",
                                  src.fileName()),
                             i18n("Create reference instead ?"),
                             i18n("&Yes"),
                             i18n("&No") );
      switch ( _choice )
      {
        case 0:
          kDebug() << "communication error whilst asking user for confirmation";
          throw Exception ( Error(ERR_SLAVE_DEFINED), "Failed to request confirmation from user." );
        case 2:
          kDebug() << "user cancelled alternate reference action upon request";
          throw Exception ( Error(ERR_USER_CANCELED), src.prettyUrl() );
      }
      kDebug() << "creating reference to file since the size exceeds the clipboards limit";
      copyFromFile_Reference ( src.path() ); // file MUST be a local file if <copy> is requested by the calling applciation
      return;
    }
  }
} // KIOProtocol::copyFromFile

/**
 * Pushes a reference to a file onto the clipboard.
 */
/*
void KIOProtocol::copyToFile_Reference ( const NodeWrapper* src, const KUrl& dest )
{
  kDebug() << src->prettyName() << dest.prettyUrl();
  if ( FALSE==NetAccess::dircopy(src->url(),dest,NULL) )
    throw Exception ( NetAccess::lastError(), NetAccess::lastErrorString() );
} // KIOProtocol::copyToFile_Reference

void KIOProtocol::copyToFile_Content ( const NodeWrapper* src, const KUrl& dest )
{
  kDebug() << src->prettyName() << dest.prettyUrl();
  QFile _file ( dest.path() );
  if ( ! _file.open(QIODevice::WriteOnly) )
    throw Exception ( Error(ERR_CANNOT_OPEN_FOR_WRITING), dest.prettyUrl() );
  // write payload into file
  int _result = _file.write ( src->payload().toUtf8() );
  _file.close();
  if ( -1==_result )
    throw Exception ( Error(ERR_COULD_NOT_WRITE), dest.prettyUrl() );
} // KIOProtocol::copyToFile_Content
*/

void KIOProtocol::copyFromFile_Reference ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  if ( url.isLocalFile() )
    m_clipboard->pushEntry ( url.toLocalFile() );
  else
    m_clipboard->pushEntry ( url.prettyUrl() );
} // KIOProtocol::copyFromFile_reference

/**
 * Pushes a reference to a file onto the clipboard.
 */
void KIOProtocol::copyFromFile_Reference ( const QString& path )
{
  kDebug() << path;
  m_clipboard->pushEntry ( path );
} // KIOProtocol::copyFromFile_Reference

/**
 * Pushes the content of a file onto the clipboard.
 * There is no check for filesize, this has already been dealt with in the calling copyFile()
 */
void KIOProtocol::copyFromFile_Content ( const KUrl& url )
{
  kDebug() << url.prettyUrl();
  // open file and buffer content as payload for a 'push'
  QByteArray _payload, _buffer;
  QFile _file ( url.path() );
  if ( ! _file.open(QIODevice::ReadOnly) )
    throw Exception ( Error(ERR_COULD_NOT_READ), url.prettyUrl() );
  // read content into buffer, push buffer onto clipboard
  do
  {
    _buffer   = _file.read ( 1024 );
    _payload += _buffer;
  } while ( ! _file.atEnd() && _buffer!=QByteArray() );
  _file.close();
  m_clipboard->pushEntry ( QString(_payload) );
} // KIOProtocol::copyFromFileContent
