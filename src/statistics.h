/***************************************************************************
 * copyright            : (C) 2005-2006 Seb Ruiz <me@sebruiz.net>          *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AMAROK_STATISTICS_H
#define AMAROK_STATISTICS_H

#include "playlistwindow.h"

#include <kdialogbase.h>    //baseclass
#include <klistview.h>      //baseclass

#include <qtimer.h>

class ClickLineEdit;
class QColor;
class QTimer;

class StatisticsList;
class StatisticsItem;
class StatisticsDetailedItem;

class Statistics : public KDialogBase
{
        Q_OBJECT

    public:
        Statistics( QWidget *parent = 0, const char *name = 0 );
        ~Statistics();

        static Statistics *instance() { return s_instance; }

    private slots:
        void    slotSetFilter();
        void    slotSetFilterTimeout();

    private:
        StatisticsList *m_listView;
        ClickLineEdit  *m_lineEdit;
        QTimer         *m_timer;

        static Statistics *s_instance;
};

class StatisticsList : public KListView
{
        Q_OBJECT

    public:
        StatisticsList( QWidget *parent, const char *name=0 );
        ~StatisticsList() {};
        
        QString filter()                           { return m_filter; }
        void    setFilter( const QString &filter ) { m_filter = filter; }
        void    renderView();

    private slots:
        void    clearHover();
        void    itemClicked( QListViewItem *item );
        void    startHover( QListViewItem *item );

    private:
        void    startDrag();
        void    viewportPaintEvent( QPaintEvent* );
        void    expandInformation( StatisticsItem *item );

        StatisticsItem *m_trackItem;
        StatisticsItem *m_mostplayedItem;
        StatisticsItem *m_artistItem;
        StatisticsItem *m_albumItem;
        StatisticsItem *m_genreItem;
        StatisticsItem *m_newestItem;

        QListViewItem  *m_currentItem;
        QString         m_filter;
};

class StatisticsItem : public QObject, public KListViewItem
{
        Q_OBJECT

    public:
        StatisticsItem( QString text, StatisticsList *parent, KListViewItem *after=0, const char *name=0 );
        ~StatisticsItem() {};

        void    paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
        void    paintFocus( QPainter*, const QColorGroup& , const QRect& ) {};  //reimp
        void    setPixmap( const QString &pix );

        void    enterHover();
        void    leaveHover();

        void       setExpanded( const bool b ) { m_isExpanded = b; }
        const bool isExpanded() { return m_isExpanded; }

        void        setSubtext( QString t ) { m_subText = t; }

    protected:
        static const int ANIM_INTERVAL = 18;
        static const int ANIM_MAX = 20;

    private slots:
        void slotAnimTimer();

    private:
        QColor  blendColors( const QColor& color1, const QColor& color2, int percent );

        QTimer *m_animTimer;
        bool    m_animEnter;
        int     m_animCount;

        bool    m_isActive;
        bool    m_isExpanded;

        QString m_subText;
};

class StatisticsDetailedItem : public KListViewItem
{
    public:
        StatisticsDetailedItem( QString &text, StatisticsItem *parent,
                                StatisticsDetailedItem *after=0, const char *name=0 );
        ~StatisticsDetailedItem() {};

        enum    ItemType { NONE, TRACK, ARTIST, ALBUM, GENRE, HISTORY };

        void    setItemType( const ItemType t ) { m_type = t; }
        const   ItemType itemType() { return m_type; }

        void    setUrl( QString &url ) { m_url = url; }
        const   QString url() { return m_url; }

        void    paintFocus( QPainter*, const QColorGroup& , const QRect& ) {};  //reimp

    private:
        ItemType m_type;
        QString  m_url;
};


#endif /* AMAROK_STATISTICS_H */
