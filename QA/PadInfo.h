#ifndef PADINFO_H
#define PADINFO_H

#include <TH1.h>
#include <TLegend.h>
#include <TVirtualPad.h>
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr;
using std::string;
using std::vector;

/**
 * \class PadInfo
 * \brief Storage for pad content and it's options
 */
class PadInfo
{
  public:
    PadInfo()
        : temp(nullptr)
        , current(nullptr)
        , ref(nullptr)
        , legend(nullptr)
        , padPtr(nullptr)
        , logx(false)
        , logy(false)
        , logz(false)
        , normalize(true)
    {}

    PadInfo(TH1* c)
        : temp(nullptr)
        , current(c)
        , ref(nullptr)
        , legend(nullptr)
        , padPtr(nullptr)
        , logx(false)
        , logy(false)
        , logz(false)
        , normalize(true)
    {}

    PadInfo(TH1* c, vector<TH1*>&& a, vector<TString>&& ao, TLegend* leg = nullptr)
        : temp(nullptr)
        , current(c)
        , ref(nullptr)
        , aux(a)
        , auxOpt(ao)
        , legend(leg)
        , padPtr(nullptr)
        , logx(false)
        , logy(false)
        , logz(false)
        , normalize(true)
    {}

    virtual ~PadInfo()
    {
        printf("~PadInfo() \n");
        if (temp)
            delete temp;
        if (current)
            delete current;
        if (ref)
            delete ref;
        for (auto& el : aux)
            if (el)
                delete el;
        if (legend)
            delete legend;
    }
    void Reset();
    TH1* temp;
    //    shared_ptr<TH1> current;
    TH1* current;
    TH1* ref;
    vector<TH1*> aux;
    vector<TString> auxOpt;
    TLegend* legend;
    TVirtualPad* padPtr;
    bool logx;
    bool logy;
    bool logz;
    bool normalize;
    string opt;
    string name;
    string variable;
    string selection;

    //    TClass* classPtr;

  private:
    ClassDef(PadInfo, 2)
};

#endif /* PADINFO_H */
