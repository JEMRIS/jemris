/*
 * MicrostructureBoxes.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#include "MicrostructureBoxes.h"

MicrostructureBoxes::MicrostructureBoxes() {
}

MicrostructureBoxes::~MicrostructureBoxes() {
}

/***********************************************************/
void MicrostructureBoxes::AddBox(box new_box) {
	m_boxes.push_back(new_box);
}
/***********************************************************/
bool MicrostructureBoxes::IsInsideObjectBox(triple pos, box cur_box){
	if ((pos.x > (cur_box.x - cur_box.size_x)) && (pos.x < (cur_box.x + cur_box.size_x)) && (pos.y > (cur_box.y - cur_box.size_y)) && (pos.y < (cur_box.y + cur_box.size_y)) && (pos.z > (cur_box.z - cur_box.size_z)) && (pos.z < (cur_box.z + cur_box.size_z))) {
		return true;
	} else {
		return false;
	}
}
/***********************************************************/
double MicrostructureBoxes::GetD(int ObjectID ,int shellid) {
	if (ObjectID == -1) {return m_D_external;};
	return m_boxes[ObjectID].D;
};

/***********************************************************/
void MicrostructureBoxes::IndexObject(triple pos,int &LastId ,int &shellid) {

	if (LastId >-1) {
		// If last position was not in extracellular space,
		// first test previous object, spin will most likely stay within this one:
		if (IsInsideObjectBox(pos,m_boxes[LastId])) {
			// no need to modify LastId:
			return;
		}
	}
	// now test all boxes:
	for (int i=0;i<m_boxes.size();i++){
		if (IsInsideObjectBox(pos,m_boxes[i])) {
			// found right box:
			LastId = i;
			return;
		}
	}
	//still not returned? position is in extracellullar space.
	LastId=-1;
	return;
}

