#include "DigiArrays.h"

#include <RootSerializer.h>
#include <TBuffer.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <fairmq/Device.h>
#include <fairmq/runDevice.h>
#include <string>

namespace bpo = boost::program_options;

namespace bmn::online
{
struct DecoderProxy : fair::mq::Device
{
  protected:
    static constexpr auto INPUT_CHANNEL_NAME = "in-channel";
    static constexpr auto OUTPUT_CHANNEL_NAME = "out-channel";

    void Init() override { OnData(INPUT_CHANNEL_NAME, &DecoderProxy::HandleData); }

    Bool_t HandleData(fair::mq::MessagePtr& inputMessage, Int_t)
    {
        LOG(info) << "Event Received. Message size: " << inputMessage->GetSize();

        auto buffer = std::make_unique<TBufferFile>(TBuffer::kRead);
        buffer->SetBuffer(inputMessage->GetData(), inputMessage->GetSize());
        auto eventData = std::unique_ptr<DigiArrays>(static_cast<DigiArrays*>(buffer->ReadObject(DigiArrays::Class())));

        auto tree = std::make_unique<TTree>();
        tree->Branch("SILICON", eventData->silicon);
        tree->Branch("GEM", eventData->gem);
        tree->Branch("CSC", eventData->csc);
        tree->Branch("TOF400", eventData->tof400);
        tree->Branch("TOF700", eventData->tof700);
        tree->Branch("TOF701", eventData->tof701);
        tree->Branch("ScWallDigi", eventData->scwall);
        tree->Branch("FHCalDigi", eventData->fhcal);
        tree->Branch("HodoDigi", eventData->hodo);
        tree->Branch("HdetDigi", eventData->ndet);
        tree->Branch("SiBT", eventData->sibt);
        tree->Branch("BC2AT", eventData->trigAr);
        tree->Fill();

        auto message = NewMessage();
        RootSerializer().Serialize(*message, tree);

        if (Send(message, OUTPUT_CHANNEL_NAME) < 0) {
            LOG(error) << "Sending message failed!";
            return kFALSE;
        }

        buffer.release();

        return kTRUE;
    }
};

}   // namespace bmn::online

void addCustomOptions(bpo::options_description& options) {}

std::unique_ptr<fair::mq::Device> getDevice(fair::mq::ProgOptions&)
{
    return std::make_unique<bmn::online::DecoderProxy>();
}
