/*

    $Id$

    Copyright (C) 2003 Florian Schmitt, Science and Computing AG
                       f.schmitt@science-computing.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef KONTODATENINFOZEIT_H
#define  KONTODATENINFOZEIT_H

#include <QObject>
#include <QTextStream>
#include "kontodateninfo.h"

/**
  * Liest die Kontodaten ueber die Zeittools ein.
  */
class KontoDatenInfoZeit: public KontoDatenInfo
{
  Q_OBJECT
signals:
  void kontoListeGeladen();
  public:
    KontoDatenInfoZeit();
    KontoDatenInfoZeit(QString sourcefile);
    virtual bool readInto(AbteilungsListe * abtlist);
    bool readInto2(AbteilungsListe * abtList, bool re);
    void setKommando(const QString& command);
    virtual bool checkIn(AbteilungsListe* abtlist);

    bool readDefaultComments(AbteilungsListe * abtList);
  private:
    QString m_DatenFileName;
    QString m_Kommando;
    bool readZeitFile(QTextStream &ts, AbteilungsListe * abtlist);
    bool readFile(QTextStream& ts, AbteilungsListe *abtList, bool comments_only);
    bool readCommentsFromZeitFile(QTextStream& ts, AbteilungsListe * abtList);
};

#endif
