/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author: arkascha $
 * $Revision: 81 $
 * $Date: 2011-08-13 13:08:50 +0200 (Sat, 13 Aug 2011) $
 */

/*!
 * @file
 * Implements the methods of the generic class ClipboardBackend. 
 * @see ClipboardBackend
 * @author Christian Reiner
 */

#include <kio/global.h>
#include <kdebug.h>
#include "clipboard/clipboard_backend.h"
#include "utility/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/*!
 * ClipboardBackend::ClipboardBackend
 * @brief Constructor of generic class ClipboardBackend
 * @param parent parent object
 * @author Christian Reiner
 */
ClipboardBackend::ClipboardBackend ( QObject* parent )
  : QObject ( parent )
{
  kDebug() << "constructing specialized DBus client of type 'klipper'";
} // ClipboardBackend::ClipboardBackend

/*!
 * ClipboardBackend::~ClipboardBackend
 * @brief Destructor of generic class ClipboardBackend
 * @author Christian Reiner
 */
ClipboardBackend::~ClipboardBackend ()
{
  kDebug() << "destructing specialized DBus client of type 'klipper'";
} // ClipboardBackend::~ClipboardBackend

#include "clipboard/clipboard_backend.moc"
