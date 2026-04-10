#ifndef BMNPADGENERATOR_H
#define BMNPADGENERATOR_H

#include <cstdio>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
// Boost
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// ROOT
#include <TCanvas.h>
#include <TClass.h>
#include <TDirectory.h>
#include <TH2F.h>
#include <TH3S.h>
// BmnRoot
#include "BmnPadBranch.h"

#include <BmnHistManager.h>

using std::string;
namespace pt = boost::property_tree;

/**
 * \class BmnPadGenerator
 * \brief Generates pad tree from the scheme
 */
class BmnPadGenerator
{
  public:
    BmnPadGenerator()
        : _HM(std::make_unique<BmnHistManager>())
        , _pad(nullptr)
    {}

    virtual ~BmnPadGenerator()
    {
        if (_pad)
            delete _pad;
    }
    /**
     * @brief Load pad tree from json config
     * @param FileName  path to json config
     *  */
    void LoadPTFrom(string FileName);
    /**
     * @brief Generate pad branch tree from the boost property tree
     * @param PropTree boost property tree
     * @return pointer to the BmnPadBranch instance - start of the branch tree
     */
    BmnPadBranch* GeneratePadBranch(pt::ptree& PropTree);
    PadInfo* GeneratePadNode(pt::ptree& PropTree);
    /**
     * @brief Create pad structure in the canvas from the pad tree
     * and associate pad branch instances with the pad pointers
     * @param br  pad tree
     * @param pad  canvas or any other TVirtualPad instance
     */
    static void PadTree2Canvas(BmnPadBranch* br, TVirtualPad* pad);

    BmnPadBranch* GetPadBranch() { return _pad; }

  private:
    std::unique_ptr<BmnHistManager> _HM;   //!
    BmnPadBranch* _pad;                    //!
    pt::ptree _TreeScheme;                 //!
};

#endif /* BMNPADGENERATOR_H */
