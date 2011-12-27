/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file
 * Implements the methods of class DBusClient. 
 * @author Christian Reiner
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
#include "client/dbus/dbus_client.h"
#include "utility/exception.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

const int timeout = 5000;
const int repeat = 6;

/*!
 * DBusClient::DBusClient
 * @brief Constructor of class DBusClient. 
 * @param service dbus specific service namespace
 * @param path dbus specific component path
 * @param interface dbus interface
 * Since this is a generic proxy there is not too much to setup.
 * Only thing is the interface object itself. 
 * @author Christian Reiner
 */
DBusClient::DBusClient ( const QString& service, const QString& path, const QString& interface )
{
  kDebug() << "constructing generic DBus client";
  kDebug() << service, path, interface;
  // setup bus connection details
  QDBusConnection _bus = QDBusConnection::sessionBus();
  this->m_interface = new QDBusInterface ( service, path, interface, _bus );
  if ( ! this->m_interface->isValid() )
    if  ( ! this->m_interface->lastError().isValid() )
      throw Exception ( Error(ERR_INTERNAL) );
    else
      throw Exception ( Error(ERR_INTERNAL), this->m_interface->lastError().message() );
  kDebug() << "connection to DBus successful.";
}

/*!
 * DBusClient::~DBusClient
 * @brief Destructor of class DBusClient. 
 * @author Christian Reiner
 */
DBusClient::~DBusClient ( )
{
  kDebug() << "destructing generic DBus client";
  delete this->m_interface;
}

/*!
 * DBusClient::resultSize
 * @brief Read-only access to the result a request produced. 
 * @return list of QVariants holding the requests result
 * Read-only access to the result a request produced. 
 * Used by deriving classes, might be extended by error handling. 
 * @author Christian Reiner
 */
QList<QVariant> DBusClient::result ( )
{
  return m_result;
} // DBusClient::result

/*!
 * DBusClient::resultSize
 * @brief Totel size of the requests result. 
 * @return size of request result
 * The actual size of the result a request produced. 
 * Used by deriving classes, might be extended by error handling. 
 * @author Christian Reiner
 */
int DBusClient::resultSize ( )
{
  return m_result.size();
} // DBusClient::resultSize

/*!
 * DBusClient::convertReturnValue
 * @brief Convert return value to a specified type. 
 * @param variant the variant to covert
 * @param _t the type to convert the variant to
 * @return the converted variant
 * Converts the representation of data in a result into a more convenient c++ format. 
 * Since the structure of a result depends on the request called and might actually
 * differ from situation to situation we use QVariants holding the data. 
 * This method is used by the calling scope as a service to convert any data held in a result into an expected format. 
 * Error handling is done as far as possible on a syntactical and type-specific level.
 * @author Christian Reiner
 */
QVariant& DBusClient::convertReturnValue ( QVariant &variant, QVariant::Type _t )
{
  kDebug() << QVariant::typeToName(_t);
  if ( ! variant.canConvert(_t) )
  {
    kDebug() << QString("trying to interpret variant of type '%1' as '%2' !?!").arg(variant.typeName()).arg(QVariant::typeToName(_t));
    throw Exception ( Error(ERR_INTERNAL), QVariant::typeToName(_t) );
  }
  if ( ! variant.convert(_t) )
  {
    kDebug() << QString("conversion of variant to type '%1' failed !").arg(QVariant::typeToName(_t));
    throw Exception ( Error(ERR_SLAVE_DEFINED), QVariant::typeToName(_t) );
  }
  return variant;
} // DBusClient::readReturnValue

/*!
 * DBusClient::called
 * @brief Generic wrapper of an actual request (call) to DBus. 
 * @param arg1 first dbus argument
 * @param arg2 second dbus argument
 * @param arg3 third dbus argument
 * @param arg4 fourth dbus argument
 * @param arg5 fifth dbus argument
 * @param arg6 sixth dbus argument
 * @param arg7 seventh dbus argument
 * @param arg8 eigth dbus argument
 * Basic error handling considers protocol and transport problems,
 * but there is no way to deal with the content received as a result. 
 * @author Christian Reiner
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
    throw Exception ( Error(ERR_SLAVE_DEFINED), _msg.errorMessage() );
  if ( QDBusMessage::ReplyMessage!=_msg.type() )
    throw Exception ( Error(ERR_SLAVE_DEFINED), i18n("DBus call did not result in a reply message.") );
  m_result = _msg.arguments();
  kDebug() << QString("read a result holding of %1 entries.").arg(m_result.size());
} // DBusClient::call
