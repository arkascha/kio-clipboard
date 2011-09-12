/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author: arkascha $
 * $Revision: 81 $
 * $Date: 2011-08-13 13:08:50 +0200 (Sat, 13 Aug 2011) $
 */
#include <kio/global.h>
#include <kdebug.h>
#include "clipboard/clipboard_backend.h"
#include "utility/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;


ClipboardBackend::ClipboardBackend ( QObject* parent )
  : QObject ( parent )
{
  kDebug() << "constructing specialized DBus client of type 'klipper'";
} // ClipboardBackend::ClipboardBackend

ClipboardBackend::~ClipboardBackend ()
{
  kDebug() << "destructing specialized DBus client of type 'klipper'";
} // ClipboardBackend::~ClipboardBackend

#include "clipboard/clipboard_backend.moc"
