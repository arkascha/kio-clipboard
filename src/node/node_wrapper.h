/* This file is part of 'kio-clipboard'
 * Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
 *
 * $Author$
 * $Revision$
 * $Date$
 */
#ifndef NODE_WRAPPER_H
#define NODE_WRAPPER_H

#include <kio/global.h>
#include <kio/udsentry.h>
#include <kmimetype.h>
#include <kdatetime.h>
#include "utility/regex.h"

using namespace KIO;
namespace KIO_CLIPBOARD
{
  class ClipboardFrontend;

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
  class NodeWrapper
    : public QObject
  {
    public:
      enum Semantics { S_EMPTY, S_TEXT, S_CODE, S_FILE, S_DIR, S_LINK, S_URL };
    Q_OBJECT
//    Q_ENUMS ( Semantics )
    Q_PROPERTY ( int            m_index                  READ getIndex           WRITE setIndex           )
    Q_PROPERTY ( const QString& m_title                  READ getTitle           WRITE setTitle           )
    Q_PROPERTY ( int            m_size                   READ getSize            WRITE setSize            )
    Q_PROPERTY ( KDateTime      m_datetime               READ getDatetime        WRITE setDatetime        )
    Q_PROPERTY ( QString        m_mimetype               READ getMimetype        WRITE setMimetype        )
    Q_PROPERTY ( int            m_access                 READ getAccess          WRITE setAccess          )
//    Q_PROPERTY ( Semantics      m_semantics              READ getSemantics       WRITE setSemantics       )
// Qt-bug ?? we cannot use an enum here, even with the Q_ENUMS macro above:
// the qt meta object handler claims such property is no writable
// thus that property will _not_ be initialized during deserialization (so get a random value)
    Q_PROPERTY ( int            m_semantics              READ getSemantics       WRITE setSemantics       )
    Q_PROPERTY ( const QString& m_name                   READ getName            WRITE setName            )
    Q_PROPERTY ( QString        m_url                    READ getUrl             WRITE setUrl             )
    Q_PROPERTY ( QString        m_link                   READ getLink            WRITE setLink            )
    Q_PROPERTY ( const QString& m_path                   READ getPath            WRITE setPath            )
    Q_PROPERTY ( int            m_type                   READ getType            WRITE setType            )
    Q_PROPERTY ( const QString& m_icon                   READ getIcon            WRITE setIcon            )
    Q_PROPERTY ( QString        m_overlay                READ getOverlays        WRITE setOverlays        )
    Q_PROPERTY ( int            m_mappingNameCardinality READ getNameCardinality WRITE setNameCardinality )
    Q_PROPERTY ( int            m_mappingNameLength      READ getNameLength      WRITE setNameLength      )
    Q_PROPERTY ( const QString& m_mappingNamePattern     READ getNamePattern     WRITE setNamePattern     )
    private:
      int             m_index;
      QString         m_title;
      int             m_size;
      KDateTime       m_datetime;
      KMimeType::Ptr  m_mimetype;
      int             m_access;
      Semantics       m_semantics;
      QString         m_name;
      KUrl            m_url;
      KUrl            m_link;
      QString         m_path;
      int             m_type;
      QString         m_icon;
      QStringList     m_overlays;
    protected:
      int             m_mappingNameCardinality;
      int             m_mappingNameLength;
      QString         m_mappingNamePattern;
      const regExPool m_regEx;
    protected:
      // member serialization interface
      inline int              getIndex           ( ) { return m_index;                  };
      inline const QString&   getTitle           ( ) { return m_title;                  };
      inline int              getSize            ( ) { return m_size;                   };
      inline const KDateTime& getDatetime        ( ) { return m_datetime;               };
      inline QString          getMimetype        ( ) { return m_mimetype->name();       };
      inline int              getAccess          ( ) { return m_access;                 };
      inline Semantics        getSemantics       ( ) { return m_semantics;              };
      inline const QString&   getName            ( ) { return m_name;                   };
      inline QString          getUrl             ( ) { return m_url.prettyUrl();        };
      inline QString          getLink            ( ) { return m_link.prettyUrl();       };
      inline const QString&   getPath            ( ) { return m_path;                   };
      inline int              getType            ( ) { return m_type;                   };
      inline const QString&   getIcon            ( ) { return m_icon;                   };
      inline QString          getOverlays        ( ) { return m_overlays.join(",");     };
      inline int              getNameCardinality ( ) { return m_mappingNameCardinality; };
      inline int              getNameLength      ( ) { return m_mappingNameLength;      };
      inline const QString&   getNamePattern     ( ) { return m_mappingNamePattern;     };
      // member deserialization interface
      inline void setIndex           ( int              index           ) { m_index                  = index;                         };
      inline void setTitle           ( const QString&   title           ) { m_title                  = title;                         };
      inline void setSize            ( int              size            ) { m_size                   = size;                          };
      inline void setDatetime        ( const KDateTime& datetime        ) { m_datetime               = datetime;                      };
      inline void setMimetype        ( const QString    mimetype        ) { m_mimetype               = KMimeType::mimeType(mimetype); };
      inline void setAccess          ( int              access          ) { m_access                 = access;                        };
//      inline void setSemantics       ( Semantics        semantics       ) { m_semantics              = semantics;                     };
      inline void setSemantics       ( int              semantics       ) { m_semantics              = Semantics(semantics);          };
      inline void setName            ( const QString&   name            ) { m_name                   = name;                          };
      inline void setUrl             ( const QString&   url             ) { m_url                    = KUrl(url);                     };
      inline void setLink            ( const QString&   link            ) { m_link                   = KUrl(link);                    };
      inline void setPath            ( const QString&   path            ) { m_path                   = path;                          };
      inline void setType            ( int              type            ) { m_type                   = type;                          };
      inline void setIcon            ( const QString&   icon            ) { m_icon                   = icon;                          };
      inline void setOverlays        ( const QString&   overlays        ) { m_overlays               = overlays.split(",");           };
      inline void setNameCardinality ( int              nameCardinality ) { m_mappingNameCardinality = nameCardinality;               };
      inline void setNameLength      ( int              nameLength      ) { m_mappingNameLength      = nameLength;                    };
      inline void setNamePattern     ( const QString&   namePattern     ) { m_mappingNamePattern     = namePattern;                   };
    public:
      NodeWrapper ( ClipboardFrontend* const clipboard, int index, const QString& payload, QObject* parent=0 );
      NodeWrapper ( const QByteArray& json, QObject* parent=0 );
      NodeWrapper ( const NodeWrapper& node, QObject* parent=0 );
      NodeWrapper ( QObject* parent=0 );
      ~NodeWrapper ( );
      inline int                   index     ( ) const { return m_index;     };
      inline const QString&        title     ( ) const { return m_title;     };
      inline int                   size      ( ) const { return m_size;      };
      inline const KDateTime&      datetime  ( ) const { return m_datetime;  };
      inline const KMimeType::Ptr& mimetype  ( ) const { return m_mimetype;  };
      inline int                   access    ( ) const { return m_access;    };
      inline const Semantics&      semantics ( ) const { return m_semantics; };
      inline const QString&        name      ( ) const { return m_name;      };
      inline const KUrl&           url       ( ) const { return m_url;       };
      inline const KUrl&           link      ( ) const { return m_link;      };
      inline const QString&        path      ( ) const { return m_path;      };
      inline int                   type      ( ) const { return m_type;      };
      inline const QString&        icon      ( ) const { return m_icon;      };
      inline const QStringList&    overlays  ( ) const { return m_overlays;  };
      QString  prettyIndex     ( ) const;
      QString  prettyMimetype  ( ) const;
      QString  prettySemantics ( ) const;
      QString  prettyName      ( ) const;
      QString  prettyUrl       ( ) const;
      QString  prettyDatetime  ( ) const;
             QString payload2title ( const QString& payload );
      static QString payload2name  ( const QString& payload );
      UDSEntry     toUDSEntry ( ) const;
      QByteArray   toJSON ( ) const;
      NodeWrapper& fromJSON ( const QByteArray& json );
  }; // class NodeWrapper

  inline QDataStream& operator<< ( QDataStream& out, const NodeWrapper*& node ) { return out << node->toJSON(); };
  inline QDataStream& operator>> ( QDataStream& in,        NodeWrapper*& node ) { QByteArray _json; in >> _json; node->fromJSON ( _json ); return in; };

} // namespace KIO_CLIPBOARD

Q_DECLARE_METATYPE ( KIO_CLIPBOARD::NodeWrapper )

#endif // NODE_WRAPPER_H