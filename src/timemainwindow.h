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

#ifndef TIMEMAINWINDOW_H
#define TIMEMAINWINDOW_H

#include <qlistview.h>
#include <qmainwindow.h>
#include "kontodateninfozeit.h"
#include "unterkontodialog.h"
#include "kontotreeview.h"
#include "toolbar.h"
#include "qaction.h"
#include "sctimexmlsettings.h"

#define SIGINT_EVENT_ID QEvent::User

class StatusBar;

/** Diese Klasse implementiert das Hauptfenster des Programms,
    und sorgt zudem fuer das Fortschreiten der Zeit.
*/
class TimeMainWindow: public QMainWindow
{
  Q_OBJECT

  public:
    TimeMainWindow(AbteilungsListe *abtlist);
    QListView* getKontoTree() { return kontoTree; };
    virtual ~TimeMainWindow();
    SCTimeXMLSettings* settings;

  public slots:

    void callUnterKontoDialog(QListViewItem * item);

    void callDateDialog();
    
    void callConfigZeitIncDialog();
    
    void callAboutBox();

    void minutenUhr();

    void pause();

    void customEvent(QCustomEvent * e);

    void pauseAbzur(bool on);

    void zeitChanged();
    
    void updateCaption();

    void save();

    void resetDiff();
    
    void inPersoenlicheKonten(bool hinzufuegen);
    void flagsChanged(const QString& abt, const QString& ko, const QString& uko, int idx);
    void changeShortCutSettings(QListViewItem * item);
    
    void editUnterKontoPressed();
    void changeDate(const QDate& datum);
    void setAktivesProjekt(QListViewItem * item);
    void eintragHinzufuegen();
    void eintragEntfernen();
    void addDeltaToZeit(int delta);
    void addTimeInc();
    void subTimeInc();
    void addFastTimeInc();
    void subFastTimeInc();
    
    void callFindKontoDialog();
    void callHelpDialog();
    void callPreferenceDialog();
    void refreshKontoListe();

  signals:
    /** Wird ausgeloest, falls sich die Gesamtzeit geaendert hat. Uebergeben wird die neue Gesamtzahl der Sekunden. */
    void gesamtZeitChanged(int) ;

    /** Wird ausgeloest, falls sich die abzurechnende Gesamtzeit
      * geaendert hat. Uebergeben wird die neue Gesamtzahl der Sekunden. 
      */
    void gesamtZeitAbzurChanged(int) ;
    
    /**
      * Wird minuetlich ausgeloest, falls keine Pause aktiv ist.
      */
    void minuteTick();

    /**
      * Wird minuetlich ausgeloest, falls keine Pause aktiv ist.
      */
    void minuteAbzurTick();	

  private:
    KontoTreeView* kontoTree;
    UnterKontoDialog* unterKontoDialog;
    QAction* editUnterKontoAction;
    QAction* inPersKontAction;
    QAction* min5PlusAction;
    QAction* min5MinusAction;
    QAction* fastPlusAction;
    QAction* fastMinusAction;
    QAction* eintragAddAction;
    QAction* eintragRemoveAction;
    AbteilungsListe* abtList;
    StatusBar* statusBar;
    QMimeSourceFactory* mimeSourceFactory;
    ToolBar* toolBar;
    bool paused;
    bool pausedAbzur;

    // Workaround, um beim Setzen der Voreinstellung fuer den inPersoenlicheKonten-Button nicht das zugehoerige
    // Event auzuloesen. Wenn inPersoenlicheKontenAllowed=false, tut inPersoenlicheKonten(bool) gar nichts.
    bool inPersoenlicheKontenAllowed;
};

#endif
