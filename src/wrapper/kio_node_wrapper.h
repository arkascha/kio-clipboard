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
#include <kmimetype.h>
#include "christian-reiner.info/regex.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{
  class KIOClipboardWrapper;

  typedef enum { S_EMPTY, S_TEXT, S_CODE, S_FILE, S_DIR, S_LINK, S_URL } Semantics;

  /**
   * This class describes all aspects of a 'node', this is an entry in a clipboard.
   * The protocol implementations use objects of this class to keep track of entries on clipboards:
   * By defining such an own description of an entry it is easier to handle an entry from sifferent places out since we can offer convenient methods.
   *
   * The class has somewhat passive character: all data is treated more or less constant
   * - the private members hold basic information as detected, requested or decided upon
   * - all private members are published via direct access methods (read only)
   * - in addition a number of convenience constructions are offered as methods as well
   *   these are generated based only on the constant settings stored in the members mentioned above
   */
  class KIONodeWrapper
  {
    private:
      int            m_index;
      QString        m_payload;
      KMimeType::Ptr m_mimetype;
      Semantics      m_semantics;
      QString        m_name;
      KUrl           m_url;
      KUrl           m_link;
      QString        m_path;
      int            m_type;
      QString        m_icon;
      QStringList    m_overlays;
    protected:
      KIOClipboardWrapper* const m_clipboard;
      const CRI::regExPool       m_regEx;
    public:
      KIONodeWrapper ( KIOClipboardWrapper* const clipboard, int index, const QString& payload );
      ~KIONodeWrapper();
      inline int                   index     ( ) const { return m_index; };
      inline const QString&        payload   ( ) const { return m_payload; };
      inline const KMimeType::Ptr& mimetype  ( ) const { return m_mimetype; };
      inline const Semantics&      semantics ( ) const { return m_semantics; };
      inline const QString&        name      ( ) const { return m_name; };
      inline const KUrl&           url       ( ) const { return m_url; };
      inline const KUrl&           link      ( ) const { return m_link; };
      inline const QString&        path      ( ) const { return m_path; };
      inline int                   type      ( ) const { return m_type; };
      inline const QString&        icon      ( ) const { return m_icon; };
      inline const QStringList&    overlays  ( ) const { return m_overlays; };
      inline int                   size      ( ) const { return m_payload.size(); };
      QString  prettyIndex     ( ) const;
      QString  prettyPayload   ( ) const;
      QString  prettyMimetype  ( ) const;
      QString  prettySemantics ( ) const;
      QString  prettyName      ( ) const;
      QString  prettyUrl       ( ) const;
      UDSEntry toUDSEntry      ( ) const;
  }; // class KIONodeWrapper

} // namespace KIO_CLIPBOARD

#endif // KIO_NODE_WRAPPER_H