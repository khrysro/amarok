/***************************************************************************
                          viswidget.h  -  description
                             -------------------
    begin                : Die Jan 7 2003
    copyright            : (C) 2003 by Mark Kretschmann
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANALYZERBASE_H
#define ANALYZERBASE_H

#include <vector>

#define SINVEC_SIZE 6000

/**
 *@author Max
 */

class AnalyzerBase
{
    public:
        AnalyzerBase( uint );
        virtual ~AnalyzerBase();
        virtual void drawAnalyzer( std::vector<float> * ) = 0;
        uint timeout() const { return m_timeout; }

    protected:
        void interpolate( std::vector<float> *, std::vector<float> & ) const;
        virtual void init() = 0;
	
    private:
        uint m_timeout;
};

#endif
