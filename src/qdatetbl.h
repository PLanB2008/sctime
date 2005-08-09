/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2003 Florian Schmitt <f.schmitt@science-computing.de>
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// This file has been ported from KDE to plain QT

#ifndef QDATETBL_H
#define QDATETBL_H

#include <qvalidator.h>
#include <q3gridview.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qcolor.h>
#include "qcalendarsystemgregorian.h"
//Added by qt3to4:
#include <QWheelEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <Q3Frame>
#include <QResizeEvent>
#include <Q3PopupMenu>
#include <QMouseEvent>

class Q3PopupMenu;

/** Week selection widget.
* @internal
* @version $Id$
* @author Stephan Binner
*/
class QDateInternalWeekSelector : public QLineEdit
{
  Q_OBJECT
protected:
  QIntValidator *val;
  int result;
public slots:
  void weekEnteredSlot();
  void setMaxWeek(int max);
signals:
  void closeMe(int);
public:
  QDateInternalWeekSelector( QWidget* parent=0, const char* name=0);
  int getWeek();
  void setWeek(int week);

private:
  class QDateInternalWeekPrivate;
  QDateInternalWeekPrivate *d;
};

/**
* A table containing month names. It is used to pick a month directly.
* @internal
* @version $Id$
* @author Tim Gilman, Mirko Boehm
*/
class QDateInternalMonthPicker : public Q3GridView
{
  Q_OBJECT
protected:
  /**
   * Store the month that has been clicked [1..12].
   */
  int result;
  /**
   * the cell under mouse cursor when LBM is pressed
   */
  short int activeCol;
  short int activeRow;
  /**
   * Contains the largest rectangle needed by the month names.
   */
  QRect max;
signals:
  /**
   * This is send from the mouse click event handler.
   */
  void closeMe(int);
public:
  /**
   * The constructor.
   */
  QDateInternalMonthPicker(QCalendarSystem* calendar_, const QDate& date, QWidget* parent, const char* name=0);
  /**
   * The destructor.
   */
  ~QDateInternalMonthPicker();
  /**
   * The size hint.
   */
  QSize sizeHint() const;
  /**
   * Return the result. 0 means no selection (reject()), 1..12 are the
   * months.
   */
  int getResult() const;
protected:
  /**
   * Set up the painter.
   */
  void setupPainter(QPainter *p);
  /**
   * The resize event.
   */
  virtual void viewportResizeEvent(QResizeEvent*);
  /**
   * Paint a cell. This simply draws the month names in it.
   */
  virtual void paintCell(QPainter* painter, int row, int col);
  /**
   * Catch mouse click and move events to paint a rectangle around the item.
   */
  virtual void contentsMousePressEvent(QMouseEvent *e);
  virtual void contentsMouseMoveEvent(QMouseEvent *e);
  /**
   * Emit monthSelected(int) when a cell has been released.
   */
  virtual void contentsMouseReleaseEvent(QMouseEvent *e);

private:
  class QDateInternalMonthPrivate;
  QDateInternalMonthPrivate *d;
  QCalendarSystem* calendar;
};

/** Year selection widget.
* @internal
* @version $Id$
* @author Tim Gilman, Mirko Boehm
*/
class QDateInternalYearSelector : public QLineEdit
{
  Q_OBJECT
protected:
  QIntValidator *val;
  int result;
public slots:
  void yearEnteredSlot();
signals:
  void closeMe(int);
public:
  QDateInternalYearSelector(QCalendarSystem* calendar_, QWidget* parent=0, const char* name=0);
  int getYear();
  void setYear(int year);

private:
  class QDateInternalYearPrivate;
  QDateInternalYearPrivate *d;
  QCalendarSystem* calendar;
};

/**
 * Frame with popup menu behavior.
 * @author Tim Gilman, Mirko Boehm
 * @version $Id$
 */
class QPopupFrame : public Q3Frame
{
  Q_OBJECT
protected:
  /**
   * The result. It is returned from exec() when the popup window closes.
   */
  int result;
  /**
   * Catch key press events.
   */
  virtual void keyPressEvent(QKeyEvent* e);
  /**
   * The only subwidget that uses the whole dialog window.
   */
  QWidget *main;
public slots:
  /**
   * Close the popup window. This is called from the main widget, usually.
   * @p r is the result returned from exec().
   */
  void close(int r);
public:
  /**
   * The contructor. Creates a dialog without buttons.
   */
  QPopupFrame(QWidget* parent=0, const char*  name=0);
  /**
   * Set the main widget. You cannot set the main widget from the constructor,
   * since it must be a child of the frame itselfes.
   * Be careful: the size is set to the main widgets size. It is up to you to
   * set the main widgets correct size before setting it as the main
   * widget.
   */
  void setMainWidget(QWidget* m);
  /**
   * The resize event. Simply resizes the main widget to the whole
   * widgets client size.
   */
  virtual void resizeEvent(QResizeEvent*);
  /**
   * Open the popup window at position pos.
   */
  void popup(const QPoint &pos);
  /**
   * Execute the popup window.
   */
  int exec(QPoint p);
  /**
   * Dito.
   */
  int exec(int x, int y);

private:

  virtual bool close(bool alsoDelete) { return Q3Frame::close(alsoDelete); }
protected:
  virtual void virtual_hook( int id, void* data );
private:
  class QPopupFramePrivate;
  QPopupFramePrivate *d;
};

/**
* Validates user-entered dates.
*/
class QDateValidator : public QValidator
{
public:
    QDateValidator(QWidget* parent=0, const char* name=0);
    virtual State validate(QString&, int&) const;
    virtual void fixup ( QString & input ) const;
    State date(const QString&, QDate&) const;
};

/**
 * Date selection table.
 * This is a support class for the KDatePicker class.  It just
 * draws the calender table without titles, but could theoretically
 * be used as a standalone.
 *
 * When a date is selected by the user, it emits a signal:
 * dateSelected(QDate)
 *
 * @internal
 * @version $Id$
 * @author Tim Gilman, Mirko Boehm
 */
class QDateTable : public Q3GridView
{
    Q_OBJECT
    Q_PROPERTY( QDate date READ getDate WRITE setDate )
    Q_PROPERTY( bool popupMenu READ popupMenuEnabled WRITE setPopupMenuEnabled )

public:
    /**
     * The constructor.
     */
    QDateTable(QCalendarSystem* calendar_, QWidget *parent=0,
	       QDate date=QDate::currentDate(),
	       const char* name=0, Qt::WFlags f=0);

    /**
     * The destructor.
     */
    ~QDateTable();

    /**
     * Returns a recommended size for the widget.
     * To save some time, the size of the largest used cell content is
     * calculated in each paintCell() call, since all calculations have
     * to be done there anyway. The size is stored in maxCell. The
     * sizeHint() simply returns a multiple of maxCell.
     */
    virtual QSize sizeHint() const;
    /**
     * Set the font size of the date table.
     */
    void setFontSize(int size);
    /**
     * Select and display this date.
     */
    bool setDate(const QDate&);
    // ### 4.0 rename to date()
    const QDate& getDate() const;

    /**
     * Enables a popup menu when right clicking on a date.
     *
     * When it's enabled, this object emits a aboutToShowContextMenu signal
     * where you can fill in the menu items.
     *
     * @since 3.2
     */
    void setPopupMenuEnabled( bool enable );

    /**
     * Returns if the popup menu is enabled or not
     */
    bool popupMenuEnabled() const;

    enum BackgroundMode { NoBgMode=0, RectangleMode, CircleMode };

    /**
     * Makes a given date be painted with a given foregroundColor, and background
     * (a rectangle, or a circle/ellipse) in a given color.
     *
     * @since 3.2
     */
    void setCustomDatePainting( const QDate &date, const QColor &fgColor, BackgroundMode bgMode=NoBgMode, const QColor &bgColor=QColor());

    /**
     * Unsets the custom painting of a date so that the date is painted as usual.
     *
     * @since 3.2
     */
    void unsetCustomDatePainting( const QDate &date );

protected:
    /**
     * calculate the position of the cell in the matrix for the given date. The result is the 0-based index.
     */
    int posFromDate( const QDate &date ); // KDE4: make this virtual, so subclasses can reimplement this and use a different default for the start of the matrix
    /**
     * calculate the date that is displayed at a given cell in the matrix. pos is the
     * 0-based index in the matrix. Inverse function to posForDate().
     */
    QDate dateFromPos( int pos ); // KDE4: make this virtual

    /**
     * Paint a cell.
     */
    virtual void paintCell(QPainter*, int, int);
    /**
     * Handle the resize events.
     */
    virtual void viewportResizeEvent(QResizeEvent *);
    /**
     * React on mouse clicks that select a date.
     */
    virtual void contentsMousePressEvent(QMouseEvent *);
    virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);
    virtual void wheelEvent( QWheelEvent * e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );

    // ### KDE 4.0 make the following private and mark as members

    /**
     * The font size of the displayed text.
     */
    int fontsize;
    /**
     * The currently selected date.
     */
    QDate date;

    QCalendarSystem* calendar;
    /**
     * The day of the first day in the month [1..7].
     */
    int firstday;
    /**
     * The number of days in the current month.
     */
    int numdays;
    /**
     * The number of days in the previous month.
     */
    int numDaysPrevMonth;
    /**
     * unused
     * ### remove in KDE 4.0
     */
    bool unused_hasSelection;
    /**
     * Save the size of the largest used cell content.
     */
    QRect maxCell;
signals:
    /**
     * The selected date changed.
     */
    // ### KDE 4.0 make parameter a const reference
    void dateChanged(QDate);
    /**
     * This function behaves essentially like the one above.
     * The selected date changed.
     * @param cur The current date
     * @param old The date before the date was changed
     */
    void dateChanged(const QDate& cur, const QDate& old);
    /**
     * A date has been selected by clicking on the table.
     */
    void tableClicked();

    void tableDoubleClicked();

    /**
     * A popup menu for a given date is about to be shown (as when the user
     * right clicks on that date and the popup menu is enabled). Connect
     * the slot where you fill the menu to this signal.
     *
     * @since 3.2
     */
    void aboutToShowContextMenu( Q3PopupMenu * menu, const QDate &date);

protected:
  virtual void virtual_hook( int id, void* data );
private:
    class QDateTablePrivate;
    QDateTablePrivate *d;
};

#endif // KDATETBL_H
