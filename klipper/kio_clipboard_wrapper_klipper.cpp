/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <kdebug.h>
#include <klocalizedstring.h>
#include "christian-reiner.info/exception.h"
#include "klipper/kio_clipboard_wrapper_klipper.h"
#include "klipper/dbus_client_klipper.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

KIOClipboardWrapperKlipper::KIOClipboardWrapperKlipper ( const KUrl& url, const QString& name )
  : KIOClipboardWrapper ( url, name )
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

void KIOClipboardWrapperKlipper::pushEntry ( const QString& entry )
{
  kDebug() << entry;
  m_dbus->setClipboardContents ( entry );
  refreshNodes ( );
} // KIOClipboardWrapperKlipper::pushEntry

void KIOClipboardWrapperKlipper::delEntry ( const KUrl& url )
{
  kDebug() << url;
  const KIONodeWrapper* const _deliquent = findNodeByUrl ( url );
  // build a backup list of all remaining entries (as strings)
  // we use prepend 'cause we need reverse order !
  QStringList _remains;
  foreach ( const KIONodeWrapper* _entry, m_nodes )
//  foreach ( const KIONodeWrapper* _entry, m_nodes.values() )
    if ( _entry->payload()!=_deliquent->payload() )
      _remains.prepend ( _entry->payload() );
  // and (re-) add the backup list from above
  m_dbus->setClipboardHistory ( _remains );
  // now re-read all entries to have a clean buffer
  refreshNodes ( );
} // KIOClipboardWrapperKlipper::delEntry

