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

#include "kontoliste.h"
#include "abteilungsliste.h"
#include "kontodateninfo.h"

#include <iostream>
#include "qtextstream.h"
#include "qfile.h"
#include <iomanip>


#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <utility>
#include "globals.h"
#include "qdir.h"
#include "qstring.h"

/** Erzeugt eine Abteilungsliste fuer das angegebene Datum.
 */
AbteilungsListe::AbteilungsListe(const QDate _datum, KontoDatenInfo* ki): std::map<QString,KontoListe>()
{
  kontoDatenInfoSuccess = false;
  aktivAbteilung="";
  aktivKonto="";
  aktivUnterkonto="";
  aktivEintrag=0;
  zeitDifferenz=0;
  datum=_datum;
  kontoDatenInfo=ki;
  reload();
}


/** Sucht nach dem in abteilung,konto,unterkonto angegebenen Konto, und liefert
  * einen iterator darauf in posUk zurueck, der fuer die ebenfalls
  * zurueckgegebene unterkontoliste gueltig ist.
  * Der return-Wert ist genau dann true, falls das Konto gefunden wurde.
  */
bool AbteilungsListe::findEintrag(EintragsListe::iterator& itEt, EintragsListe* &eintragsliste, const QString& abteilung, const QString& konto, const QString& unterkonto, int idx)
{

  UnterKontoListe *unterkontoliste;
  UnterKontoListe::iterator itUk;

  eintragsliste=NULL;

  if (!findUnterKonto(itUk,unterkontoliste,abteilung,konto,unterkonto)) return false;

  if (itUk==unterkontoliste->end())
    return false;

  eintragsliste=&(itUk->second);

  itEt=eintragsliste->find(idx);
  if (itEt==eintragsliste->end())
    return false;
  return true;
}


/**
 * Sucht das uebergebene Konto, und liefert die Liste in der es sich befindet,
 * in kontoliste und den Iterator darauf in itKo zurueck.
 */
bool AbteilungsListe::findKonto(KontoListe::iterator& itKo, KontoListe* &kontoliste, const QString& abteilung, const QString& konto)
{
  iterator posAbt=find(abteilung);

  if (posAbt==end())
    return false;

  kontoliste=&(posAbt->second);

  itKo=kontoliste->find(konto);

  if (itKo==kontoliste->end())
    return false;
  return true;
}

/**
 * Sucht die Abteilung, in der sich das Konto konto befindet.
 * Rueckgabewert ist der Name der Abteilung.
 */
QString AbteilungsListe::findAbteilungOfKonto(const QString& konto)
{
  for (iterator posAbt=begin(); posAbt!=end(); ++posAbt) {

    KontoListe* kontoliste=&(posAbt->second);

    KontoListe::iterator itKo=kontoliste->find(konto);

    if (itKo!=kontoliste->end())
      return posAbt->first;
  }
  return "";
}


/**
 * Sucht das uebergebene UnterKonto, und liefert die Liste in der es sich befindet,
 * in unterkontoliste und den Iterator darauf in itUk zurueck.
 */
bool AbteilungsListe::findUnterKonto(UnterKontoListe::iterator& itUk, UnterKontoListe* &unterkontoliste, const QString& abteilung, const QString& konto, const QString& unterkonto)
{
  KontoListe *kontoliste;
  KontoListe::iterator itKo;

  if (!findKonto(itKo,kontoliste,abteilung,konto)) return false;

  unterkontoliste=&(itKo->second);

  itUk=unterkontoliste->find(unterkonto);
  if (itUk==unterkontoliste->end())
    return false;
  return true;
}

/** Fuegt eine Abteilung hinzu, falls nicht vorhanden. Zurueckgegeben wird die zugehoerige Kontoliste */

KontoListe* AbteilungsListe::insertAbteilung(const QString& abteilung)
{
  QString abt=abteilung.simplifyWhiteSpace();

  if (abteilung=="") return NULL;

  iterator posAbt=find(abt);

  KontoListe *kontoliste;

  if (posAbt==end())
  {
    kontoliste=&((insert (posAbt,AbteilungsListe::value_type(abt,KontoListe())))->second);
  }
  else
    kontoliste=&(posAbt->second);

  return kontoliste;

}

/** Fuegt ein Konto hinzu, falls nicht vorhanden. Zurueckgegeben wird die zugehoerige UnterKontoliste */

UnterKontoListe* AbteilungsListe::insertKonto(const QString& abteilung,const QString& konto)
{
  KontoListe *kontoliste=insertAbteilung(abteilung);
  KontoListe::iterator posKont;
    
  posKont=kontoliste->find(konto);

  UnterKontoListe *unterkontoliste;

  if (posKont==kontoliste->end())
  {
    unterkontoliste = &((kontoliste->insert(posKont,KontoListe::value_type(konto,UnterKontoListe())))->second);

  } else unterkontoliste=&(posKont->second);
  
  return unterkontoliste;
}

/** Fuegt ein UnterKonto hinzu, falls nicht vorhanden. Zurueckgegeben wird die zugehoerige EintragsListe */

EintragsListe* AbteilungsListe::insertUnterKonto(const QString& abteilung,const QString& konto,const QString& unterkonto)
{
  UnterKontoListe *unterkontoliste=insertKonto(abteilung,konto);
  UnterKontoListe::iterator posUk;

  posUk=unterkontoliste->find(unterkonto);

  EintragsListe *eintragsliste;

  if (posUk==unterkontoliste->end())
  {
    eintragsliste = &((unterkontoliste->insert(posUk,UnterKontoListe::value_type(unterkonto,EintragsListe())))->second);
  } else eintragsliste=&(posUk->second);
  return eintragsliste;
}

/** Fuegt den angegebenen Eintrag ein.
 *  Es wird der Index des neuen Eintrags zurueckgegeben.
 *  Mit idx!=-1 kann der Index des Eintrags vordefiniert werden (Doppelte Eintraege werden verworfen).
 */
int AbteilungsListe::insertEintrag(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, UnterKontoEintrag eintrag)
{

  EintragsListe *eintragsliste=insertUnterKonto(abteilung,konto,unterkonto);
  if (idx==-1) {
    idx=0;
    for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
      if (idx==etPos->first) idx++; else break; //Ersten freien Integerwert nehmen.
    }
  }
  eintragsliste->insert(EintragsListe::value_type(idx,eintrag));

  return idx;
}


void AbteilungsListe::setBeschreibung(const QString& abteilung, const QString& konto, const QString& unterkonto, const QString& beschreibung)
{
  UnterKontoListe *unterkontoliste;
  UnterKontoListe::iterator itUk;
  if (!findUnterKonto(itUk,unterkontoliste,abteilung,konto,unterkonto)) return;
  itUk->second.setBeschreibung(beschreibung);
}


QString AbteilungsListe::getBeschreibung(const QString& abteilung, const QString& konto, const QString& unterkonto)
{
  UnterKontoListe *unterkontoliste;
  UnterKontoListe::iterator itUk;

  if (!findUnterKonto(itUk,unterkontoliste,abteilung,konto,unterkonto)) return "";

  return itUk->second.beschreibung();
}


void AbteilungsListe::deleteEintrag(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx)
{
  EintragsListe::iterator eti;
  EintragsListe* etl;

  if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx))
    {
      etl->erase(eti);
      if (etl->size()==1) {
        if ((etl->begin()->second.flags)&UK_PERSOENLICH)
          setUnterKontoFlags(abteilung,konto,unterkonto,UK_PERSOENLICH,FLAG_MODE_OR);
        else
          setUnterKontoFlags(abteilung,konto,unterkonto,UK_PERSOENLICH,FLAG_MODE_NAND);
      }
    }
}

/** Liefert den in abteilung,konto,unterkonto ,idx angegebenen Eintrag in uk zurueck.
 *  Der return-Wert ist genau dann true, falls das Konto gefunden wurde.
 */
bool AbteilungsListe::getEintrag(UnterKontoEintrag& uk, const QString& abteilung, const QString& konto, const QString& unterkonto, int idx)
{
  EintragsListe::iterator eti;
  EintragsListe* etl;

  if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      uk=eti->second;
      return true;
  } else return false;
}


/** Ersetzt das in abteilung,konto,unterkonto angegebene Unterkonto durch uk.
 * Das Konto muss dazu bereits vorhanden sein. Liefert true, falls erfolgreich.
 */
bool AbteilungsListe::setEintrag(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, const UnterKontoEintrag& uk, bool regular)
{
  EintragsListe::iterator eti;
  EintragsListe* etl;

  if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      if (!regular) zeitDifferenz+=uk.sekunden-eti->second.sekunden;
      eti->second=uk;
      return true;
  } else return false;
}


/** Setzt den Kommentar fuer das in abteilung,konto,unterkonto angegebene Unterkonto.
 * Das Konto muss dazu bereits vorhanden sein. Liefert true, falls erfolgreich.
 */
bool AbteilungsListe::setKommentar(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, const QString& kommentar)
{
    EintragsListe::iterator eti;
    EintragsListe* etl;

    if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      eti->second.kommentar=kommentar;
      return true;
    } else return false;
}


/** Setzt die gebuchten Sekunden fuer das in abteilung,konto,unterkonto angegebene Unterkonto.
 * Das Konto muss dazu bereits vorhanden sein. Liefert true, falls erfolgreich.
 */
bool AbteilungsListe::setSekunden(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, int sekunden, bool regular)
{
    EintragsListe::iterator eti;
    EintragsListe* etl;

    if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      if (!regular) zeitDifferenz+=sekunden-eti->second.sekunden;
      eti->second.sekunden=sekunden;

      return true;
    } else return false;
}


/** Setzt die gebuchten abzurechnenden Sekunden fuer das in abteilung,konto,unterkonto angegebene Unterkonto.
 * Das Konto muss dazu bereits vorhanden sein. Liefert true, falls erfolgreich.
 */
bool AbteilungsListe::setSekundenAbzur(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, int sekundenabzur)
{
    EintragsListe::iterator eti;
    EintragsListe* etl;

    if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      eti->second.sekundenAbzur=sekundenabzur;

      return true;
    } else return false;
}


/** Veraendert die Zeiten fuer das angegebene Konto um delta Sekunden
 */
void AbteilungsListe::changeZeit(const QString& Abteilung,const QString& Konto,const QString& Unterkonto,int Eintrag, int delta, bool regular )
{
    EintragsListe::iterator eti;
    EintragsListe* etl;

    if (findEintrag(eti,etl, Abteilung, Konto, Unterkonto, Eintrag)) {
      if (!regular) {
        if (eti->second.sekunden<-delta)
          zeitDifferenz+=-eti->second.sekunden;
        else zeitDifferenz+=delta;
      }
      eti->second.sekunden+=delta;
      eti->second.sekundenAbzur+=delta;
      if (eti->second.sekunden<0) eti->second.sekunden=0;
      if (eti->second.sekundenAbzur<0) eti->second.sekundenAbzur=0;
    }
}


/** Erhoeht die Zeiten fuer das aktive Konto um 60s (die Abzurechnende nur, falls abzur=true);
 */
void AbteilungsListe::minuteVergangen(bool abzur)
{

  EintragsListe::iterator eti;
  EintragsListe* etl;

  if (findEintrag(eti,etl, aktivAbteilung, aktivKonto, aktivUnterkonto,aktivEintrag)) {

    eti->second.sekunden+=60;
    if (abzur) eti->second.sekundenAbzur+=60;
  }
}

int applyFlagMode(int oldflags, int mask, int mode)
{
  int newflags=0;
  switch (mode) {
    case FLAG_MODE_OVERWRITE: newflags=mask; break;
    case FLAG_MODE_OR: newflags=oldflags|mask; break;
    case FLAG_MODE_NAND: newflags=oldflags&(~mask); break;
    case FLAG_MODE_XOR: newflags=oldflags^mask; break;
    default: std::cout<<"Unknown Mode in applyFlagMode"<<std::endl;
  }
  return newflags;
}

/**
 * Setzt die Flags fuer den angebenen Eintrag.
 */
bool AbteilungsListe::setEintragFlags(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, int flags, int mode)
{

    EintragsListe::iterator eti;
    EintragsListe* etl;

    if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      eti->second.flags=applyFlagMode(eti->second.flags,flags,mode);
      return true;
    } else return false;
}

/**
 * Setzt die Flags fuer das angebene Unterkonto.
 */
bool AbteilungsListe::setUnterKontoFlags(const QString& abteilung, const QString& konto, const QString& unterkonto, int flags, int mode)
{

    UnterKontoListe::iterator uki;
    UnterKontoListe* ukl;

    if (findUnterKonto(uki,ukl, abteilung, konto, unterkonto)) {
      uki->second.setFlags(applyFlagMode(uki->second.getFlags(),flags,mode));

      return true;
    } else return false;
}

/**
 * Setzt die Flags fuer das angebene Konto.
 */
bool AbteilungsListe::setKontoFlags(const QString& abteilung, const QString& konto, int flags, int mode)
{

    KontoListe::iterator koi;
    KontoListe* kol;

    if (findKonto(koi,kol, abteilung, konto)) {
      koi->second.setFlags(applyFlagMode(koi->second.getFlags(),flags,mode));

      return true;
    } else return false;
}

/**
 * Setzt die Flags fuer das angebene Konto.
 */
bool AbteilungsListe::setAbteilungFlags(const QString& abteilung, int flags, int mode)
{

    iterator abti=find(abteilung);

    if (abti!=end()) {
      abti->second.setFlags(applyFlagMode(abti->second.getFlags(),flags,mode));

      return true;
    } else return false;
}


/**
 * Setzt die Flags fuer alle Unterkonten im angegeben Konto
 */
void AbteilungsListe::setAllEintragFlagsInKonto(const QString& abteilung, const QString& konto, int flags)
{
  KontoListe::iterator itKo;
  KontoListe* kontoliste;

  if (!findKonto(itKo,kontoliste,abteilung,konto)) return;

  UnterKontoListe* unterkontoliste=&(itKo->second);
  for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin(); ukontPos!=unterkontoliste->end(); ++ukontPos) {
    EintragsListe* eintragsliste=&(ukontPos->second);
    for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
      etPos->second.flags=flags;
    }
  }
}


/**
 * Setzt die Flags fuer alle Eintraege im angegeben UnterKonto
 */
void AbteilungsListe::setAllEintragFlagsInUnterKonto(const QString& abteilung, const QString& konto, const QString& unterkonto, int flags)
{
  UnterKontoListe::iterator itUk;
  UnterKontoListe* unterkontoliste;

  if (!findUnterKonto(itUk,unterkontoliste,abteilung,konto,unterkonto)) return;

  EintragsListe* eintragsliste=&(itUk->second);
  for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
      etPos->second.flags=flags;
  }
}

void AbteilungsListe::moveEintragPersoenlich(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx, bool persoenlich)
{
  EintragsListe::iterator itEt;
  EintragsListe* eintragsliste;

  if (!findEintrag(itEt,eintragsliste,abteilung,konto,unterkonto,idx)) return;
  int mode;
  if (persoenlich) mode=FLAG_MODE_OR; else mode=FLAG_MODE_NAND;
  itEt->second.flags=applyFlagMode(itEt->second.flags,UK_PERSOENLICH,mode);

  if (persoenlich)
    eintragsliste->setFlags(applyFlagMode(eintragsliste->getFlags(),UK_PERSOENLICH,mode));
  else {
    bool hasPersEintrag=false;
    for (EintragsListe::iterator itEt=eintragsliste->begin(); itEt!=eintragsliste->end(); ++itEt) {
      if (itEt->second.flags&UK_PERSOENLICH) {
        hasPersEintrag=true;
        break;
      }
    }
    if (!hasPersEintrag)
      eintragsliste->setFlags(applyFlagMode(eintragsliste->getFlags(),UK_PERSOENLICH,mode));
  }
}

void AbteilungsListe::moveUnterKontoPersoenlich(const QString& abteilung, const QString& konto, const QString& unterkonto, bool persoenlich)
{
  UnterKontoListe::iterator itUko;
  UnterKontoListe* unterkontoliste;

  if (!findUnterKonto(itUko,unterkontoliste,abteilung,konto,unterkonto)) return;

  int mode;
  if (persoenlich) mode=FLAG_MODE_OR; else mode=FLAG_MODE_NAND;

  EintragsListe* eintragsliste=&(itUko->second);
  eintragsliste->setFlags(applyFlagMode(eintragsliste->getFlags(),UK_PERSOENLICH,mode));
  for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
    etPos->second.flags=applyFlagMode(etPos->second.flags,UK_PERSOENLICH,mode);
  }
}

void AbteilungsListe::moveKontoPersoenlich(const QString& abteilung, const QString& konto, bool persoenlich)
{
  KontoListe::iterator itKo;
  KontoListe* kontoliste;

  if (!findKonto(itKo,kontoliste,abteilung,konto)) return;

  int mode;
  if (persoenlich) mode=FLAG_MODE_OR; else mode=FLAG_MODE_NAND;

  UnterKontoListe* unterkontoliste=&(itKo->second);
  unterkontoliste->setFlags(applyFlagMode(unterkontoliste->getFlags(),UK_PERSOENLICH,mode));
  for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin(); ukontPos!=unterkontoliste->end(); ++ukontPos) {
    EintragsListe* eintragsliste=&(ukontPos->second);
    eintragsliste->setFlags(applyFlagMode(eintragsliste->getFlags(),UK_PERSOENLICH,mode));
    for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
      etPos->second.flags=applyFlagMode(etPos->second.flags,UK_PERSOENLICH,mode);
    }
  }
}


/**
 * Liefert die Flags fuer den angebenen Eintrag.
 */
int AbteilungsListe::getEintragFlags(const QString& abteilung, const QString& konto, const QString& unterkonto,int idx)
{
    EintragsListe::iterator eti;
    EintragsListe* etl;

    if (findEintrag(eti,etl, abteilung, konto, unterkonto,idx)) {
      return eti->second.flags;
    } else return 0;

}

/**
 * Liefert die Flags fuer das angebene Unterkonto.
 */
int AbteilungsListe::getUnterKontoFlags(const QString& abteilung, const QString& konto, const QString& unterkonto)
{

    UnterKontoListe::iterator uki;
    UnterKontoListe* ukl;

    if (findUnterKonto(uki,ukl, abteilung, konto, unterkonto)) {
      return uki->second.getFlags();

      return true;
    } else return 0;
}

/**
 * Gibt die Zeiten fuer das angebene Unterkonto zurueck.
 */
void AbteilungsListe::getUnterKontoZeiten(const QString& abteilung, const QString& konto,
                                         const QString& unterkonto, int& sek, int& sekabzur)
{

    UnterKontoListe::iterator uki;
    UnterKontoListe* ukl;

    if (findUnterKonto(uki,ukl, abteilung, konto, unterkonto)) {
       sek = 0;
       sekabzur = 0;
       EintragsListe* eintragsliste=&(uki->second);
       for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
          sek += etPos->second.sekunden;
          sekabzur += etPos->second.sekundenAbzur;
       }
    }
    else {
      sek = 0;
      sekabzur = 0;
    }
}

/**
 * Setzt die Flags fuer das angebene Konto.
 */
int AbteilungsListe::getKontoFlags(const QString& abteilung, const QString& konto)
{

    KontoListe::iterator koi;
    KontoListe* kol;

    if (findKonto(koi,kol, abteilung, konto)) {
      return koi->second.getFlags();

      return true;
    }
    else
      return 0;
}

/**
 * Setzt die Flags fuer das angebene Konto.
 */
int AbteilungsListe::getAbteilungFlags(const QString& abteilung)
{

    iterator abti=find(abteilung);

    if (abti!=end()) {
      return abti->second.getFlags();

      return true;
    } else return 0;
}


/** Liefert das momentan aktive Konto in die Referenzen abteilung,konto,unterkonto
 *
 */
void AbteilungsListe::getAktiv(QString& abteilung, QString& konto, QString& unterkonto,int& idx)
{
  abteilung=aktivAbteilung;
  konto=aktivKonto;
  unterkonto=aktivUnterkonto;
  idx=aktivEintrag;
}


/** Setzt den uebergebenen Eintrag mit index idx, Abteilung abteilung,... auf aktiv */
void AbteilungsListe::setAsAktiv(const QString& abteilung, const QString& konto, const QString& unterkonto,int idx)
{
    aktivAbteilung=abteilung;
    aktivKonto=konto;
    aktivUnterkonto=unterkonto;
    
    UnterKontoListe::iterator uki;
    UnterKontoListe* ukl;

    aktivEintrag=0;

    if (findUnterKonto(uki,ukl, abteilung, konto, unterkonto)) {      
      if (uki->second.find(idx)!=uki->second.end())
        aktivEintrag = idx;
      else 
        aktivEintrag = (uki->second.begin())->first;
    }
}


/** Liefert true, falls der uebergebene Eintrag mit index idx, Abteilung abteilung,... das aktive ist */
bool AbteilungsListe::isAktiv(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx)
{
  return ((aktivAbteilung==abteilung)&&(aktivKonto==konto)&&(aktivUnterkonto==unterkonto)&&(aktivEintrag==idx));
}

bool AbteilungsListe::isPersoenlich(const QString& abteilung, const QString& konto, const QString& unterkonto, int idx)
{
  return getEintragFlags(abteilung,konto,unterkonto,idx)&UK_PERSOENLICH;
}


/** Addiert die Zeiten aller Eintraege und liefert sie in sek bzw sekabzur zurueck.*/
void AbteilungsListe::getGesamtZeit(int& sek, int& sekabzur)
{

  sek=0;
  sekabzur=0;

  AbteilungsListe::iterator abtPos;

  for (abtPos=begin(); abtPos!=end(); ++abtPos) {
    KontoListe* kontoliste=&(abtPos->second);
    for (KontoListe::iterator kontPos=kontoliste->begin(); kontPos!=kontoliste->end(); ++kontPos) {
      UnterKontoListe* unterkontoliste=&(kontPos->second);
      for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin(); ukontPos!=unterkontoliste->end(); ++ukontPos) {
        EintragsListe* eintragsliste=&(ukontPos->second);
        for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
          sek+=etPos->second.sekunden;
          sekabzur+=etPos->second.sekundenAbzur;
        }
      }
    }
  }
}


/** Liefert das Datum der AbteilungsListe */
const QDate& AbteilungsListe::getDatum()
{
  return datum;
}

/** Setzt das Datum der AbteilungsListe */
void AbteilungsListe::setDatum(const QDate& _datum)
{
  datum=_datum;
}


int AbteilungsListe::getZeitDifferenz()
{
  return zeitDifferenz;
}

/**
 * Loescht alle Eintraege in der Abteilungsliste.
 */
void AbteilungsListe::clearKonten()
{
  zeitDifferenz=0;
  AbteilungsListe::iterator abtPos;

  for (abtPos=begin(); abtPos!=end(); ++abtPos) {

    KontoListe* kontoliste=&(abtPos->second);
    //kontoliste->setFlags(0);
    for (KontoListe::iterator kontPos=kontoliste->begin(); kontPos!=kontoliste->end(); ++kontPos) {
      UnterKontoListe* unterkontoliste=&(kontPos->second);
      //unterkontoliste->setFlags(0);
      for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin(); ukontPos!=unterkontoliste->end(); ++ukontPos) {
        EintragsListe* eintragsliste=&(ukontPos->second);
        //eintragsliste->setFlags(0);
        eintragsliste->clear();
        eintragsliste->insert(EintragsListe::value_type(0,UnterKontoEintrag()));
      }
    }
  }
}

/**
  * Laedt die Liste der Abteilungen neu ein
  */

  void AbteilungsListe::reload()
  {
    kontoDatenInfoSuccess = kontoDatenInfo->readInto(this);
  }

  bool AbteilungsListe::kontoDatenInfoConnected()
  {
    return kontoDatenInfoSuccess;
  }

  void AbteilungsListe::resetZeitDifferenz()
  {
     zeitDifferenz = 0;
  }