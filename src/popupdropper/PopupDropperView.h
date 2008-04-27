/* 
   Copyright (C) 2008 Jeff Mitchell <mitchell@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef POPUPDROPPER_VIEW_H
#define POPUPDROPPER_VIEW_H

#include <QGraphicsView>

class PopupDropper;
class PopupDropperViewPrivate;

class PopupDropperView : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY( bool quitOnDragLeave READ quitOnDragLeave WRITE setQuitOnDragLeave )

public:
    PopupDropperView( PopupDropper *pd, QGraphicsScene *scene, QWidget *parent );
    ~PopupDropperView();

    void dropEvent( QDropEvent *event );
    void dragMoveEvent( QDragMoveEvent *event );
    void dragLeaveEvent( QDragLeaveEvent *event );
    
    bool quitOnDragLeave() const;
    void setQuitOnDragLeave( bool quit );

private:
    friend class PopupDropperViewPrivate;
    PopupDropperViewPrivate* const d;
};

#endif /* AMAROK_POPUPDROPPER_VIEW_H */

