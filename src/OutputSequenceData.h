/** @file OutputSequenceData.h
 *  @brief Implementation of JEMRIS OutputSequenceData
 */

#ifndef OUTPUTSEQUENCEDATA_H_
#define OUTPUTSEQUENCEDATA_H_

#include "Event.h"
#include "Declarations.h"

#include <vector>
#include <map>

/**
 * @brief Sequence block for execution on hardware.
 *
 * The main role of this class is to store indices of the events occurring in this
 * block. The event details are contained in the OutputSequenceData class.
 * @see OutputSequenceData
 */
class SeqBlock
{
public:
	/**
	 * @brief Open file format event order
	 */
	enum EventCode {
		RF,
		XGRAD,
		YGRAD,
		ZGRAD,
		ADC
	};

	/**
	 * @brief Number of event types per sequence block
	 */
	static const int NUM_EVENTS = (ADC+1);

	/**
	 * @brief Constructor
	 */
	SeqBlock(){ for (int i=0; i<NUM_EVENTS; i++) events[i]=0; }

    /**
     * @brief List of event IDs
     *
     * Array contains integer IDs to events stored in the
     * event libraries of the OutputSequenceData class.
     * The order is given by the enum EventCode.
     */
	int events[NUM_EVENTS];     // RF X Y Z ADC

	double duration;          /**< @brief Duration of this block in ms */

};

/**
 * @brief Maintain libraries of hardware events and write sequence file.
 *
 * This class maintains a list of compressed sequence events suitable
 * for execution on hardware. A sequence file can be generated for execution on real
 * MR hardware according to the open file format as part of the *Pulseq* project
 * (http://pulseq.github.io).
 *
 * The hierarchical sequence description contains the following elements:
 *  - **Blocks**: A list of event IDs.
 *  - **Events**: Hardware events (dependent on type: ADC, RF, gradient etc). These may
 *                refer to one or more basic shapes.
 *  - **Shapes**: Compressed list of samples. This can describe for example, RF amplitude
 *                or gradient waveforms.
 *
 * This hierarchy is maintained automatically by the class. The calling code simply
 * adds uncompressed hardware events.
 *
 * @see AddEvents(), WriteFiles()
 */
class OutputSequenceData
{
	/**
	 * @brief Compressed shape object
	 *
	 * This class stores an arbitrary shape in a compressed format where the
	 * derivative is encoded such that duplicate values are not repeated.
	 */
	struct CompressedShape
	{
	public:
		/**
		 * @brief Compare two compressed shapes.
		 *
		 * This function is necessary to ensure duplicate shapes are only stored once.
		 */
		bool operator==(const CompressedShape &other) const {
			if (this->m_num_uncompressed_samples!=other.m_num_uncompressed_samples) return false;
			if (this->m_samples.size()!=other.m_samples.size()) return false;
			for (int i=0; i<this->m_samples.size(); i++) {
				if (this->m_samples[i]!=other.m_samples[i]) return false;
			}
			return true;
		}

		int m_num_uncompressed_samples;     /**< @brief Number of samples of the uncompressed waveforms */
		std::vector<double> m_samples;      /**< @brief The compressed waveform samples */
	};

 public:

	/**
	 * @brief Default constructor
	 */
	OutputSequenceData           () : m_duration(0.0) {
		SetRotationMatrix(0.0,0.0,0.0);
	};

	/**
	 * @brief Default destructor
	 */
	~OutputSequenceData          () {};

	/**
	 * @brief Add events to a new sequence block
	 *
	 * Creates a new block based on the vector of event pointers. The event
	 * data is copied by this function, so pointers can safely be deleted afterwards.
	 */
	void AddEvents(std::vector<Event*> &events, double duration=0.0);

	/**
	 * @brief Set rotation matrix
	 *
	 * Calculate the rotation matrix from the axis/angle representation used
	 * internally.
	 *
	 * @param  alpha   Rotation angle
	 * @param  theta   Inclination from z-axis
	 * @param  phi     Azimutal phase from x-axis
	 */
	void SetRotationMatrix(double alpha, double theta, double phi);

	/**
	 * @brief Set custom scan definitions
	 */
	void SetDefinitions(std::map<std::string,std::string> &defs) {m_definitions=defs; };

	/**
	 * @brief Write the sequence data to file
	 */
	void WriteFiles(const std::string &outDir, const std::string &outFile);

 private:
	/**
	 * @brief Search library for match
	 */
	template<typename T> int SearchLibrary(std::vector<T> &library, T &obj);

	/**
	 * @brief Compress a shape
	 */
	void CompressShape(std::vector<double> &shape, CompressedShape *out);


	double  m_rot_matrix[3][3];     /**< @brief Rotation matrix */
	double  m_duration;             /**< @brief Total duration of sequence blocks */
	double  m_max_block_duration; 	/**< @brief Maximum duration of a single block */

	std::map<std::string,std::string> m_definitions; /**< @brief Custom definitions for user-specific purposes to be output in file */

	std::vector<SeqBlock>         m_blocks;          /**< @brief List of sequence blocks */

	std::vector<RFEvent>          m_rf_library;      /**< @brief Library of RF events */
	std::vector<GradEvent>        m_grad_library;    /**< @brief Library of gradient events */
	std::vector<ADCEvent>         m_adc_library;     /**< @brief Library of ADC events */

	std::vector<CompressedShape>  m_shape_library;   /**< @brief Library of compressed shapes (referenced by events) */

};

#endif /*OUTPUTSEQUENCEDATA_H_*/
