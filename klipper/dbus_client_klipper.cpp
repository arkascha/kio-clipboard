#include <QDBusInterface>
#include <QDBusReply>
#include <QStringList>
#include <kio/global.h>
#include <kdebug.h>
#include <klocalizedstring.h>

#include "klipper/dbus_client_klipper.h"
#include "christian-reiner.info/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;


DBusClientKlipper::DBusClientKlipper ( QObject* _parent )
  : DBusClient ( _parent )
{
  kDebug() << "constructing specialized DBus client of type 'klipper'";
  setupInterface ( "org.kde.klipper", "/klipper", "org.kde.klipper.klipper" );
} // DBusClientKlipper::DBusClientKlipper

DBusClientKlipper::~DBusClientKlipper ()
{
  kDebug() << "destructing specialized DBus client of type 'klipper'";
  delete this->m_interface;
} // DBusClientKlipper::~DBusClientKlipper

void DBusClientKlipper::clearClipboardContents ()
{
  kDebug();
  m_interface->call ( "clearClipboardContents" );
} // DBusClientKlipper::clearClipboardContents

void DBusClientKlipper::clearClipboardHistory ()
{
  kDebug();
  m_interface->call ( "clearClipboardHistory" );
} // DBusClientKlipper::clearClipboardHistory

QString DBusClientKlipper::getClipboardContents ()
{
  kDebug();
  call ( "getClipboardContents" );
  if ( 1 != m_result.size() )
    throw CRI::Exception ( Error(ERR_INTERNAL), i18n("DBus call did not return a single entry as expected.") );
  QString _entry = convertReturnValue(m_result.first(),QVariant::String).toString();
  kDebug() << QString("read clipboard content '%1%2'").arg(_entry.left(25)).arg((25>_entry.size())?"[...]":"");
  return _entry;
} // DBusClientKlipper::getClipboardContents

QStringList DBusClientKlipper::getClipboardHistoryMenu ()
{
  kDebug();
  call ( "getClipboardHistoryMenu" );
  QStringList _entries = convertReturnValue(m_result.first(),QVariant::StringList).toStringList();
  kDebug() << QString("clipboard returned list holding %1 entries").arg(_entries.count());
  return _entries;
} // DBusClientKlipper::getClipboardHistoryMenu

QString DBusClientKlipper::getClipboardHistoryItem ( int index )
{
  kDebug() << index;
  call ( "getClipboardHistoryMenuItem", index );
  if ( 1 != m_result.size() )
    throw CRI::Exception ( Error(ERR_INTERNAL), i18n("DBus call did not return a single entry as expected.") );
  QString _entry = convertReturnValue(m_result.first(),QVariant::String).toString();
  kDebug() << QString("read clipboard history item #%1: '%2%3'").arg(index).arg(_entry.left(25)).arg((25>_entry.size())?"[...]":"");
  return _entry;
} // DBusClientKlipper::getClipboardHistoryItem

void DBusClientKlipper::setClipboardContents ( QString &entry )
{
  kDebug() << entry;
  call ( "setClipboardContents", entry );
} // DBusClientKlipper::setClipboardContents

void DBusClientKlipper::setClipboardHistory ( QStringList &entries )
{
  // strategy: remove all entries and re-add everything in the correct order
  kDebug();
  call ( "clearClipboardHistory" );
  foreach ( QString _entry, entries )
    call ( "setClipboardContents", _entry );
  kDebug() << QString("populated clipboard history with %1 entries").arg(entries.size());
} // DBusClientKlipper::setClipboardHistory

#include "klipper/dbus_client_klipper.moc"
