#include "PadInfo.h"

void PadInfo::Reset() {
    if (current) current->Reset();
    for (TH1* h : aux)
        h->Reset();
}
