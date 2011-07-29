/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef KIO_NODE_WRAPPER_H
#define KIO_NODE_WRAPPER_H

#include <kio/global.h>
#include "kio/udsentry.h"
#include "christian-reiner.info/regex.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{

  enum Semantics { T_TEXT,T_CODE,T_FILE,T_LINK,T_DIR,T_URL };

  class KIONodeWrapper
  {
    private:
      int       m_index;
      QString   m_payload;
      QString   m_mimetype;
      Semantics m_semantics;
      QString   m_name;
      KUrl      m_url;
      QString   m_path;
      int       m_type;
    protected:
      int                  m_mappingNameCardinality;
      int                  m_mappingNameLength;
      QString              m_mappingNamePattern;
      const CRI::regExPool m_regEx;
    public:
      KIONodeWrapper ( int index, const QString& payload );
      ~KIONodeWrapper();
      inline int              index     ( ) const { return m_index; };
      inline const QString&   payload   ( ) const { return m_payload; };
      inline const QString&   mimetype  ( ) const { return m_mimetype; };
      inline const Semantics& semantics ( ) const { return m_semantics; };
      inline const QString&   name      ( ) const { return m_name; };
      inline const KUrl&      url       ( ) const { return m_url; };
      inline const QString&   path      ( ) const { return m_path; };
      inline int              type      ( ) const { return m_type; };
      QString  prettyIndex     ( ) const;
      QString  prettyPayload   ( ) const;
      QString  prettySemantics ( ) const;
      QString  prettyName      ( ) const;
      QString  prettyUrl       ( ) const;
      UDSEntry toUDSEntry      ( ) const;
  }; // class KIONodeWrapper

} // namespace KIO_CLIPBOARD

#endif // KIO_NODE_WRAPPER_H