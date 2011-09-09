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
 * detection of availability of klipper clipboard (in local session)
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
      kDebug() << "detected available clipboard of type 'KLIPPER' with url 'klipper:/'";
      _clipboards << new KlipperFrontend ( KUrl("klipper:/"), "klipper" );
    }
  }
  kDebug() << "detected" << _clipboards.count() << "available clipboards of type 'KLIPPER'";
  return _clipboards;
} // KlipperFrontend::detectClipboards

/**
 * All setup required is a DBus client we use as a proxy
 */
KlipperFrontend::KlipperFrontend ( const KUrl& url, const QString& name )
  : ClipboardFrontend ( url, name )
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
 * reads current clipboard entry (the content)
 * note that this is always a string, be it human-readable or not
 */
QString KlipperFrontend::getClipboardEntry ( )
{
  kDebug();
  return m_backend->getClipboardContents ( );
} // KlipperFrontend::getClipboardEntry

/**
 * reads single clipboard entry (the content) at a specified position
 * note that this is always a string, be it human-readable or not
 */
QString KlipperFrontend::getClipboardEntry ( int index )
{
  kDebug();
  return m_backend->getClipboardHistoryItem ( index );
} // KlipperFrontend::getClipboardEntry

/**
 * conveniently get all clipboard entries in a single call
 * this makes sense for refreshing and initializing
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
  throw CRI::Exception ( Error(ERR_UNSUPPORTED_ACTION), url.prettyUrl() );
} // KlipperFrontend::delEntry
