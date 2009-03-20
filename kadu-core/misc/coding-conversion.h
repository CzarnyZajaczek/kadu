#ifndef CODING_CONVERSION_H
#define CODING_CONVERSION_H

#include "base.h"

KADUAPI QString cp2unicode(const QByteArray &);
KADUAPI QByteArray unicode2cp(const QString &);
KADUAPI QString latin2unicode(const QByteArray &);
KADUAPI QByteArray unicode2latin(const QString &);
KADUAPI QString unicode2std(const QString &);

// TODO: why not use normal QUrl::encode ?
//zamienia polskie znaki na format latin2 "url" (czyli do postaci %XY)
KADUAPI QString unicode2latinUrl(const QString &buf);
//zamienia kodowanie polskich znak�w przekonwertowane z utf-8 przy pomocy QUrl::encode na kodowanie latin-2
KADUAPI QString unicodeUrl2latinUrl(const QString &buf);

#endif
