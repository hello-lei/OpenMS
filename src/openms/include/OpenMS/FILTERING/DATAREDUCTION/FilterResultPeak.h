// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2013.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Lars Nilse $
// $Authors: Lars Nilse $
// --------------------------------------------------------------------------

#ifndef OPENMS_FILTERING_DATAREDUCTION_FILTERRESULTPEAK_H
#define OPENMS_FILTERING_DATAREDUCTION_FILTERRESULTPEAK_H

#include <OpenMS/KERNEL/StandardTypes.h>
#include <OpenMS/FILTERING/DATAREDUCTION/FilterResultRaw.h>
#include <OpenMS/FILTERING/DATAREDUCTION/FilterResultPeak.h>

#include <vector>
#include <algorithm>
#include <iostream>

namespace OpenMS
{
    /**
     * @brief data structure storing a single peak that passed all filters
     * 
     * Each peak filter result corresponds to a successful search for a particular
     * peak pattern in the centroided data. The actual m/z shifts seen in the filter
     * result might differ from the theoretical shifts listed in the peak pattern.
     * 
     * @see PeakPattern
     */
    class OPENMS_DLLAPI FilterResultPeak
    {
        /**
         * @brief position of the peak
         */
        double mz_;
        double rt_;
 
        /**
         * @brief m/z shifts at which peaks corresponding to a pattern were found
         */
        std::vector<double> mz_shifts_;

        /**
         * @brief peak intensities at mz_ + mz_shifts_
         */
        std::vector<double> intensities_;

        /**
         * @brief (optional) raw data points corresponding to the peak
         */
        std::vector<FilterResultRaw> raw_data_points_;
 
        public:
        /**
         * @brief constructor
         */
        FilterResultPeak(double mz, double rt, std::vector<double> mz_shifts, std::vector<double> intensities, std::vector<FilterResultRaw> rawDataPoints);

         /**
         * @brief returns m/z of the peak
         */
         double getMz();
         
         /**
         * @brief returns RT of the peak
         */
         double getRt();
         
         /**
         * @brief returns m/z shift at position i
         */
        double getMzShiftAt(int i);

        /**
         * @brief returns m/z shifts
         */
        std::vector<double> getmz_shifts();

        /**
         * @brief returns intensity at position i
         */
        double getIntensityAt(int i);
        
        /**
         * @brief returns intensities
         */
        std::vector<double> getIntensities();
        
        /**
         * @brief returns the numer of raw data points belonging to the peak
         */
         int size();
         
         /**
         * @brief returns a single raw data point belonging to the peak
         */
         FilterResultRaw getFilterResultRaw(int i);
  };
  
}

#endif /* FILTERRESULTPEAK_H_ */
