#ifndef KIO_CLIPBOARD_WRAPPER_H
#define KIO_CLIPBOARD_WRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <kio/global.h>
#include <kio/jobclasses.h>
#include <kio/udsentry.h>
#include "christian-reiner.info/regex.h"
using namespace KIO;
namespace KIO_CLIPBOARD
{
  const QStringList tokenizeUrl ( const KUrl& url);

  enum ClipboardType  { KLIPPER };
  enum Semantic { T_STRING,T_CODE,T_FILE,T_LINK,T_DIR,T_URL };
  typedef struct
  {
    int      numericIndex;
    QString  clipboardPayload;
    QString  technicalName;
    Semantic internalType;
    QString  urlTarget;
    QString  fileTarget;
    QString  linkTarget;
    int      fileType;
    QString  displayName;
    QString  printableType;
    QString  mimeType;
  } EntryTokens;

  class KIOClipboardWrapper
  {
    private:
      const ClipboardType  m_type;
      const KUrl           m_base;
    protected:
      int            m_mappingNameCardinality;
      int            m_mappingNameLength;
      QString        m_mappingNamePattern;
      const CRI::regExPool m_regEx;
    public:
      UDSEntryList m_pool;
    protected:
      QString     prettyTruncateName  ( const QString& name );
      QString     mapDetails2Display  ( const EntryTokens& details );
      EntryTokens mapPayload2Details  ( int index, const QString& payload );
      const UDSEntry& findEntryByUrl ( const KUrl& url );
      void refreshPool ();
    public:
      static const QMap<ClipboardType,QString> detectClipboards ( );
      KIOClipboardWrapper ( const ClipboardType type, const KUrl& base );
      ~KIOClipboardWrapper ();
      inline const ClipboardType type () { return this->m_type; };
      inline const KUrl          base () { return this->m_base; };
      UDSEntry createPoolEntry ( int index, QString payload );
      virtual QString     getClipboardEntry   () = 0;
      virtual QStringList getClipboardEntries () = 0;
      virtual void        pushEntry ( QString entry ) = 0;
      virtual void        delEntry  ( const KUrl& url ) = 0;
  }; // class KIOClipboardWrapper

}; // namespace KIO_CLIPBOARD

#endif // KIO_CLIPBOARD_WRAPPER_H