// (c) Mark Kretschmann 2004
// See COPYING file for licensing information

    
#include "config.h"

#ifdef HAVE_SQLITE

#include "collectionbrowser.h"
#include "directorylist.h"
#include "metabundle.h"
#include "threadweaver.h"

#include <sqlite.h>

#include <qcstring.h>
#include <qptrlist.h>
#include <qpushbutton.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kdirlister.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kurldrag.h>    //dragObject()


CollectionBrowser::CollectionBrowser( const char* name )
    : QVBox( 0, name )
{
    QPushButton* button = new QPushButton( "Select Folders", this );
    new Browser( this );

    connect( button, SIGNAL( clicked() ), this, SLOT( setupDirs() ) );
}


//////////////////////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////////////////////

void
CollectionBrowser::setupDirs() //SLOT
{
    DirectoryList list( m_dirs.toStringList(), false );
    DirectoryList::Result result = list.exec();

    m_dirs = KURL::List( result.dirs );
}


//////////////////////////////////////////////////////////////////////////////////////////
// CLASS Browser
//////////////////////////////////////////////////////////////////////////////////////////

CollectionBrowser::Browser::Browser( QWidget* parent )
    : KIconView( parent, 0 )
    , m_parent( static_cast<CollectionBrowser*>( parent ) )
    , m_weaver( new ThreadWeaver( this ) )
    , m_dirLister( new KDirLister() )
{
    kdDebug() << k_funcinfo << endl;
        
    setSelectionMode( QIconView::Extended );
    setItemsMovable( false );

    m_db = sqlite_open( ( KGlobal::dirs()->saveLocation( "data", kapp->instanceName() + "/" )
                          + "collection.db" ).latin1(),
                        0, 0 );
    
    if ( !m_db )
        kdWarning() << k_funcinfo << "Could not open SQLite database\n";
    
    QCString command = "create table tags ( title varchar(100), artist varchar(100) );";
    execSql( command, 0, 0 );
                
    KURL url;
    url.setPath( "/home/mark/mp3" );
    m_parent->m_dirs << url;
    readDir( url );                
}


CollectionBrowser::Browser::~Browser()
{
    kdDebug() << k_funcinfo << endl;
    
    delete m_dirLister;
    sqlite_close( m_db );
}


//////////////////////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////////////////////

void 
CollectionBrowser::Browser::readDir( const KURL& url )
{
    m_dirLister->openURL( url );
    
    while ( !m_dirLister->isFinished() )
        kapp->processEvents(); 
       
    m_weaver->append( new CollectionReader( this, m_dirLister->items() ) );
}


void 
CollectionBrowser::Browser::customEvent( QCustomEvent *e )
{
    kdDebug() << k_funcinfo << endl;
    
    if ( e->type() == ThreadWeaver::Job::CollectionReader ) {
        CollectionReader* c = static_cast<CollectionReader*>( e );
       
        kdDebug() << "********************************\n";
        kdDebug() << "CollectionEvent arrived.\n";
        kdDebug() << "********************************\n";
        
        kdDebug() << "Number of bundles: " << c->list().count() << endl;

        MetaBundle* bundle;
        
        for ( int i = 0; i < c->list().count(); i++ ) {
            bundle = c->list().at( i );
//             kdDebug() << bundle->artist() << endl;
            
            QCString command = "insert into tags( artist ) values ('";
            command += bundle->artist().latin1();
            command +=         "');";
            
            execSql( command, 0, 0 );
            delete bundle;
        }
    }
    dumpDb();
}


void 
CollectionBrowser::Browser::dumpDb()
{
    kdDebug() << k_funcinfo << endl;
    
    QCString command = "select artist from tags;";
   
    QStringList values;
    QStringList names;
    
    execSql( command, &values, &names );
    kdDebug() << values << endl;
}


bool 
CollectionBrowser::Browser::execSql( const QCString& statement,
                                     QStringList* const values,
                                     QStringList* const names )
{
    kdDebug() << k_funcinfo << endl;
    
    if ( !m_db ) {
        kdWarning() << k_funcinfo << "SQLite pointer == NULL.\n";
        return false;
    }
    
    const char* tail;
    sqlite_vm* vm;
    char* errorStr;
    int error;
    //compile SQL program to virtual machine
    error = sqlite_compile( m_db, statement, &tail, &vm, &errorStr );
        
    if ( error != SQLITE_OK ) {
        kdWarning() << k_funcinfo << "sqlite_compile error:\n";
        kdWarning() << errorStr << endl;
        sqlite_freemem( errorStr );
        return false;
    }
    
    int n;
    const char** value;
    const char** colName;
    //execute virtual machine by iterating over rows
        while( true ) {
        error = sqlite_step( vm, &n, &value, &colName );
        
        if ( error == SQLITE_DONE || error == SQLITE_ERROR )
            break; 
        if ( values && value )
            *values << *value;
        if ( names && colName )
            *names << *colName;
    }
    //deallocate vm ressources
    sqlite_finalize( vm, 0 );
    
    if ( error != SQLITE_DONE ) {
        kdWarning() << k_funcinfo << "sqlite_step error.\n";
        return "error";
    }
        
    return true;
}


// QDragObject*
// CollectionBrowser::dragObject()
// {
//     return new KURLDrag( currentItem()->url(), this );
// }


#include "collectionbrowser.moc"

#endif /* HAVE_SQLITE */


