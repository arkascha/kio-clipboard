/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <QDBusInterface>
#include <QDBusReply>
#include <QStringList>
#include <kio/global.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include "clipboards/klipper/klipper_backend.h"
#include "christian-reiner.info/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;


KlipperBackend::KlipperBackend ( QObject* parent )
  : ClipboardBackend ( parent )
  , DBusClient ( "org.kde.klipper", "/klipper", "org.kde.klipper.klipper" )
{
  kDebug() << "constructing specialized DBus client of type 'klipper'";
} // KlipperBackend::KlipperBackend

KlipperBackend::~KlipperBackend ()
{
  kDebug() << "destructing specialized DBus client of type 'klipper'";
} // KlipperBackend::~KlipperBackend

void KlipperBackend::clearClipboardContents ()
{
  kDebug();
  m_interface->call ( "clearClipboardContents" );
} // KlipperBackend::clearClipboardContents

void KlipperBackend::clearClipboardHistory ()
{
  kDebug();
  m_interface->call ( "clearClipboardHistory" );
} // KlipperBackend::clearClipboardHistory

QString KlipperBackend::getClipboardContents ()
{
  kDebug();
  call ( "getClipboardContents" );
  if ( 1 != m_result.size() )
    throw CRI::Exception ( Error(ERR_INTERNAL), i18n("DBus call did not return a single entry as expected.") );
  QString _entry = convertReturnValue(m_result.first(),QVariant::String).toString();
  kDebug() << QString("read clipboard content '%1%2'").arg(_entry.left(25)).arg((25>_entry.size())?"[...]":"");
  return _entry;
} // KlipperBackend::getClipboardContents

QStringList KlipperBackend::getClipboardHistoryMenu ()
{
  kDebug();
  call ( "getClipboardHistoryMenu" );
  QStringList _entries = convertReturnValue(m_result.first(),QVariant::StringList).toStringList();
  kDebug() << QString("clipboard returned list holding %1 entries").arg(_entries.count());
  return _entries;
} // KlipperBackend::getClipboardHistoryMenu

QString KlipperBackend::getClipboardHistoryItem ( int index )
{
  kDebug() << index;
  call ( "getClipboardHistoryMenuItem", index );
  if ( 1 != m_result.size() )
    throw CRI::Exception ( Error(ERR_INTERNAL), i18n("DBus call did not return a single entry as expected.") );
  QString _entry = convertReturnValue(m_result.first(),QVariant::String).toString();
  kDebug() << QString("read clipboard history item #%1: '%2%3'").arg(index).arg(_entry.left(25)).arg((25>_entry.size())?"[...]":"");
  return _entry;
} // KlipperBackend::getClipboardHistoryItem

void KlipperBackend::setClipboardContents ( const QString& entry )
{
  kDebug() << entry;
  call ( "setClipboardContents", entry );
} // KlipperBackend::setClipboardContents

void KlipperBackend::setClipboardHistory ( const QStringList& entries )
{
  // strategy: remove all entries and re-add everything in the correct order
  kDebug();
  call ( "clearClipboardHistory" );
  foreach ( const QString& _entry, entries )
    call ( "setClipboardContents", _entry );
  kDebug() << QString("populated clipboard history with %1 entries").arg(entries.size());
} // KlipperBackend::setClipboardHistory

#include "clipboards/klipper/klipper_backend.moc"
