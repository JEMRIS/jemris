/** @file BiotSavartLoop.cpp
 *  @brief Implementation of JEMRIS BiotSavartLoop
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
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

#include "config.h"

#include "BiotSavartLoop.h"

#ifdef HAVE_BOOST
    #include <boost/math/special_functions/ellint_2.hpp>
    #include <boost/math/special_functions/ellint_1.hpp>
#endif

bool BiotSavartLoop::Prepare (const PrepareMode mode) {

    bool success = true;

    m_mask   = 0.0;
    m_radius = 100.0;

    ATTRIBUTE("Radius" , m_radius);
    ATTRIBUTE("Mask" , m_mask);
    success   = Coil::Prepare(mode);
    GridMap();

    return success;

}

double BiotSavartLoop::GetSensitivity(const double* position) {

    double a     = m_radius;
    double px = position[XC]-m_position[XC];
    double py = position[YC]-m_position[YC];
    double pz = position[ZC]-m_position[ZC];

    double ppx, ppy, ppz;

    //shift half mesh size
    px += 0.5 * m_extent / m_points;
    py += 0.5 * m_extent / m_points;
    pz += 0.5 * m_extent / m_points;

    //azimuth rotation
    ppy = px*cos(m_azimuth) + py*sin(m_azimuth);
    ppx = py*cos(m_azimuth) - px*sin(m_azimuth);
    px = ppx; py = ppy;

    //polar rotation: axis of rotation is the (new) x-axis
    ppz = pz*cos(m_polar) - py*sin(m_polar);
    ppy = py*cos(m_polar) + pz*sin(m_polar);
    py = ppy; pz = ppz;

    // distance between coil-center and position
    double dist = sqrt( abs(pow(px,2)+pow(py,2)+pow(pz,2)) );

    //return zero on torus with radius m_mask
    if (pow(a-sqrt(pow(px,2)+pow(py,2)),2)+pow(pz,2) < pow(m_mask,2) ) return 0.0;

    // angle coil-normal and position vector
    double angle = acos (pz/dist);

    // Bio-Savart closed form solution for a current loop
    // online: http://www.netdenizen.com/emagnettest/offaxis/?offaxisloop
    double r     = dist * sin (angle);	// distance off axis
    double x     = dist * cos (angle);	// distance on axis

    double alpha = r/a;
    double beta  = x/a;
    double gamma = x/r;
    double Q     = pow  ((1.0+alpha),2) + pow (beta,2);

    double k     = sqrt(4*alpha/Q);
    k            = (isnan(k)?0.0:k);

    double Kk    = 1.0;
    double Ek    = 1.0;

    #ifdef HAVE_BOOST
    // Complete elliptical integrals
    Kk = boost::math::ellint_1(k);
    Ek = boost::math::ellint_2(k);
    #endif

    //field parallel to coil normal vector
    double Bx    = (Ek * (1.0 - pow(alpha,2) - pow(beta,2)) / (Q-4.0*alpha) + Kk)         / (PI * sqrt(Q));
    //field orthogonal to coil normal vector
    double Br    = (Ek * (1.0 + pow(alpha,2) + pow(beta,2)) / (Q-4.0*alpha) - Kk) * gamma / (PI * sqrt(Q)) ;

    // return to cartesian coordinate system of the coil
    double Bz, By, phi;
    phi = atan2(py,px);
    Bz = Bx;
    Bx = Br * cos(phi);
    By = Br * sin(phi);

    // return to global coordinate system - polar rotation
    double BBx, BBy, BBz;
    BBz = Bz*cos(m_polar) + By*sin(m_polar);
    BBy = By*cos(m_polar) - Bz*sin(m_polar);
    Bz = BBz; By = BBy;

    // return to global coordinate system - azimuth rotation
    BBy = Bx*cos(m_azimuth) - By*sin(m_azimuth);
    BBx = By*cos(m_azimuth) + Bx*sin(m_azimuth);
    Bx = BBx; By = BBy;

    // Compute clockwise rotating field (equal to 1/2 of linear field)
    // Ref: Principles of Magentic Resonance Engineering, Z-P Liang and P. C. Lauterbur,
    //   section 3.2.2 pp. 71ff
    Bx = 0.5*Bx;
    By = 0.5*By;
    Bz = 0.5*Bz;

    // compute |B1|
    double B1 = sqrt(pow(Bx,2.0)+pow(By,2.0));

    // compute phase and store for later retrieval 
    m_biosavart_phase = atan2(By,Bx); 
    
    // check for numerical problems
    B1 = (isnan(B1)? 0.5:B1);
    
    // return amplitude of B1
    return B1;

}
