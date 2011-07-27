/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $HeadURL$
 * $Author$
 * $Revision$
 * $Date$
 */
#include <kdebug.h>
#include <klocalizedstring.h>

#include "klipper/kio_clipboard_wrapper_klipper.h"
#include "klipper/dbus_client_klipper.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

KIOClipboardWrapperKlipper::KIOClipboardWrapperKlipper ( const KUrl& base )
  : KIOClipboardWrapper ( KIO_CLIPBOARD::KLIPPER, base )
{
  kDebug() << "constructing specialized clipboard wrapper of type 'klipper'";
  m_dbus = new DBusClientKlipper ();
} // constructor

KIOClipboardWrapperKlipper::~KIOClipboardWrapperKlipper ()
{
  kDebug() << "destructing specialized clipboard wrapper of type 'klipper'";
  delete m_dbus;
} // destructor

QString KIOClipboardWrapperKlipper::getClipboardEntry ()
{
  kDebug();
  QString entry;
  // consult the worker client
  entry = m_dbus->getClipboardContents();
  return entry;
} // KIOClipboardWrapperKlipper::getClipboardEntry

QStringList KIOClipboardWrapperKlipper::getClipboardEntries ()
{
  kDebug();
  QStringList entries;
  // consult the worker client
  entries = m_dbus->getClipboardHistoryMenu ();
  kDebug() << QString("got list of %1 clipboard entries.").arg(entries.size());
  return entries;
} // KIOClipboardWrapperKlipper::getClipboardEntries

void KIOClipboardWrapperKlipper::pushEntry ( QString entry )
{
  kDebug() << entry;
  m_dbus->setClipboardContents ( entry );
  refreshPool();
} // KIOClipboardWrapperKlipper::pushEntry

void KIOClipboardWrapperKlipper::delEntry ( const KUrl& url )
{
  kDebug() << url;
  QStringList _remains;
  UDSEntry _deliquent = findEntryByUrl ( url );
  // build a backup list of all remaining entries (as strings)
//  foreach ( UDSEntry _entry, pool )
  // reverse order required !
  for ( int _i=m_pool.size()-1; _i>=0; _i--)
  {
    const UDSEntry& _entry = m_pool.at(_i);
    if ( _entry.stringValue(UDSEntry::UDS_NAME)!=_deliquent.stringValue(UDSEntry::UDS_NAME) )
      _remains << _entry.stringValue ( UDSEntry::UDS_EXTRA );
  }
  // and (re-) add the backup list from above
  m_dbus->setClipboardHistory ( _remains );
  // now re-read all entries to have a clean buffer
  refreshPool();
} // KIOClipboardWrapperKlipper::delEntry

