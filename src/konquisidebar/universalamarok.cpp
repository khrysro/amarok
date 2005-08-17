/***************************************************************************
 *   Copyright (C) 2004 by Marco Gulino                                    *
 *   marco@Paganini                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "universalamarok.h"

#include <qlabel.h>
#include <kinstance.h>
#include <klocale.h>
#include <qstring.h>
#include <qwidget.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <qlayout.h>
#include <qtimer.h>
#include <dcopclient.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <ktoolbar.h>
#include <kapplication.h>
#include <qslider.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <khtmlview.h>

#define HTML_FILE KGlobal::dirs()->saveLocation( "data", "amarok/", true ) + "contextbrowser.html"

amarokWidget::amarokWidget( QWidget * parent, const char * name, WFlags f )
                 : QVBox(parent, name, f)
{
    setAcceptDrops(true);
}
void amarokWidget::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept( KURLDrag::canDecode(event) );
}

void amarokWidget::dropEvent(QDropEvent* event)
{
    KURL::List urlList;
    if( KURLDrag::decode(event, urlList) )
    {
        KURL::List::iterator it;
        for (it = urlList.begin(); it != urlList.end(); ++it )
            emit emitURL(*it);
    }
}

bool amarokWidget::eventFilter( QObject *, QEvent *e )
{
    if(e->type() < QEvent::DragEnter || e->type() > QEvent::Drop )
        return false;
    QApplication::sendEvent(this, e);
    return true;
}

UniversalAmarok::UniversalAmarok(KInstance *inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name):
                   KonqSidebarPlugin(inst,parent,widgetParent,desktopName,name)
{
    widget=new amarokWidget(widgetParent);
//    widgetParent->resize(580,300);
    browser = new KHTMLPart(widget, "widget-browser");
//browser=new KHTMLPart(widget);
kdDebug() << "parentPart() << " << browser->parentPart() << endl;
    browser->setDNDEnabled( true );
    browser->setEncoding("utf8", true);
    updateBrowser(HTML_FILE);
browser->view()->installEventFilter(widget);
    amarokDCOP=new DCOPClient();
    amarokDCOP->attach();
    KToolBar* toolBar=new KToolBar(widget, "PlayerControls");
    toolBar->setMaximumHeight(48);
    toolBar->insertButton("player_start",0,SIGNAL(clicked() ),this, SLOT(sendPrev() ) );
    toolBar->insertButton("player_play",0,SIGNAL(clicked() ),this, SLOT(sendPlay() ) );
    toolBar->insertButton("player_pause",0,SIGNAL(clicked() ),this, SLOT(sendPause() ) );
    toolBar->insertButton("player_stop",0,SIGNAL(clicked() ),this, SLOT(sendStop() ) );
    toolBar->insertButton("player_end",0,SIGNAL(clicked() ),this, SLOT(sendNext() ) );

    toolBar->insertSeparator();
    toolBar->insertButton("arts",0,SIGNAL(clicked() ),this, SLOT(sendMute() ) );
    toolBar->insertSeparator();
    toolBar->insertButton("gohome",0,SIGNAL(clicked() ),this, SLOT(cbHome() ) );
    toolBar->insertButton("today",0,SIGNAL(clicked() ),this, SLOT(currentTrack() ) );
    toolBar->insertButton("document",0,SIGNAL(clicked() ),this, SLOT(lyrics() ) );


    vol_slider=new QSlider(0,100,1,0,Qt::Vertical, toolBar,"volume");
    connect(vol_slider, SIGNAL( valueChanged(int) ), this, SLOT(volChanged(int ) ) );
    toolBar->insertWidget(1,2, vol_slider);

    fileInfo = new QFileInfo(HTML_FILE);
    QTimer *t = new QTimer( this );
    connect( t, SIGNAL(timeout()), SLOT(updateStatus() ) );
    t->start( 2000, false );
kdDebug() << "Connecting widget signal" << endl;
    connect( widget, SIGNAL(emitURL( const KURL &)), this, SLOT(openURLRequest( const KURL &) ) );
        connect( browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ),
             this,                          SLOT( openURLRequest( const KURL & ) ) );
    widget->show();
}


UniversalAmarok::~UniversalAmarok()
{
    delete fileInfo;
}


#include "universalamarok.moc"


// FIXME: is this referenced from anywhere ??!

extern "C"
{
    void* KDE_EXPORT create_konqsidebar_universalamarok(KInstance *instance,QObject *par,QWidget *widp,QString &desktopname,const char *name)
    {
        return new UniversalAmarok(instance,par,widp,desktopname,name);
    }
}

// FIXME: Is this referenced from anywhere ??!
extern "C"
{
    KDE_EXPORT bool add_konqsidebar_universalamarok(QString* fn, QString* param, QMap<QString,QString> *map)
        {
        Q_UNUSED(param);

        map->insert ("Type", "Link");
        map->insert ("URL", "");
        map->insert ("Icon", "amarok");
        map->insert ("Name", i18n ("amaroK"));
        map->insert ("Open", "true");
        map->insert ("X-KDE-KonqSidebarModule","konqsidebar_universalamarok");
        fn->setLatin1 ("amarok.desktop");
        return true;
    }
}


/*!
    \fn UniversalAmarok::updateBrowser()
 */
void UniversalAmarok::updateBrowser(const QString& file)
{
    if (! (QFile::exists(file) ) )
    {
        showIntroduction();
        return;
    }
    browser->openURL(file);
}


/*!
    \fn UniversalAmarok::updateStatus()
 */
void UniversalAmarok::updateStatus()
{
    fileInfo->refresh();
    if( fileInfo->lastModified() != fileDT )
    {
        updateBrowser( HTML_FILE );
        fileDT=fileInfo->lastModified();
    }
}


/*!
    \fn UniversalAmarok::getCurrentPlaying()
 */
QString UniversalAmarok::getCurrentPlaying()
{
    QCString returnType;
    QByteArray returnData;
    QString result;
    if(! amarokDCOP->call("amarok", "player", "nowPlaying()", QByteArray(), returnType, returnData) ) return NULL;
    if(returnType!="QString") return NULL;
    QDataStream dataparsing(returnData, IO_ReadOnly);
    dataparsing >> result;
    return result;
}


/*!
    \fn UniversalAmarok::openURLRequest( const KURL &url )
 */
void UniversalAmarok::openURLRequest( const KURL &url )
{
   kdDebug() << "amarok-sidebar: Catched url request: " << url << endl;
   checkForAmarok();
   QByteArray data;
   QDataStream arg(data, IO_WriteOnly);
   arg << url;
   amarokDCOP->send("amarok", "playlist", "playMedia(KURL)", data);
}


/*!
    \fn UniversalAmarok::checkForAmarok()
 */
void UniversalAmarok::checkForAmarok()
{
    if(amarokDCOP->isApplicationRegistered("amarok")) return;
    KApplication::kdeinitExecWait("amarok");
}

void UniversalAmarok::volChanged(int vol)
{
    checkForAmarok();
    QByteArray data;
    QDataStream arg(data, IO_WriteOnly);
    arg << 100-vol;
    amarokDCOP->send("amarok", "player", "setVolume(int)", data );
}

void UniversalAmarok::showIntroduction()
{
    QString  m_HTMLSource="";
    m_HTMLSource.append(
            "<html>"
            "<div id='introduction_box' class='box'>"
                "<div id='introduction_box-header' class='box-header'>"
                    "<span id='introduction_box-header-title' class='box-header-title'>"
                    + i18n( "Hello amaroK user!" ) +
                    "</span>"
                "</div>"
                "<div id='introduction_box-body' class='box-body'>"
                    "<p>" +
                    i18n( "This is the Context Browser: "
                          "it shows you contextual information about the currently playing track. "
                          "In order to use this feature of amaroK, you need to build a Collection."
                        ) +
                    "</p>"
                    "<a href='show:collectionSetup' class='button'>" + i18n( "Build Collection..." ) + "</a>"
                "</div>"
            "</div>"
            "</html>"
                       );
kdDebug() << m_HTMLSource << endl;
    browser->begin();
    browser->write( m_HTMLSource );
    browser->end();
}
