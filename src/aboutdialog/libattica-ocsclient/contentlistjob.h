/*
    This file is part of KDE.

    Copyright (c) 2008 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
    USA.
 */
#ifndef ATTICA_CONTENTLISTJOB_H
#define ATTICA_CONTENTLISTJOB_H

#include "content.h"

#include <QUrl>
#include <kjob.h>

namespace KIO {
class Job;
}

namespace AmarokAttica {

class ATTICA_EXPORT ContentListJob : public KJob
{
    Q_OBJECT
  public:
    ContentListJob();

    void setUrl( const QUrl & );

    void start();

    Content::List contentList() const;
    
  protected Q_SLOTS:
    void doWork();

    void slotJobResult( KJob *job );
    void slotJobData( KIO::Job *job, const QByteArray &data );
    
  private:
    QUrl m_url;
    KIO::Job *m_job;
    QByteArray m_data;
  
    Content::List m_contentList;
};

}

#endif
