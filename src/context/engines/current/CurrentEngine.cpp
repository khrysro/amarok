/***************************************************************************
 * copyright            : (C) 2007 Leo Franchi <lfranchi@gmail.com>        *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "CurrentEngine.h"

#include "Amarok.h"
#include "Debug.h"
#include "ContextObserver.h"
#include "ContextView.h"
#include "EngineController.h"
#include "meta/Meta.h"
#include "meta/MetaUtility.h"
#include "meta/SourceInfoCapability.h"

#include <QVariant>

using namespace Context;

CurrentEngine::CurrentEngine( QObject* parent, const QList<QVariant>& args )
    : DataEngine( parent )
    , ContextObserver( ContextView::self() )
    , m_coverWidth( 0 )
    , m_requested( true )
{
    DEBUG_BLOCK
    Q_UNUSED( args )
    m_sources = QStringList();
    m_sources << "current";
    update();
}

CurrentEngine::~CurrentEngine()
{
    DEBUG_BLOCK
}

QStringList CurrentEngine::sources() const
{
    DEBUG_BLOCK
    return m_sources; // we don't have sources, if connected, it is enabled.
}

bool CurrentEngine::sourceRequested( const QString& name )
{
    DEBUG_BLOCK
    Q_UNUSED( name );
/*    m_sources << name;    // we are already enabled if we are alive*/
    removeAllData( name );
    setData( name, QVariant());
    update();
    m_requested = true;
    return true;
}

void CurrentEngine::message( const ContextState& state )
{
    DEBUG_BLOCK

    m_currentTrack = The::engineController()->currentTrack();
    
    if( state == Current /*&& m_requested*/ )
    {
        debug() << "1";
        if( m_currentTrack )
        {
            debug() << "2";
            unsubscribeFrom( m_currentTrack );
            if( m_currentTrack->album() )
                unsubscribeFrom( m_currentTrack->album() );
        }
        update();
    }
}

void CurrentEngine::metadataChanged( Meta::Album* album )
{
    DEBUG_BLOCK
    setData( "current",  "albumart", album->image( coverWidth() ) );
}

void
CurrentEngine::metadataChanged( Meta::Track *track )
{
    DEBUG_BLOCK
    QVariantMap trackInfo = Meta::Field::mapFromTrack( track );
    setData( "current", "current", trackInfo );
}

void CurrentEngine::update()
{
    DEBUG_BLOCK

    m_currentTrack = The::engineController()->currentTrack();
    if( !m_currentTrack )
        return;
    subscribeTo( m_currentTrack );

    QVariantMap trackInfo = Meta::Field::mapFromTrack( m_currentTrack.data() );

    int width = coverWidth();
    if( m_currentTrack->album() )
        subscribeTo( m_currentTrack->album() );
    removeAllData( "current" );
    if( m_currentTrack->album() ) {

        //add a source info emblem ( if available ) to the cover

        QPixmap art = m_currentTrack->album()->image( width );
        setData( "current", "albumart",  QVariant( art ) );

     }
    else
    setData( "current", "albumart", QVariant( QPixmap() ) );
    setData( "current", "current", trackInfo );

    Meta::SourceInfoCapability *sic = m_currentTrack->as<Meta::SourceInfoCapability>();
    if( sic )
    {
        //is the source defined
        QString source = sic->sourceName();
        if ( !source.isEmpty() ) {
            setData( "current", "source_emblem",  QVariant( sic->emblem() ) );
        }
        delete sic;
    } else {
        setData( "current", "source_emblem",  QVariant( QPixmap() ) );
    }
}

#include "CurrentEngine.moc"
