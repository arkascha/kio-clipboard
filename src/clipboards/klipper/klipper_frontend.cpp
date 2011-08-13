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
#include "clipboards/klipper/klipper_frontend.h"
#include "clipboards/klipper/klipper_backend.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/**
 * All setup required is a DBus client we use as a proxy
 */
KlipperFrontend::KlipperFrontend ( const KUrl& url, const QString& name )
  : KIOClipboardWrapper ( url, name )
{
  kDebug() << "constructing specialized clipboard wrapper of type 'klipper'";
  m_backend = new KlipperBackend ();
} // constructor

/**
 * All cleanup required is to destroy the DBus client
 */
KlipperFrontend::~KlipperFrontend ()
{
  kDebug() << "destructing specialized clipboard wrapper of type 'klipper'";
  delete m_backend;
} // destructor

/**
 * reads a single clipboard entry (the content)
 * note that this is always a string, be it human-readable or not
 */
QString KlipperFrontend::getClipboardEntry ()
{
  kDebug();
  QString entry;
  // consult the worker client
  entry = m_backend->getClipboardContents();
  return entry;
} // KlipperFrontend::getClipboardEntry

/**
 * conveniently get all clipboard entries in a single call
 * this makes sense for refreshing and initializing
 */
QStringList KlipperFrontend::getClipboardEntries ()
{
  kDebug();
  QStringList entries;
  // consult the worker client
  entries = m_backend->getClipboardHistoryMenu ();
  kDebug() << QString("got list of %1 clipboard entries.").arg(entries.size());
  return entries;
} // KlipperFrontend::getClipboardEntries

/**
 * push a new entry onto the clipboard
 * since clipboards act as fifos this entry is typically pushed onto the front of the stack
 */
void KlipperFrontend::pushEntry ( const QString& entry )
{
  kDebug() << entry;
  m_backend->setClipboardContents ( entry );
  refreshNodes ( );
} // KlipperFrontend::pushEntry

/**
 * removes an entry from the clipboard
 * note that this works random-access, a feature that is not actually offered by a clipboard
 * we kind of emulate this feature instead by removing all entries and repopulating the clipboard
 * with all entries except that one to be removed
 */
void KlipperFrontend::delEntry ( const KUrl& url )
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
  m_backend->setClipboardHistory ( _remains );
  // now re-read all entries to have a clean buffer
  refreshNodes ( );
} // KlipperFrontend::delEntry
