/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file
 * Implements class KlipperBackend.
 * @see KlipperBackend
 * @author Christian Reiner
 */

#include <QDBusInterface>
#include <QDBusReply>
#include <QStringList>
#include <kio/global.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include "clipboard/klipper/klipper_backend.h"
#include "utility/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/*!
 * KlipperBackend::KlipperBackend
 * @brief Constructor of the backend part of the clipboard wrapper.
 * @param parent
 * DBusclient is the abstract communication client used in background
 * @author Christian Reiner
 */
KlipperBackend::KlipperBackend ( QObject* parent )
  : ClipboardBackend ( parent )
  , DBusClient ( "org.kde.klipper", "/klipper", "org.kde.klipper.klipper" )
{
  kDebug() << "constructing specialized DBus client of type 'klipper'";
} // KlipperBackend::KlipperBackend

/*!
 * KlipperBackend::~KlipperBackend
 * @brief Destructor of the backend part of the clipboard wrapper
 * @author Christian Reiner
 */
KlipperBackend::~KlipperBackend ( )
{
  kDebug() << "destructing specialized DBus client of type 'klipper'";
} // KlipperBackend::~KlipperBackend

/*!
 * KlipperBackend::clearClipboardContents
 * @brief Clears the currently active clipboard content. 
 * @author Christian Reiner
 */
void KlipperBackend::clearClipboardContents ( )
{
  kDebug();
  m_interface->call ( "clearClipboardContents" );
} // KlipperBackend::clearClipboardContents

/*!
 * KlipperBackend::clearClipboardHistory
 * @brief Removes all entries from the clipboard. 
 * @author Christian Reiner
 */
void KlipperBackend::clearClipboardHistory ( )
{
  kDebug();
  m_interface->call ( "clearClipboardHistory" );
} // KlipperBackend::clearClipboardHistory

/*!
 * KlipperBackend::getClipboardContents
 * @brief Retrieves the currently active clipboard content.
 * @return string holding the current clipboard content
 * @author Christian Reiner
 */
QString KlipperBackend::getClipboardContents ( )
{
  kDebug();
  call ( "getClipboardContents" );
  if ( 1 != m_result.size() )
    throw Exception ( Error(ERR_INTERNAL), i18n("DBus call did not return a single entry as expected.") );
  QString _entry = convertReturnValue(m_result.first(),QVariant::String).toString();
  kDebug() << QString("read clipboard content '%1%2'").arg(_entry.left(25)).arg((25>_entry.size())?"[...]":"");
  return _entry;
} // KlipperBackend::getClipboardContents

/*!
 * KlipperBackend::getClipboardHistoryMenu
 * @brief Retrieves all entries available in the clipboard.
 * @return list of string holding the clipboard history
 * @author Christian Reiner
 */
QStringList KlipperBackend::getClipboardHistoryMenu ( )
{
  kDebug();
  call ( "getClipboardHistoryMenu" );
  QStringList _entries = convertReturnValue(m_result.first(),QVariant::StringList).toStringList();
  kDebug() << QString("clipboard returned list holding %1 entries").arg(_entries.count());
  return _entries;
} // KlipperBackend::getClipboardHistoryMenu

/*!
 * KlipperBackend::getClipboardHistoryItem
 * @brief Retrieves a specific entry from the clipboard, indentified by its numeric index.
 * @param index numeric index if the entry to be retrieved.
 * @return string holding the content of the requested clipboard entry. 
 * @author Christian Reiner
 */
QString KlipperBackend::getClipboardHistoryItem ( int index )
{
  kDebug() << index << "/" << index-1; // the dbus service counts from 0, not from 1
  call ( "getClipboardHistoryItem", index-1 );
  if ( 1 != m_result.size() )
    throw Exception ( Error(ERR_INTERNAL), i18n("DBus call did not return a single entry as expected.") );
  QString _entry = convertReturnValue(m_result.first(),QVariant::String).toString();
  kDebug() << QString("read clipboard history item #%1: '%2%3'").arg(index).arg(_entry.left(25)).arg((25>_entry.size())?"[...]":"");
  return _entry;
} // KlipperBackend::getClipboardHistoryItem

/*!
 * KlipperBackend::setClipboardContents
 * @brief Sets the content of the currently active clipboard entry.
 * @param entry string to be set as the new clipboard content. 
 * @author Christian Reiner
 */
void KlipperBackend::setClipboardContents ( const QString& entry )
{
  kDebug() << entry;
  call ( "setClipboardContents", entry );
} // KlipperBackend::setClipboardContents

/*!
 * KlipperBackend::setClipboardHistory
 * @brief Replaces all clipboard entries by a list of new ones.
 * @param entries string list of new entries to be set in the clipboard
 * @author Christian Reiner
 */
void KlipperBackend::setClipboardHistory ( const QStringList& entries )
{
  // strategy: remove all entries and re-add everything in the correct order
  kDebug();
  call ( "clearClipboardHistory" );
  foreach ( const QString& _entry, entries )
    call ( "setClipboardContents", _entry );
  kDebug() << QString("populated clipboard history with %1 entries").arg(entries.size());
} // KlipperBackend::setClipboardHistory

#include "clipboard/klipper/klipper_backend.moc"
