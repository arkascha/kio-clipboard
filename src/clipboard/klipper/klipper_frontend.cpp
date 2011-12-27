/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file
 * Implements class KlipperFrontend.
 * @see KlipperFrontend
 * @author Christian Reiner
 */

#include <kdebug.h>
#include <klocalizedstring.h>
#include "utility/exception.h"
#include "clipboard/klipper/klipper_frontend.h"
#include "clipboard/klipper/klipper_backend.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/*!
 * KlipperFrontend::detectClipboards
 * @brief Detection of availability of a clipboard of type 'klipper' (in local session).
 * The availability is detected by the presence of a dbus service 'org.kde.klipper'. 
 * @param dbus dbus object used for communication
 * @return list of created ClipboardFrontend objects
 * @author Christian Reiner
 */
QList<const ClipboardFrontend*> KlipperFrontend::detectClipboards ( DBusClient& dbus )
{
  QList<const ClipboardFrontend*> _clipboards;
  dbus.call ( "ListNames" );
  const QStringList _names = dbus.convertReturnValue(dbus.result().first(),QVariant::StringList).toStringList();
  // now add entries one by one
  foreach (const QString& _name, _names )
  {
    if ( "org.kde.klipper"==_name )
    {
      kDebug() << "detected available clipboard of type 'KLIPPER', chosing url 'klipper:/'";
      _clipboards << new KlipperFrontend ( KUrl("klipper:/"), "klipper" );
    }
  }
  kDebug() << "detected" << _clipboards.count() << "available clipboards of type 'KLIPPER'";
  return _clipboards;
} // KlipperFrontend::detectClipboards

/*!
 * KlipperFrontend::KlipperFrontend
 * @brief Constructor of class KlipperFrontend. 
 * Nearly all setup required is done by the generic frontend class this class is derived from.
 * Only thing left is to setup a type specific backend object. 
 * @param url url of the clipboard node
 * @param name visible name of the clipboard node
 * @author Christian Reiner
 */
KlipperFrontend::KlipperFrontend ( const KUrl& url, const QString& name )
  : ClipboardFrontend ( url, name )
{
  kDebug() << "constructing specialized clipboard wrapper of type 'klipper'";
  m_backend = new KlipperBackend ();
} // constructor

/*!
 * KlipperFrontend::~KlipperFrontend
 * @brief Destructor of class KlipperFrontend
 * All cleanup required is to destroy the private backend object.
 * @author Christian Reiner
 */
KlipperFrontend::~KlipperFrontend ( )
{
  kDebug() << "destructing specialized clipboard wrapper of type 'klipper'";
  delete m_backend;
} // destructor

/*!
 * KlipperFrontend::getClipboardEntry
 * @brief Reads current clipboard entry (the content). 
 * Note that this is always of type string, be it human-readable or not.
 * @author Christian Reiner
 */
QString KlipperFrontend::getClipboardEntry ( )
{
  kDebug();
  return m_backend->getClipboardContents ( );
} // KlipperFrontend::getClipboardEntry

/*!
 * KlipperFrontend::getClipboardEntry
 * @brief Reads single clipboard entry (the content) at a specified position.
 * @param index numerical index of the requested clipboard entry
 * @return string holding the requested clipboard entry
 * Note that this is always a string, be it human-readable or not. 
 * @author Christian Reiner
 */
QString KlipperFrontend::getClipboardEntry ( int index )
{
  kDebug();
  return m_backend->getClipboardHistoryItem ( index );
} // KlipperFrontend::getClipboardEntry

/*!
 * KlipperFrontend::getClipboardEntries
 * @brief Conveniently get all clipboard entries in a single call.
 * @return string list holding all entries available in the clipboard
 * This makes sense for a complete refresh or initialization.
 * @author Christian Reiner
 */
QStringList KlipperFrontend::getClipboardEntries ( )
{
  kDebug();
  QStringList entries;
  // consult the worker client
  entries = m_backend->getClipboardHistoryMenu ( );
  kDebug() << QString( "got list of %1 clipboard entries.").arg(entries.size() );
  return entries;
} // KlipperFrontend::getClipboardEntries

/*!
 * KlipperFrontend::pushEntry
 * @brief Push a new entry onto the clipboard.
 * @param entry string to be added to the clipboard as a new entry
 * Since clipboard histories act as FIFOs this entry is typically pushed onto the front of the queue.
 * @author Christian Reiner
 */
void KlipperFrontend::pushEntry ( const QString& entry )
{
  kDebug() << entry;
  m_backend->setClipboardContents ( entry );
  refreshNodes ( );
} // KlipperFrontend::pushEntry

/*!
 * KlipperFrontend::delEntry
 * @brief Removes an entry from the clipboard.
 * @param url url of the entry to be removed
 * Note that this works random-access, a feature that is not actually offered by the klipper application.
 * We kind of emulate this feature instead by removing all entries and repopulating the clipboard with all entries except that one to be removed.
 * @author Christian Reiner
 */
void KlipperFrontend::delEntry ( const KUrl& url )
{
  kDebug() << url;
  throw Exception ( Error(ERR_UNSUPPORTED_ACTION), url.prettyUrl() );
} // KlipperFrontend::delEntry
