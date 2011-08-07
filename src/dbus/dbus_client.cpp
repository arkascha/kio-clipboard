/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>
#include <QDBusInterface>
#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <knotification.h>
#include <KUrl>
#include <KProcess>
#include <kdebug.h>
#include <KMimeType>
#include <KMessageBox>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <klocalizedstring.h>

#include "dbus/dbus_client.h"
#include "christian-reiner.info/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

const int timeout = 5000;
const int repeat = 6;

/**
 * Constructor
 * nothing special to setup since this is a generic proxy
 */
DBusClient::DBusClient ( QObject* _parent )
  : QObject ( _parent )
{
  kDebug() << "constructing generic DBus client";
}

/**
 * Destructor
 * nothing to cleanup
 */
DBusClient::~DBusClient ( )
{
  kDebug() << "destructing generic DBus client";
}

/**
 * TODO: move this into the destructor
 */
void DBusClient::cleanup ()
{
  // * de-register any signals from DBus
  // * clean detach from DBus
  delete m_interface;
} // DBusClient::cleanup

/**
 * TODO: move this into the constructor
 */
void DBusClient::setupInterface ( const QString& service, const QString& path, const QString& interface )
{
  kDebug() << service, path, interface;
  // setup bus connection details
  QDBusConnection _bus = QDBusConnection::sessionBus();
  this->m_interface = new QDBusInterface ( service, path, interface, _bus, this );
  if ( ! this->m_interface->isValid() )
    if  ( ! this->m_interface->lastError().isValid() )
      throw CRI::Exception ( Error(ERR_INTERNAL) );
    else
      throw CRI::Exception ( Error(ERR_INTERNAL), this->m_interface->lastError().message() );
  kDebug() << "connection to DBus successful.";
} // DBusClient::setupInterface

/**
 * read-only access to the result a request produced
 * used by deriving classes, might be extended by error handling
 */
QList<QVariant> DBusClient::result ( )
{
  return m_result;
} // DBusClient::result

/**
 * the actual size of the result a request produced
 * used by deriving classes, might be extended by error handling
 */
int DBusClient::resultSize ( )
{
  return m_result.size();
} // DBusClient::resultSize

/**
 * converts the representation of data in a result into a more convenient c++ format
 * since the structure of a result depends on the request called and might actually
 * differ from situation to situation we use QVariants to holds the data
 *
 * this method is used by the calling scope as a service to convert a data held in a result into an expected format
 * error handling is done as far as possible (syntactically and type-specific)
 */
QVariant& DBusClient::convertReturnValue ( QVariant &variant, QVariant::Type _t )
{
  kDebug() << QVariant::typeToName(_t);
  if ( ! variant.canConvert(_t) )
  {
    kDebug() << QString("trying to interpret variant of type '%1' as '%2' !?!").arg(variant.typeName()).arg(QVariant::typeToName(_t));
    throw CRI::Exception ( Error(ERR_INTERNAL), QVariant::typeToName(_t) );
  }
  if ( ! variant.convert(_t) )
  {
    kDebug() << QString("conversion of variant to type '%1' failed !").arg(QVariant::typeToName(_t));
    throw CRI::Exception ( Error(ERR_SLAVE_DEFINED), QVariant::typeToName(_t) );
  }
  return variant;
} // DBusClient::readReturnValue

/**
 * generic wrapper of an actual request (call) to DBus
 * basic error handling considers protocol and transport problems,
 * but there is no way to deal with the content received as a result
 */
void DBusClient::call ( const QString method, const QVariant & arg1,
                                              const QVariant & arg2,
                                              const QVariant & arg3,
                                              const QVariant & arg4,
                                              const QVariant & arg5,
                                              const QVariant & arg6,
                                              const QVariant & arg7,
                                              const QVariant & arg8 )
{
  kDebug() << method << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8;
  QDBusMessage _msg = m_interface->call ( method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 );
  // error checking
  if ( QDBusMessage::ErrorMessage==_msg.type() )
    throw CRI::Exception ( Error(ERR_SLAVE_DEFINED), _msg.errorMessage() );
  if ( QDBusMessage::ReplyMessage!=_msg.type() )
    throw CRI::Exception ( Error(ERR_SLAVE_DEFINED), i18n("DBus call did not result in a reply message.") );
  m_result = _msg.arguments();
  kDebug() << QString("read a result holding of %1 entries.").arg(m_result.size());
} // DBusClient::call

#include "dbus/dbus_client.moc"
