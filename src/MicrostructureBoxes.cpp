/*
 * MicrostructureBoxes.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: dpflug
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

