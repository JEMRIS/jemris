/** @file CoilArray.h
 *  @brief Implementation of JEMRIS CoilArray
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

#ifndef COILARRAY_H_
#define COILARRAY_H_

#include "XMLIO.h"

#include "Signal.h"
#include "CoilPrototypeFactory.h"

class Coil;

/**
 *  @brief Coil configuration and sensitivities
 */
class CoilArray  {

 public:

    /**
     * @brief Default constructor
     *
     * Empty constructor will create a single channel ideal coil with flat sensitivity map.
     */
    CoilArray();

    /**
     * @brief Initialize the array and its elements
     *
     * @param uri Configuration file. (Assigned to Simulation in simu.xml)
     */
    void Initialize  (string uri);

    /**
     * @brief Populate coil array
     *        Run over coil array configuration tree and Populate the array
     */
    unsigned int  Populate ();

    /**
     * @brief Default destructor
     */
    virtual ~CoilArray     ();

    /**
     * @brief Clone a coil
     *
     * @param  ptr  Pointer to myself
     * @param  node DOMNode with coil configuation
     *
     * @return      Created coil
     */
    static unsigned int CreateCoil (void* ptr,DOMNode* node);

    /**
     * @brief Run over XML tree and return nodes
     *
     * @return All nodes in the tree.
     *
     */
    DOMNode*        RunTree (DOMNode* node, void* ptr, unsigned int (*fun) (void*, DOMNode*));


    /**
     * @brief Get the number of channels
     *
     * @return The number of my channels
     */
    inline unsigned int GetSize   () { return m_coils.size(); };

    /**
     * @brief Create the signal structures of given size for all my coils.
     *
     * @param lADCs Number of ADC events.
     */
    void InitializeSignals (long lADCs);

    /**
     * @brief Recieve a signal from the World singleton with my coils for a given event.
     *
     * @param lADC position of this ADC event within all ADC events.
     */
    void Receive           (long lADC);

    /**
     * @brief Dump all signals
     * Dump the signals from all coils to discrete files.
     */
    void DumpSignals       (string tmp_prefix = "", bool normalize = true);

    /**
     * @brief Dump all sensitivities
     * Dump the sensitivities from all coils to discrete files.
     */
    void DumpSensMaps      (bool verbose = false);


    /**
     * @brief Set signal file-name prefix
     * Set the prefix string for signal binary filenames.
     * @param val the prefix
     */
    inline void SetSignalPrefix      (string val) {m_signal_prefix = val;};

    /**
     * @brief Get a particular coil
     *
     * @param  channel The number of the particular channel.
     * @return The requested coil.
     */
    Coil* GetCoil          (unsigned channel);

    /**
     * @brief Prepare my coils
     *
     * @param  mode Prepare mode
     *
     * @return      Success
     */
    bool Prepare (PrepareMode mode);

    /**
     * @brief
     */
    void setMode (unsigned short mode) { m_mode = mode; }

    /**
     * @brief reads restart signal.
     */
    int ReadRestartSignal();

 private:

    vector<Coil*>         m_coils;         /**< @brief My coils         */
    double                m_radius;        /**< @brief My radius        */
    unsigned short        m_mode;          /**< @brief My mode (RX/TX)  */
    string	          m_signal_prefix; /**< @brief prefix string to signal binary filenames */

    CoilPrototypeFactory* m_cpf;           /**< @brief Coil factory    */
    DOMDocument*          m_dom_doc;       /**< @brief DOM document containing configuration */
    Parameters*           m_params;        /**< @brief My parameters   */
    XMLIO*                m_xio;           /**< @brief My XML IO module   */


    /**
     * @brief Dump to binary file.
     *        Dump sensitivities or signals coils to discrete files.
     *
     * @param what What should be dumped.
     */
    void Dump              (string what);
};

#endif /*COILARRAY_H_*/
