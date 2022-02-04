/** @file Event.h
 *  @brief Implementation of JEMRIS sequence Event for hardware execution.
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2022  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#ifndef EVENT_H_
#define EVENT_H_

#include "Declarations.h"
#include <vector>

/**
 * @brief Base class for hardware events.
 */
class Event
{
public:
	/**
	 * @brief Default constructor
	 */
	Event(){};

	/**
	 * @brief Default destructor
	 */
	virtual ~Event(){};
	/**
	 * @brief Compare two hardware events.
	 *
	 * This function *must* be implemented by subclasses to ensure duplicate
	 * events can be detected.
	 */
	virtual bool operator==(const Event &other) const { return false; };

};

/**
 * @brief Delay hardware event
 *
 * Represents a sequence delay, when no events are occurring.
 */
class DelayEvent : public Event
{
public:
	/**
	 * @brief Compare two delay events.
	 * @see Event::operator==()
	 */
	bool operator==(const DelayEvent &other) const {
	    return (this->m_delay==other.m_delay);
	}
public:
	double m_delay;     /**< @brief delay (us) */
};

/**
 * @brief RF hardware event
 *
 * Stores information necessary to execute an RF pulse on hardware:
 *  - Amplitude
 *  - Frequency and phase offsets
 *  - Waveform shape
 */
class RFEvent : public Event
{
public:
	/**
	 * @brief Compare two RF events.
	 * @see Event::operator==()
	 */
	bool operator==(const RFEvent &other) const {
	    return (this->m_amplitude==other.m_amplitude && this->m_mag_shape==other.m_mag_shape && this->m_phase_shape==other.m_phase_shape && this->m_delay==other.m_delay &&
	    		this->m_freq_offset==other.m_freq_offset && this->m_phase_offset==other.m_phase_offset);
	}
public:
	double m_amplitude;                  /**< @brief Amplitude (rad/ms) */
	int m_mag_shape;                     /**< @brief ID of magnitude shape (set by OutputSequenceData) */
	int m_phase_shape;                   /**< @brief ID of phase shape (set by OutputSequenceData) */
	int m_delay;               		 	 /**< @brief Initial delay (us) */
	double m_freq_offset;                /**< @brief Frequency offset (rad/ms) */
	double m_phase_offset;               /**< @brief Phase offset (rad) */
	std::vector<double> m_magnitude;     /**< @brief Magnitude waveform samples */
	std::vector<double> m_phase;         /**< @brief Phase waveform samples */
};

/**
 * @brief Gradient hardware event.
 *
 * Stores information necessary to execute an gradient event on hardware.
 * This class can represent two types of gradients:
 *  - *Trapezoids*: Stores ramp up/down and duration information.
 *  - *Arbitrary gradients*: Stores gradient waveform samples
 */
class GradEvent : public Event
{
public:
	/**
	 * @brief Compare two gradient events.
	 * @see Event::operator==()
	 */
	bool operator==(const GradEvent &other) const {
		bool ok;
		if (this->m_shape>=0)    // Arbitrary gradient
			ok = (this->m_shape==other.m_shape);
		else                     // Trapezoidal gradient
			ok = (this->m_ramp_up_time==other.m_ramp_up_time && this->m_flat_time==other.m_flat_time && this->m_ramp_down_time==other.m_ramp_down_time);
		return (ok && this->m_amplitude==other.m_amplitude && this->m_channel==other.m_channel && this->m_delay==other.m_delay);
	}
public:
	int  m_channel;                      /**< @brief Channel (0, 1 or 2 for X,Y,Z) */
	double m_amplitude;                  /**< @brief Amplitude (rad/ms/mm) */
	int m_delay;               			 /**< @brief Initial delay (us) */

	// Trapezoid:
	long m_ramp_up_time;                 /**< @brief Ramp up time (us) */
	long m_flat_time;                    /**< @brief Flat-top time (us) */
	long m_ramp_down_time;               /**< @brief Ramp down time (us) */

	// Arbitrary:
	int m_shape;                         /**< @brief ID of gradient waveform (0 if trapezoid)*/
	std::vector<double> m_samples;       /**< @brief samples of gradient waveform */
};


/**
 * @brief ADC hardware event.
 *
 * Stores information necessary to execute an ADC event:
 *  - Number of samples
 *  - Dwell time
 *  - Delay
 *  - Frequency and phase offset information (to shift FOV)
 */
class ADCEvent : public Event
{
public:
	/**
	 * @brief Compare ADC events.
	 * @see Event::operator==()
	 */
	bool operator==(const ADCEvent &other) const {
	    return (this->m_num_samples==other.m_num_samples && this->m_dwell_time==other.m_dwell_time && this->m_delay==other.m_delay &&
	    		this->m_freq_offset==other.m_freq_offset && this->m_phase_offset==other.m_phase_offset);
	}
public:
	int m_num_samples;          /**< @brief Number of samples */
	int m_dwell_time;           /**< @brief Dwell time (ns) */
	int m_delay;                /**< @brief Initial delay (us) */
	double m_freq_offset;       /**< @brief Frequency offset of receiver (rad/ms) */
	double m_phase_offset;      /**< @brief Phase offset of receiver (rad) */
};


#endif /*EVENT_H_*/
