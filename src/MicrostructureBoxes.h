/*
 * MicrostructureBoxes.h
 *
 *  Created on: Nov 8, 2010
 *      Author: dpflug
 */

#ifndef MICROSTRUCTUREBOXES_H_
#define MICROSTRUCTUREBOXES_H_

#include "Microstructure.h"
#include <vector>


struct box{
double x,y,z;					//	position of box
double size_x,size_y,size_z;	//	box extends from [x-size_x..x+size_x, y-size_y..y+size_y, z-size_z..z+size_z ]
double D;						// 	Diffusion constant inside box
};


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
