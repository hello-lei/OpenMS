// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2017.
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

#include <OpenMS/TRANSFORMATIONS/FEATUREFINDER/FeatureFinderMultiplexAlgorithm.h>
#include <OpenMS/TRANSFORMATIONS/FEATUREFINDER/MultiplexDeltaMassesGenerator.h>

#include <OpenMS/CHEMISTRY/IsotopeDistribution.h>
#include <OpenMS/MATH/STATISTICS/StatisticFunctions.h>
#include <OpenMS/MATH/MISC/MathFunctions.h>
#include <OpenMS/CONCEPT/Constants.h>

#include <vector>
#include <numeric>
#include <fstream>
#include <algorithm>

using namespace std;

namespace OpenMS
{
  FeatureFinderMultiplexAlgorithm::FeatureFinderMultiplexAlgorithm() :
    DefaultParamHandler("FeatureFinderMultiplexAlgorithm")
  {
    // parameter section: algorithm
    defaults_.setValue("algorithm::labels", "[][Lys8,Arg10]", "Labels used for labelling the samples. If the sample is unlabelled (i.e. you want to detect only single peptide features) please leave this parameter empty. [...] specifies the labels for a single sample. For example\n\n[][Lys8,Arg10]        ... SILAC\n[][Lys4,Arg6][Lys8,Arg10]        ... triple-SILAC\n[Dimethyl0][Dimethyl6]        ... Dimethyl\n[Dimethyl0][Dimethyl4][Dimethyl8]        ... triple Dimethyl\n[ICPL0][ICPL4][ICPL6][ICPL10]        ... ICPL");
    defaults_.setValue("algorithm::charge", "1:4", "Range of charge states in the sample, i.e. min charge : max charge.");
    defaults_.setValue("algorithm::isotopes_per_peptide", "3:6", "Range of isotopes per peptide in the sample. For example 3:6, if isotopic peptide patterns in the sample consist of either three, four, five or six isotopic peaks. ", ListUtils::create<String>("advanced"));
    defaults_.setValue("algorithm::rt_typical", 40.0, "Typical retention time [s] over which a characteristic peptide elutes. (This is not an upper bound. Peptides that elute for longer will be reported.)");
    defaults_.setMinFloat("algorithm::rt_typical", 0.0);
    defaults_.setValue("algorithm::rt_band", 10.0, "RT band which is taken into considerations when filtering.TODO docu");
    defaults_.setMinFloat("algorithm::rt_band", 0.0);
    defaults_.setValue("algorithm::rt_min", 2.0, "Lower bound for the retention time [s]. (Any peptides seen for a shorter time period are not reported.)");
    defaults_.setMinFloat("algorithm::rt_min", 0.0);
    defaults_.setValue("algorithm::mz_tolerance", 6.0, "m/z tolerance for search of peak patterns.");
    defaults_.setMinFloat("algorithm::mz_tolerance", 0.0);
    defaults_.setValue("algorithm::mz_unit", "ppm", "Unit of the 'mz_tolerance' parameter.");
    defaults_.setValidStrings("algorithm::mz_unit", ListUtils::create<String>("Da,ppm"));
    defaults_.setValue("algorithm::intensity_cutoff", 1000.0, "Lower bound for the intensity of isotopic peaks.");
    defaults_.setMinFloat("algorithm::intensity_cutoff", 0.0);
    defaults_.setValue("algorithm::peptide_similarity", 0.5, "Two peptides in a multiplet are expected to have the same isotopic pattern. This parameter is a lower bound on their similarity.");
    defaults_.setMinFloat("algorithm::peptide_similarity", -1.0);
    defaults_.setMaxFloat("algorithm::peptide_similarity", 1.0);
    defaults_.setValue("algorithm::averagine_similarity", 0.4, "The isotopic pattern of a peptide should resemble the averagine model at this m/z position. This parameter is a lower bound on similarity between measured isotopic pattern and the averagine model.");
    defaults_.setMinFloat("algorithm::averagine_similarity", -1.0);
    defaults_.setMaxFloat("algorithm::averagine_similarity", 1.0);
    defaults_.setValue("averagine_similarity_scaling", 0.75, "Let x denote this scaling factor, and p the averagine similarity parameter. For the detection of single peptides, the averagine parameter p is replaced by p' = p + x(1-p), i.e. x = 0 -> p' = p and x = 1 -> p' = 1. (For knock_out = true, peptide doublets and singlets are detected simulataneously. For singlets, the peptide similarity filter is irreleavant. In order to compensate for this 'missing filter', the averagine parameter p is replaced by the more restrictive p' when searching for singlets.)", ListUtils::create<String>("advanced"));
    defaults_.setMinFloat("algorithm::averagine_similarity_scaling", 0.0);
    defaults_.setMaxFloat("algorithm::averagine_similarity_scaling", 1.0);
    defaults_.setValue("algorithm::missed_cleavages", 0, "Maximum number of missed cleavages due to incomplete digestion. (Only relevant if enzymatic cutting site coincides with labelling site. For example, Arg/Lys in the case of trypsin digestion and SILAC labelling.)");
    defaults_.setMinInt("algorithm::missed_cleavages", 0);
    defaults_.setValue("algorithm::knock_out", "false", "Is it likely that knock-outs are present? (Supported for doublex, triplex and quadruplex experiments only.)", ListUtils::create<String>("advanced"));
    defaults_.setValidStrings("algorithm::knock_out", ListUtils::create<String>("true,false"));
    defaults_.setValue("algorithm::averagine_type","peptide","The type of averagine to use, currently RNA, DNA or peptide", ListUtils::create<String>("advanced"));
    defaults_.setValidStrings("algorithm::averagine_type", ListUtils::create<String>("peptide,RNA,DNA"));

    // parameter section: labels
    MultiplexDeltaMassesGenerator generator;
    Param p = generator.getParameters();
    for (Param::ParamIterator it = p.begin(); it != p.end(); ++it)
    {
      defaults_.setValue(("labels::" + it->name), it->value, it->description, ListUtils::create<String>("advanced"));
      defaults_.setMinFloat(it->name, 0.0);
    }

  }

  void FeatureFinderMultiplexAlgorithm::run(bool centroided)
  {
  }
}
