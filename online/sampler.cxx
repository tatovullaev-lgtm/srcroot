#include <FairRootManager.h>
#include <RootSerializer.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <fairmq/Device.h>
#include <fairmq/runDevice.h>
#include <string>
#include <thread>

namespace bpo = boost::program_options;

namespace bmn::online
{
struct Sampler : fair::mq::Device
{
  protected:
    static constexpr auto OUTPUT_CHANNEL_NAME = "out-channel";

    void Init() override
    {
        fEventsFilePath = fConfig->GetValue<std::string>("file-path");
        fEventDispatchDelay = fConfig->GetValue<UShort_t>("dispatch-delay");
        fCurrentEvent = fConfig->GetValue<size_t>("start-event");
    }

    void InitTask() override
    {
        fEventFile = std::unique_ptr<TFile>(TFile::Open(fEventsFilePath, "read"));
        fEventData = std::unique_ptr<TTree>(fEventFile->Get<TTree>("bmndata"));
        fNEvents = fEventData->GetEntries();

        LOG(info) << "Number of events in the file: " << fNEvents;
    }

    Bool_t ConditionalRun() override
    {
        LOG(info) << "Event #" << fCurrentEvent;

        if (fCurrentEvent > fNEvents) {
            return kFALSE;
        }

        std::unique_ptr<TTree> eventData(fEventData->CopyTree("", "", 1, fCurrentEvent));

        auto message = NewMessage();
        RootSerializer().Serialize(*message, eventData);

        if (Send(message, OUTPUT_CHANNEL_NAME) < 0) {
            LOG(error) << "Sending message failed!";
            return kFALSE;
        }

        fCurrentEvent++;

        std::this_thread::sleep_for(std::chrono::milliseconds(fEventDispatchDelay));

        return kTRUE;
    }

  private:
    TString fEventsFilePath = "";
    UShort_t fEventDispatchDelay = 1000;
    size_t fCurrentEvent = 0;

    std::unique_ptr<TFile> fEventFile;
    std::unique_ptr<TTree> fEventData;
    size_t fNEvents = 0;
};
}   // namespace bmn::online

void addCustomOptions(bpo::options_description& options)
{
    auto op = options.add_options();

    op("file-path", bpo::value<std::string>()->required(), "Path to the event file");
    op("dispatch-delay", bpo::value<UShort_t>()->default_value(1000),
       "Delay before dispatching an event in milliseconds");
    op("start-event", bpo::value<size_t>()->default_value(0), "Start event number");
}

std::unique_ptr<fair::mq::Device> getDevice(fair::mq::ProgOptions&)
{
    return std::make_unique<bmn::online::Sampler>();
}
