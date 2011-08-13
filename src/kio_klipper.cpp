/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <stdlib.h>
#include <unistd.h>

#include <QCoreApplication>
#include <kcomponentdata.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include "kio_klipper_protocol.h"
#include "about_klipper.data"

extern "C" { int KDE_EXPORT kdemain(int argc, char **argv); }
int kdemain( int argc, char **argv )
{
  KAboutData aboutData ( ABOUT_APP_NAME,
                         ABOUT_CATALOG_NAME,
                         ki18n(ABOUT_PROGRAM_NAME),
                         ABOUT_VERSION,
                         ki18n(ABOUT_DESCRIPTION),
                         ABOUT_LICENCE_TYPE,
                         ki18n(ABOUT_COPYRIGHT),
                         ki18n(ABOUT_INFORMATION),
                         ABOUT_WEBPAGE,
                         ABOUT_EMAIL );
  KComponentData componentData ( aboutData );

  QCoreApplication app( argc, argv );

  if (argc != 4)
  {
    fprintf(stderr, i18n("Usage: ").arg("kio_klipper protocol domain-socket1 domain-socket2\n").toUtf8() );
    exit(-1);
  }

  kDebug() << QString("started kio slave '%1' with PID %2").arg(argv[0]).arg(getpid());
  KIO_CLIPBOARD::KIOKlipperProtocol slave(argv[2], argv[3]);
  slave.dispatchLoop();

  kDebug() << "slave done";
  return ( 0 );
} // kdemain

