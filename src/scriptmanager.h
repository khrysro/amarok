// (c) 2003 Scott Wheeler <wheeler@kde.org>,
// (c) 2004 Mark Kretschmann <markey@web.de>
// See COPYING file for licensing information.

#ifndef AMAROK_SCRIPTMANAGER_H
#define AMAROK_SCRIPTMANAGER_H

#include "playlistwindow.h"

#include <qmap.h>

#include <kdialogbase.h>    //baseclass
#include <kurl.h>

class ScriptManagerBase;
class QListViewItem;
class KProcess;

class ScriptManager : public KDialogBase
{
        Q_OBJECT

    public:
        ScriptManager( QWidget *parent = 0, const char *name = 0 );
        virtual ~ScriptManager();

        static ScriptManager* instance() { return s_instance ? s_instance : new ScriptManager( PlaylistWindow::self() ); }

    public slots:

    private slots:
        void slotAddScript();
        void slotRemoveScript();
        void slotEditScript();
        void slotRunScript();
        void slotStopScript();
        void slotConfigureScript();
        void scriptFinished( KProcess* process );

    private:
        static ScriptManager* s_instance;

        ScriptManagerBase* m_base;

        struct ScriptItem {
            KURL           url;
            KProcess*      process;
            QListViewItem* li;
        };

        typedef QMap<QString, ScriptItem> ScriptMap;

        ScriptMap m_scripts;
};


#endif /* AMAROK_SCRIPTMANAGER_H */


