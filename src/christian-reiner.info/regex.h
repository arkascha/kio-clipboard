/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef CRI_REGEX_H
#define CRI_REGEX_H

#include <QMap>
#include <QString>
#include <QDataStream>

/**
 * This class acts exactly as a plain QMap (QMap<QString,QRegExp>)
 * In additional it holds a collection of regular expressions that are required in day-to-day use to check syntactical patterns
 */

namespace CRI
{
  class regExPool
  {
    public:
      QMap<QString,QRegExp> p; // p as in "pool"
      inline regExPool() { p["protocol"] = QRegExp("[a-z]{3,9}");
                           p["user"]     = QRegExp("[^@]+@{1}");
                           p["domain"]   = QRegExp("([-a-z0-9]+.)+[a-z]{2,6}");
                           p["port"]     = QRegExp("[0-9]+");
                           p["path"]     = QRegExp("(/[^/]+)+/?");
                           p["query"]    = QRegExp("([&?][a-z0-9_]=[^&])*");
                           p["anchor"]   = QRegExp("\\w+");
                           p["uri"]      = QRegExp ( QString("^((%1):/{1,3})(%2)?(%3)(:%4)?(%5(%6)?(#%7)?)?$")
                                                                  .arg(p["protocol"].pattern())
                                                                  .arg(p["user"].pattern())
                                                                  .arg(p["domain"].pattern())
                                                                  .arg(p["port"].pattern())
                                                                  .arg(p["path"].pattern())
                                                                  .arg(p["query"].pattern())
                                                                  .arg(p["anchor"].pattern()) );
                         };
      inline QMap<QString,QRegExp>::iterator                    begin          ( )                                                             { return p.begin ( ); };
      inline QMap<QString,QRegExp>::const_iterator              begin          ( )                                                       const { return p.begin ( ); };
      inline void                                               clear          ( )                                                             {        p.clear ( ); };
      inline QMap<QString,QRegExp>::const_iterator              constBegin     ( )                                                       const { return p.constBegin ( ); };
      inline QMap<QString,QRegExp>::const_iterator              constEnd       ( )                                                       const { return p.constEnd ( ); };
      inline QMap<QString,QRegExp>::const_iterator              constFind      ( const QString & key )                                   const { return p.constFind ( key ); };
      inline bool                                               contains       ( const QString & key )                                   const { return p.contains ( key ); };
      inline int                                                count          ( const QString & key )                                   const { return p.count ( key ); };
      inline int                                                count          ( )                                                       const { return p.count ( ); };
      inline bool                                               empty          ( )                                                       const { return p.empty ( ); };
      inline QMap<QString,QRegExp>::iterator                    end            ( )                                                             { return p.end ( ); };
      inline QMap<QString,QRegExp>::const_iterator              end            ( )                                                       const { return p.end ( ); };
      inline QMap<QString,QRegExp>::iterator                    erase          ( QMap<QString,QRegExp>::iterator pos )                         { return p.erase ( pos ); };
      inline QMap<QString,QRegExp>::iterator                    find           ( const QString & key )                                         { return p.find ( key ); };
      inline QMap<QString,QRegExp>::const_iterator              find           ( const QString & key )                                   const { return p.find ( key ); };
      inline QMap<QString,QRegExp>::iterator                    insert         ( const QString & key, const QRegExp & value )                  { return p.insert ( key, value ); };
      inline QMap<QString,QRegExp>::iterator                    insertMulti    ( const QString & key, const QRegExp & value )                  { return p.insert ( key, value ); };
      inline bool                                               isEmpty        ( )                                                       const { return p.isEmpty ( ); };
      inline const QString                                      key            ( const QRegExp & value )                                 const { return p.key ( value ); };
      inline const QString                                      key            ( const QRegExp & value, const QString & defaultQString ) const { return p.key ( value, defaultQString ); };
      inline QList<QString>                                     keys           ( )                                                       const { return p.keys ( ); };
      inline QList<QString>                                     keys           ( const QRegExp & value )                                 const { return p.keys ( value ); };
      inline QMap<QString,QRegExp>::iterator                    lowerBound     ( const QString & key )                                         { return p.lowerBound ( key ); };
      inline QMap<QString,QRegExp>::const_iterator              lowerBound     ( const QString & key )                                   const { return p.lowerBound ( key ); };
      inline int                                                remove         ( const QString & key )                                         { return p.remove ( key ); };
      inline int                                                size           ( )                                                       const { return p.size ( ); };
      inline QRegExp                                            take           ( const QString & key )                                         { return p.take ( key ); };
      inline QList<QString>                                     uniqueKeys     ( )                                                       const { return p.uniqueKeys ( ); };
      inline QMap<QString, QRegExp> &                           unite          ( const QMap<QString, QRegExp> & other )                        { return p.unite ( other ); };
      inline QMap<QString,QRegExp>::iterator                    upperBound     ( const QString & key )                                         { return p.upperBound ( key ); };
      inline QMap<QString,QRegExp>::const_iterator              upperBound     ( const QString & key )                                   const { return p.upperBound ( key ); };
      inline const QRegExp                                      value          ( const QString & key )                                   const { return p.value ( key ); };
      inline const QRegExp                                      value          ( const QString & key, const QRegExp & defaultValue )     const { return p.value ( key, defaultValue ); };
      inline QList<QRegExp>                                     values         ( )                                                       const { return p.values ( ); };
      inline QList<QRegExp>                                     values         ( const QString & key )                                   const { return p.values ( key ); };
      inline bool                                               operator!=     ( const QMap<QString, QRegExp> & other )                  const { return p!=other; };
      inline QMap<QString, QRegExp> &                           operator=      ( const QMap<QString, QRegExp> & other )                        { return p; };
      inline bool                                               operator==     ( const QMap<QString, QRegExp> & other )                  const { return p==other; };
      inline QRegExp &                                          operator[]     ( const QString & key )                                         { return p[key]; };
      inline const QRegExp                                      operator[]     ( const QString & key )                                   const { return p[key]; };
#ifndef QT_NO_STL
      inline std::map<QString, QRegExp>                         toStdMap       ( )                                                       const { return p.toStdMap ( ); };
#endif
  }; // class regExPool

  inline QDataStream & operator<< ( QDataStream & out, const regExPool & map ) { out << map.p; }
  inline QDataStream & operator>> ( QDataStream & in,        regExPool & map ) { in  >> map.p; }
} // namespace

#endif