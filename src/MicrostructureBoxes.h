/*
 * MicrostructureBoxes.h
 *
 *  Created on: Nov 8, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MICROSTRUCTUREBOXES_H_
#define MICROSTRUCTUREBOXES_H_

#include "Microstructure.h"
#include <vector>


/**
 *@brief Box with diffusion connstant.
 */
struct box{
double x;	/**<@brief x position of box */
double y;	/**<@brief y position of box */
double z;	/**<@brief z position of box */
double size_x;	/**<@brief x box extends from [x-size_x..x+size_x] */
double size_y;	/**<@brief y box extends from [y-size_y..y+size_y] */
double size_z;	/**<@brief z box extends from [z-size_z..z+size_z] */
double D;	/**<@brief Diffusion constant inside box */
};


/**
 *@brief A microsructure composed of boxes
 */
class MicrostructureBoxes: public Microstructure {
public:
	MicrostructureBoxes();
	virtual ~MicrostructureBoxes();

	void AddBox(box new_box);
	virtual double GetD(int LastId=-1,int shellid=-1);
	virtual void IndexObject(triple pos,int &LastId ,int &shellid);
private:
	bool IsInsideObjectBox(triple pos, box cur_box);
	vector<box> m_boxes;		// container for all boxes inside microstructure

};

#endif /* MICROSTRUCTUREBOXES_H_ */
