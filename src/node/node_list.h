/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */

/*!
 * @file Declaration of class NodeList
 * @see NodeList
 * @author Christian Reiner
 */

#ifndef NODE_LIST_H
#define NODE_LIST_H

#include <QMap>
#include <kio/global.h>
#include <kio/udsentry.h>

using namespace KIO;
namespace KIO_CLIPBOARD
{
  class NodeWrapper;
  /*!
   * class NodeList
   * @brief Container class holding a list of node objects (clipboard items)
   * @author Christian Reiner
   */
  class NodeList
  {
    public:
      class iterator;
      class const_iterator : public QMap<QString, const NodeWrapper*>::const_iterator
      {
        friend class iterator;
        private:
          QMap<QString, const NodeWrapper*>::const_iterator m_iterator;
        public:
          inline                           const_iterator ( )                                                                   {                                                       };
          inline                           const_iterator ( const const_iterator& other )                                       {          m_iterator=other.m_iterator;                 };
//          inline                           const_iterator ( const iterator& other )                                             {          m_iterator=other.m_iterator;                 };
          inline                           const_iterator ( const QMap<QString, const NodeWrapper*>::const_iterator& iterator ) {          m_iterator=iterator;                         };
          inline const QString&            key            ( ) const                                                             { return   m_iterator.key();                            };
          inline const NodeWrapper* const& value          ( ) const                                                             { return   m_iterator.value();                          };
          inline bool                      operator!=     ( const const_iterator& other ) const                                 { return   m_iterator!=other.m_iterator;                };
          inline const NodeWrapper* const& operator*      ( ) const                                                             { return   m_iterator.value();                          };
          inline const_iterator            operator+      ( int j ) const                                                       { return   m_iterator+j;                                };
          inline const_iterator&           operator++     ( )                                                                   {        ++m_iterator;                   return *this;  };
          inline const_iterator            operator++     ( int )                                                               { return   m_iterator++;                                };
          inline const_iterator&           operator+=     ( int j )                                                             {          m_iterator+=j;                return *this;  };
          inline const_iterator            operator-      ( int j ) const                                                       { return   m_iterator-j;                                };
          inline const_iterator&           operator--     ( )                                                                   {        --m_iterator;                   return *this;  };
          inline const_iterator            operator--     ( int )                                                               { return   m_iterator--;                                };
          inline const_iterator&           operator-=     ( int j )                                                             {          m_iterator-=j;                return *this;  };
          inline const NodeWrapper* const* operator->     ( ) const                                                             { return  &m_iterator.value();                          };
          inline bool                      operator==     ( const const_iterator& other ) const                                 { return   m_iterator==other.m_iterator;                };
//          inline const_iterator&           operator=      ( const const_iterator& other )                                       {          m_iterator=other.m_iterator;  return *this;  };
      };
      class iterator
      {
        friend class const_iterator;
        private:
          QMap<QString, const NodeWrapper*>::iterator m_iterator;
        public:
          inline                     iterator   ( )                                                             {                                                       };
          inline                     iterator   ( const iterator& other )                                       {          m_iterator=other.m_iterator;                 };
//          inline                     iterator   ( const const_iterator& other )                                 {          m_iterator=other.m_iterator;                 };
          inline                     iterator   ( const QMap<QString, const NodeWrapper*>::iterator& iterator ) {          m_iterator=iterator;                         };
          inline const QString&      key        ( ) const                                                       { return   m_iterator.key();                            };
          inline const NodeWrapper*& value      ( ) const                                                       { return   m_iterator.value();                          };
          inline bool                operator!= ( const iterator& other ) const                                 { return   m_iterator!=other.m_iterator;                };
          inline bool                operator!= ( const const_iterator& other ) const                           { return   m_iterator!=other.m_iterator;                };
          inline const NodeWrapper*& operator*  ( ) const                                                       { return   m_iterator.value();                          };
          inline iterator            operator+  ( int j ) const                                                 { return   m_iterator+j;                                };
          inline iterator&           operator++ ( )                                                             {        ++m_iterator;                   return *this;  };
          inline iterator            operator++ ( int )                                                         { return   m_iterator++;                                };
          inline iterator&           operator+= ( int j )                                                       {          m_iterator+=j;                return *this;  };
          inline iterator            operator-  ( int j ) const                                                 { return   m_iterator-j;                                };
          inline iterator&           operator-- ( )                                                             {        --m_iterator;                   return *this;  };
          inline iterator            operator-- ( int )                                                         { return   m_iterator--;                                };
          inline iterator&           operator-= ( int j )                                                       {          m_iterator-=j;                return *this;  };
          inline const NodeWrapper** operator-> ( ) const                                                       { return  &m_iterator.value();                          };
          inline bool                operator== ( const iterator& other ) const                                 { return   m_iterator==other.m_iterator;                };
          inline bool                operator== ( const const_iterator& other ) const                           { return   m_iterator==other.m_iterator;                };
//          inline iterator&           operator=  ( const iterator& other )                                       {          m_iterator=other.m_iterator;  return *this;  };
      };
    public:
      QMap<QString, const NodeWrapper*> m_nodes;
      inline                                          NodeList    ( )                                                            {                                        };
      inline                                          NodeList    ( const NodeList& nodes )                                      { m_nodes=nodes.m_nodes;                 };
      inline                                          NodeList    ( const QMap<QString, const NodeWrapper*>& nodes )             { m_nodes=nodes;                         };
      inline                                          ~NodeList   ( )                                                            {                                        };
      inline NodeList::iterator                       begin       ( )                                                            { return m_nodes.begin();                };
      inline NodeList::const_iterator                 begin       ( ) const                                                      { return m_nodes.begin();                };
      inline void                                     clear       ( )                                                            {        m_nodes.clear();                };
      inline NodeList::const_iterator                 constBegin  ( ) const                                                      { return m_nodes.constBegin();           };
      inline NodeList::const_iterator                 constEnd    ( ) const                                                      { return m_nodes.constEnd();             };
      inline NodeList::const_iterator                 constFind   ( const QString& key ) const                                   { return m_nodes.constFind(key);         };
      inline bool                                     contains    ( const QString& key )                                         { return m_nodes.contains(key);          };
      inline int                                      count       ( const QString& key ) const                                   { return m_nodes.count(key);             };
      inline int                                      count       ( ) const                                                      { return m_nodes.count();                };
      inline bool                                     empty       ( ) const                                                      { return m_nodes.empty();                };
      inline NodeList::iterator                       end         ( )                                                            { return m_nodes.end();                  };
      inline NodeList::const_iterator                 end         ( ) const                                                      { return m_nodes.end();                  };
//      inline NodeList::iterator                       erase       ( NodeList::iterator pos )                                     { return m_nodes.erase(pos);             };
      inline NodeList::iterator                       find        ( const QString& key )                                         { return m_nodes.find(key);              };
      inline NodeList::const_iterator                 find        ( const QString& key ) const                                   { return m_nodes.find(key);              };
      inline NodeList::iterator                       insert      ( const QString& key, const NodeWrapper* value )               { return m_nodes.insert(key,value);      };
      inline NodeList::iterator                       insertMulti ( const QString& key, const NodeWrapper*& value )              { return m_nodes.insertMulti(key,value); };
      inline bool                                     isEmpty     ( )                                                            { return m_nodes.isEmpty();              };
      inline const QString                            key         ( const NodeWrapper*& value ) const                            { return m_nodes.key(value);             };
      inline const QString                            key         ( const NodeWrapper*& value, const QString& defaultKey ) const { return m_nodes.key(value,defaultKey);  };
      inline QList<QString>                           keys        ( ) const                                                      { return m_nodes.keys();                 };
      inline QList<QString>                           keys        ( const NodeWrapper*& value ) const                            { return m_nodes.keys(value);            };
      inline NodeList::iterator                       lowerBound  ( const QString& key )                                         { return m_nodes.lowerBound(key);        };
      inline NodeList::const_iterator                 lowerBound  ( const QString& key ) const                                   { return m_nodes.lowerBound(key);        };
      inline int                                      remove      ( const QString& key )                                         { return m_nodes.remove(key);            };
      inline int                                      size        ( )                                                            { return m_nodes.size();                 };
      inline const NodeWrapper*                       take        ( const QString& key )                                         { return m_nodes.take(key);              };
#ifndef QT_NO_STL
      inline std::map<QString, const NodeWrapper*>    toStdMap    ( ) const                                                      { return m_nodes.toStdMap(); };
#endif
      inline QList<QString>                           uniqueKeys  ( ) const                                                      { return m_nodes.uniqueKeys(); };
      inline NodeList&                                unite       ( const NodeList& other )                                      {        m_nodes.unite(other.m_nodes); return *this; };
      inline NodeList::iterator                       upperBound  ( const QString& key )                                         { return m_nodes.upperBound(key); };
      inline NodeList::const_iterator                 upperBound  ( const QString& key ) const                                   { return m_nodes.upperBound(key); };
      inline const NodeWrapper*                       value       ( const QString& key ) const                                   { return m_nodes.value(key); };
      inline const NodeWrapper*                       value       ( const QString& key, const NodeWrapper*& defaultValue ) const { return m_nodes.value(key,defaultValue); };
      inline QList <const NodeWrapper*>               values      ( )                                                            { return m_nodes.values(); };
      inline bool                                     operator!=  ( const NodeList& other ) const                                { return m_nodes!=other.m_nodes; };
      inline const NodeList&                          operator=   ( const NodeList& other )                                      {        m_nodes=other.m_nodes; return *this; };
      inline bool                                     operator==  ( const NodeList & other ) const                               { return m_nodes==other.m_nodes; };
      inline const NodeWrapper*&                      operator[]  ( const QString& key )                                         { return m_nodes[key]; };
      inline const NodeWrapper*                       operator[]  ( const QString& key ) const                                   { return m_nodes[key]; };
      inline const QMap<QString, const NodeWrapper*>& toMap       ( ) const                                                      { return m_nodes; };
      UDSEntryList toUDSEntryList ( ) const;
      QByteArray   toJSON         ( ) const;
      NodeList&    fromJSON       ( const QByteArray& json );
  }; // class NodeList
  
  inline QDataStream& operator<< ( QDataStream& out, const NodeList& list ) { return out << list.toJSON(); };
  inline QDataStream& operator>> ( QDataStream& in,        NodeList& list ) { QByteArray _json; in >> _json; list.fromJSON(_json); return in; };
  
} // namespace KIO_CLIPBOARD

#endif // NODE_LIST_H