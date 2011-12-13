/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file Implementation of class NodeList
 * @see NodeList
 * @author Christian Reiner
 */

#include <QVariant>
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <kdebug.h>
#include "utility/exception.h"
#include "node/node_wrapper.h"
#include "node/node_list.h"

using namespace KIO;
using namespace KIO_CLIPBOARD;

/*!
 * NodeList::toUDSEntryList
 * @brief Creates a UDSEntryList to describe all nodes contained in the list.
 * @return UDSEntryList
 * @author Christian Reiner
 */
UDSEntryList NodeList::toUDSEntryList ( ) const
{
  UDSEntryList _entries;
  foreach ( const NodeWrapper* const& _node, m_nodes )
    _entries << _node->toUDSEntry();
  kDebug() << "created list holding" << _entries.size() << "nodes";
  return _entries;
} // NodeList::toUDSEntry

/*!
 * NodeList::toJSON
 * @brief Serializes a list of nodes to JSON data
 * @return QByteArray holding the JSON serialized data
 * @author Christian Reiner
 */
QByteArray NodeList::toJSON ( ) const
{
  kDebug() << "creating JSON notation of node list holding" << m_nodes.size() << "entries";
  // FIXME: guess this wont work since the map holds only pointers, not objects as values
  QVariantMap _nodes;
  NodeList::const_iterator _iterator;
  for ( _iterator=m_nodes.begin(); _iterator!=m_nodes.end(); _iterator++ )
    _nodes.insert ( _iterator.key(), _iterator.value()->toJSON() );
  QJson::Serializer _serializer;
  return _serializer.serialize ( _nodes );
} // NodeList::toJSON

/*!
 * NodeList::fromJSON
 * @brief Constructs a list of nodes as described by the JSON data
 * @param json JSON serialized node data
 * @return list of node objects
 * @author Christian Reiner
 */
NodeList& NodeList::fromJSON ( const QByteArray& json )
{
  kDebug();
  QJson::Parser parser;
  bool ok;
  QVariantMap _nodes = parser.parse ( json, &ok ).toMap();
  if ( ! ok )
    throw Exception ( Error(ERR_INTERNAL), "Failed to deserialize json notation of node list" );
  // create nodes one by one and push them into the cleared list
  m_nodes.clear ( );
  QVariantMap::iterator _iterator;
  for ( _iterator=_nodes.begin(); _iterator!=_nodes.end(); _iterator++ )
    m_nodes.insert ( _iterator.key(), new NodeWrapper(_iterator.value().toByteArray()) );
  kDebug() << "created node list holding" << m_nodes.size() << "entries from JSON notation";
  return *this;
} // NodeList::fromJSON
