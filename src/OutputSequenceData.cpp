/** @file OutputSequenceData.h
 *  @brief Implementation of JEMRIS OutputSequenceData
 */

#include "OutputSequenceData.h"
#include "config.h"
#include "Parameters.h"
#include "md5.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

/***********************************************************/
void OutputSequenceData::SetRotationMatrix(double alpha, double theta, double phi)
{
	double pi_180 = PI/180.0;
	phi   = -phi   * pi_180;    // Azimuth angle
	theta = theta  * pi_180;    // Inclination angle
	alpha = -alpha * pi_180;    // Rotation angle about axis

	// Define axis (ux,uy uz) and rotation angle from JEMRIS parameters
	double ux = sin(theta)*cos(phi);
	double uy = sin(theta)*sin(phi);
	double uz = cos(theta);
	double c0 = cos(alpha);
	double c = 1 - c0;
	double s = sin(alpha);

	// Compute rotation matrix from axis/angle representation
	// First row
	m_rot_matrix[0][0] = c0 + ux*ux*c;
	m_rot_matrix[0][1] = ux*uy*c - uz*s;
	m_rot_matrix[0][2] = ux*uz*c + uy*s;

	// Second row
	m_rot_matrix[1][0] = uy*ux*c + uz*s;
	m_rot_matrix[1][1] = c0 + uy*uy*c;
	m_rot_matrix[1][2] = uy*uz*c - ux*s;

	// Third row
	m_rot_matrix[2][0] = uz*ux*c - uy*s;
	m_rot_matrix[2][1] = uz*uy*c + ux*s;
	m_rot_matrix[2][2] = c0 + uz*uz*c;

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			if (fabs(m_rot_matrix[i][j])<1.0e-10)
				m_rot_matrix[i][j]=0.0;
}

/***********************************************************/
void OutputSequenceData::AddEvents(vector<Event*> &events, double duration)
{
	SeqBlock block;
	CompressedShape compressed;
	int idx;

	for (vector<Event*>::iterator it=events.begin(); it != events.end(); ++it)
	{
		Event *event = (*it);

		// Add RF events
		RFEvent *rf = dynamic_cast<RFEvent*>(event);
		if (rf!=NULL) {
			// Search library of basic shapes
			CompressShape(rf->m_magnitude, &compressed);
			idx = SearchLibrary(m_shape_library,compressed);
			if (idx>=m_shape_library.size())
				m_shape_library.push_back(compressed);
			rf->m_mag_shape=idx+1;

			CompressShape(rf->m_phase, &compressed);
			idx = SearchLibrary(m_shape_library,compressed);
			if (idx>=m_shape_library.size())
				m_shape_library.push_back(compressed);
			rf->m_phase_shape=idx+1;

			// Search library of RF events
			idx = SearchLibrary(m_rf_library,*rf);
			if (idx>=m_rf_library.size())
				m_rf_library.push_back(*rf);

			// Set index of current block
			block.events[SeqBlock::RF] = idx+1;
		}

		// Add linear and PatLoc gradient events
		GradEvent *grad = dynamic_cast<GradEvent*>(event);
		if (grad!=NULL && grad->m_channel<3) {
			if (grad->m_shape>=0) {
				// Search library of basic shapes
				CompressShape(grad->m_samples, &compressed);
				idx = SearchLibrary(m_shape_library,compressed);
				if (idx>=m_shape_library.size())
					m_shape_library.push_back(compressed);
				grad->m_shape=idx+1;
			}

			// Search library of Grad events
			idx = SearchLibrary(m_grad_library,*grad);
			if (idx>=m_grad_library.size())
				m_grad_library.push_back(*grad);

			// Set index of current block
			block.events[SeqBlock::XGRAD+grad->m_channel] = idx+1;
		}


		// Add ADC events
		ADCEvent *adc = dynamic_cast<ADCEvent*>(event);
		if (adc!=NULL) {
			// Search library of ADC events
			idx = SearchLibrary(m_adc_library,*adc);
			if (idx>=m_adc_library.size())
				m_adc_library.push_back(*adc);

			// Set index of current block
			block.events[SeqBlock::ADC] = idx+1;
		}

		// Add delay events
		DelayEvent *delay = dynamic_cast<DelayEvent*>(event);
		if (delay!=NULL) {
			// Search library of delay events
			idx = SearchLibrary(m_delay_library,*delay);
			if (idx>=m_delay_library.size())
				m_delay_library.push_back(*delay);

			// Set index of current block
			block.events[SeqBlock::DELAY] = idx+1;
		}
	}

	m_duration += duration;
	m_blocks.push_back(block);


}

/***********************************************************/
void OutputSequenceData::WriteFiles(const string &outDir, const string &outFile)
{
	string filePath = outDir + outFile;

	const double GRAD_TO_EXTERNAL = 1.0e6/(TWOPI);
	const double FREQ_TO_EXTERNAL = 1.0e3/(TWOPI);

	ofstream outfile (filePath.c_str(), ofstream::out);
	stringstream output;

	// Header
	output << "# Pulseq sequence format" << endl;
	output << "# Created by JEMRIS " << VERSION << endl << endl;

	// Determine if rotation matrix is not identity
	double distanceFromIdentity = 0.0;
	distanceFromIdentity += fabs(m_rot_matrix[0][0]-1.) + fabs(m_rot_matrix[0][1]) + fabs(m_rot_matrix[0][2]);
	distanceFromIdentity += fabs(m_rot_matrix[1][0]) + fabs(m_rot_matrix[1][1]-1.) + fabs(m_rot_matrix[1][2]);
	distanceFromIdentity += fabs(m_rot_matrix[2][0]) + fabs(m_rot_matrix[2][1]) + fabs(m_rot_matrix[2][2]-1.);

	// Output Pulseq Version
	output << "[VERSION]" << endl;
	output << "major 1" << endl;
	output << "minor 2" << endl;
	output << "revision 1" << endl;
	output << endl;

	// Output high level parameters
	Parameters* P = Parameters::instance();
	output << "[DEFINITIONS]" << endl;
	for (map<string,string>::iterator it=m_definitions.begin(); it!=m_definitions.end(); ++it)
		output << it->first << " " << it->second << endl;
	output << "Name " << outFile.substr(0, outFile.find(".", 0)) << endl;
	output << "Num_Blocks " << m_blocks.size() << endl;
	if (P->m_fov_x > 0 && P->m_fov_y > 0 && P->m_fov_z > 0){
		output << "FOV " << P->m_fov_x << " " << P->m_fov_y << " " << P->m_fov_z << endl; // FOV in [mm] in Pulseq 1.2.1 ([m] from 1.3.0 on)
		output << "Slice_Thickness " << P->m_fov_z << endl;
	}
	if (distanceFromIdentity>1e-6) {
		output << "Rot_Matrix " << setprecision(9)
		        << m_rot_matrix[0][0] << " " << m_rot_matrix[0][1] << " " << m_rot_matrix[0][2] << " "
		        << m_rot_matrix[1][0] << " " << m_rot_matrix[1][1] << " " << m_rot_matrix[1][2] << " "
		        << m_rot_matrix[2][0] << " " << m_rot_matrix[2][1] << " " << m_rot_matrix[2][2] << endl;
	}
	output << endl;

	// Determine width of block ID fields for right align
	int blockIdWidth;
	ostringstream ss;
	ss <<  m_blocks.size();
	blockIdWidth = ss.str().length();

	// Output blocks
	// ============================================================
	output << "# Format of blocks:" << endl;
	output << "#" << setw(blockIdWidth-1) << "#";
	output << "  D RF  GX  GY  GZ ADC" << endl;
	int eventWidths[] = {2,2,3,3,3,2};
	output << "[BLOCKS]" << endl;

	for (int iB=0; iB<m_blocks.size(); iB++)
	{
		SeqBlock & block = m_blocks[iB];
		output << setw(blockIdWidth) << iB+1;
		for (int iE=0; iE<SeqBlock::NUM_EVENTS; iE++)
			output << " " << setw(eventWidths[iE]) << block.events[iE];
		output << endl;
	}
	output << endl;

	// Output events
	// ============================================================

	// RF events
	if (m_rf_library.size()>0) {
		output << "# Format of RF events:" << endl;
		output << "# id amplitude mag_id phase_id delay freq phase" << endl;
		output << "# ..        Hz   ....     ....    us   Hz   rad" << endl;
		output << "[RF]" << endl;
		for (int iE=0; iE<m_rf_library.size(); iE++) {
			RFEvent &rf = m_rf_library[iE];
			output << iE+1 << " " <<  setw(12) << FREQ_TO_EXTERNAL*rf.m_amplitude
			        << " " << rf.m_mag_shape << " " << rf.m_phase_shape << " " << setw(3) << rf.m_delay
			        << " " << FREQ_TO_EXTERNAL*rf.m_freq_offset << " " << rf.m_phase_offset << endl;
		}
		output << endl;
	}

	// Count types of gradient events
	int numArbGrad=0, numTrapGrad=0;
	for (int iE=0; iE<m_grad_library.size(); iE++) {
			GradEvent &grad = m_grad_library[iE];
			if (grad.m_shape>0) numArbGrad++;
			if (grad.m_shape<0) numTrapGrad++;
	}

	// Arbitrary gradient events
	if (numArbGrad>0) {
		output << "# Format of arbitrary gradients:" << endl;
		output << "# id amplitude shape_id delay" << endl;
		output << "# ..      Hz/m     ....    us" << endl;
		output << "[GRADIENTS]" << endl;
		for (int iE=0; iE<m_grad_library.size(); iE++) {
			GradEvent &grad = m_grad_library[iE];
			if (grad.m_shape>0)
				output << iE+1 << " " << setw(12) << GRAD_TO_EXTERNAL*grad.m_amplitude << " " << grad.m_shape << " " << setw(3) << grad.m_delay << endl;
		}
		output << endl;
	}

	// Trapezoid gradient events
	if (numTrapGrad>0) {
		output << "# Format of trapezoid gradients:" << endl;
		output << "# id amplitude rise flat fall delay" << endl;
		output << "# ..      Hz/m   us   us   us    us" << endl;
		output << "[TRAP]" << endl;
		for (int iE=0; iE<m_grad_library.size(); iE++) {
			GradEvent &grad = m_grad_library[iE];
			if (grad.m_shape<0)
				output << setw(2) << iE+1 << " " <<  setprecision(7) << setw(13) << GRAD_TO_EXTERNAL*grad.m_amplitude << " " << setw(3) << grad.m_ramp_up_time
				        << " " << setw(4) << grad.m_flat_time << " " << setw(3) << grad.m_ramp_down_time << " " << setw(3) << grad.m_delay << endl;
		}
		output << endl;
	}

	// ADC events
	if (m_adc_library.size()>0) {
		output << "# Format of ADC events:" << endl;
		output << "# id num dwell delay freq phase" << endl;
		output << "# ..  ..    ns    us   Hz   rad" << endl;
		output << "[ADC]" << endl;
		for (int iE=0; iE<m_adc_library.size(); iE++) {
			ADCEvent &adc = m_adc_library[iE];
			output << setw(2) << iE+1 << " " << setw(3) << adc.m_num_samples << " " << setw(6) << adc.m_dwell_time
			        << " " << setw(3) << adc.m_delay << " " << adc.m_freq_offset*1.0e3/(2.0*PI) << " " << adc.m_phase_offset << endl;
		}
		output << endl;
	}

	// Delay events
	if (m_delay_library.size()>0) {
		output << "# Format of delays:" << endl;
		output << "# id delay (us)" << endl;
		output << "[DELAYS]" << endl;
		for (int iE=0; iE<m_delay_library.size(); iE++) {
			DelayEvent &delay = m_delay_library[iE];
			output << iE+1 << " " << delay.m_delay << endl;
		}
		output << endl;
	}

	// Output shapes
	// ============================================================

	output << "# Sequence Shapes" << endl << endl;
	output << "[SHAPES]" << endl << endl;

	for (int iS=0; iS<m_shape_library.size(); iS++)
	{
		CompressedShape &shape = m_shape_library[iS];
		output << "shape_id " <<  iS+1 << endl;
		output << "num_samples " << shape.m_num_uncompressed_samples << endl;
		for (int k=0; k<shape.m_samples.size(); k++)
			output << setprecision(std::numeric_limits<float>::digits10) << shape.m_samples[k] << endl; // Pulseq sequence has floating point preicision
		output << endl;
	}
	output << endl;

	// Write stringstream to file
	outfile << output.rdbuf();

	// Add MD5 signature
	string output_str = output.str();
	string signature = md5(output_str);
	outfile << "[SIGNATURE]" << endl;
	outfile << "Type " << "md5" << endl;
	outfile << "Hash " << signature << endl;

	outfile.close();

}

/***********************************************************/
template<typename T> int OutputSequenceData::SearchLibrary(vector<T> &library, T &obj)
{
	for (int i=0; i<library.size(); i++) {
		if (library[i]==obj) {
			return i;
		}
	}
	// Not found, return size of library (index for new element)
	return library.size();
}

/***********************************************************/
void OutputSequenceData::CompressShape(vector<double> &shape, CompressedShape *out)
{
	out->m_samples.clear();
	out->m_num_uncompressed_samples=shape.size();

	double *diff = new double[shape.size()];
	diff[0]=shape[0];
	for (int i=1; i<shape.size(); i++) {
		diff[i]=shape[i]-shape[i-1];
		if (abs(diff[i])<1e-12)
			diff[i]=0.0;
	}

	int idx=0;
	while (idx<shape.size()) {
		double curr=diff[idx];
		int count=0;
		// Pulseq sequence has floating point precision
		while (idx<shape.size()-1 && fabs(curr-diff[idx+1]) <= ( (fabs(curr)<fabs(diff[idx+1]) ? fabs(diff[idx+1]) : fabs(curr)) * 10 * std::numeric_limits<float>::epsilon())){
			count++;
			idx++;
		}
		if (count==0)
			out->m_samples.push_back(curr);
		else {
			out->m_samples.push_back(curr);
			out->m_samples.push_back(curr);
			out->m_samples.push_back(count-1);
		}
		idx++;
	}
	delete[] diff;

}
