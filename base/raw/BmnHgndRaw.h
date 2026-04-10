#ifndef BMNHGNDRAW_H
#define BMNHGNDRAW_H

#include <array>
#include <cstdint>
#include <vector>

namespace BmnHgndRaw
{

namespace Block
{

// Header
static constexpr uint32_t HEADER_MASK = 0xF000;
static constexpr uint8_t EVENT_HEADER = 0xA;

inline bool is_word_event_header(uint16_t word)
{
    return ((word & HEADER_MASK) >> 12) == EVENT_HEADER;
}

// Version
enum EventPacketVersion
{
    ERR,
    V1,
    V2
};

inline EventPacketVersion get_event_packet_version(uint16_t word)
{
    uint8_t version = uint8_t(word & 0x000F);
    if (version == 1)
        return EventPacketVersion::V1;
    if (version == 2)
        return EventPacketVersion::V2;
    return EventPacketVersion::ERR;
}

// Length
static constexpr uint16_t EVENT_TYPE_1_PACKET_LENGTH = 8;
static constexpr uint16_t EVENT_TYPE_2_PACKET_LENGTH = 18;

inline uint16_t get_block_size(EventPacketVersion packet_version = ERR)
{
    if (packet_version == Block::EventPacketVersion::V1) {
        return Block::EVENT_TYPE_1_PACKET_LENGTH;
    }
    if (packet_version == Block::EventPacketVersion::V2) {
        return Block::EVENT_TYPE_2_PACKET_LENGTH;
    }
    return 0;
}

}   // namespace Block

struct FIFO_block
{
    std::vector<uint16_t> p_words;
    FIFO_block() = default;
    FIFO_block(std::vector<uint16_t> words)
        : p_words(words)
    {}
    virtual ~FIFO_block() = default;

    virtual bool validate() = 0;
    virtual void reset() = 0;
};

struct Event : public FIFO_block
{
    uint64_t timestamp;
    uint8_t channel_number;
    uint16_t channel_number_global;

    Event()
        : FIFO_block() {};
    Event(std::vector<uint16_t> words)
        : FIFO_block(words)
    {}
    ~Event() override {}
};

struct Event_type_1 final : public Event
{
    uint8_t TDC_rise_time;
    uint8_t TDC_falling_time;
    uint16_t pulse_length;

    bool validate() override
    {
        uint16_t trailer = p_words[7];
        if (trailer != 0xABBA)
            return false;

        if ((p_words[5] & 0x80) != 0)
            return false;
        if ((p_words[5] & 0x8000) != 0)
            return false;

        return true;
    }

    Event_type_1(std::vector<uint16_t> words)
        : Event(words)
    {
        channel_number = (p_words[0] & 0x0FF0) >> 4;
        // Timestamp
        timestamp = 0;
        for (size_t i = 0; i < 4; i++)
            timestamp += uint64_t(p_words[i + 1]) << 16 * i;
        TDC_rise_time = p_words[5] & 0x007F;
        TDC_falling_time = (p_words[5] & 0x7F00) >> 8;
        pulse_length = p_words[6];
    }

    Event_type_1()
        : Event()
    {}
    ~Event_type_1() override {}
    void reset() override { *this = Event_type_1(); }
};

struct Event_type_2 final : public Event
{
    uint8_t TDC_rise_time;
    uint8_t TDC_falling_time;
    uint8_t raw_tdc_rise_0;
    uint8_t raw_tdc_rise_1;
    uint8_t raw_tdc_rise_2;
    uint8_t raw_tdc_rise_3;
    uint8_t TDC_time_rise_0;
    uint8_t TDC_time_rise_1;
    uint8_t TDC_time_rise_2;
    uint8_t TDC_time_rise_3;
    uint8_t raw_tdc_fall_0;
    uint8_t raw_tdc_fall_1;
    uint8_t raw_tdc_fall_2;
    uint8_t raw_tdc_fall_3;
    uint8_t TDC_time_fall_0;
    uint8_t TDC_time_fall_1;
    uint8_t TDC_time_fall_2;
    uint8_t TDC_time_fall_3;
    uint8_t post_gen_rise;
    uint8_t post_gen_fall;
    uint16_t pulse_length;

    bool validate() override
    {
        uint16_t trailer = (p_words[15] & 0xFF00) >> 8;
        if (trailer != 0xB0)
            return false;

        if ((p_words[13] & 0x80) != 0)
            return false;
        if ((p_words[13] & 0x8000) != 0)
            return false;

        return true;
    }

    Event_type_2(std::vector<uint16_t> words)
        : Event(words)
    {
        channel_number = (p_words[0] & 0x0FF0) >> 4;
        // Timestamp
        timestamp = 0;
        for (int i = 0; i < 4; i++)
            timestamp += uint64_t(p_words[i + 1]) << 16 * i;
        raw_tdc_rise_0 = p_words[5] & 0x00FF;
        raw_tdc_rise_1 = (p_words[5] & 0xFF00) >> 8;
        raw_tdc_rise_2 = p_words[6] & 0x00FF;
        raw_tdc_rise_3 = (p_words[6] & 0xFF00) >> 8;
        TDC_time_rise_0 = p_words[7] & 0x00FF;
        TDC_time_rise_1 = (p_words[7] & 0xFF00) >> 8;
        TDC_time_rise_2 = p_words[8] & 0x00FF;
        TDC_time_rise_3 = (p_words[8] & 0xFF00) >> 8;
        raw_tdc_fall_0 = p_words[9] & 0x00FF;
        raw_tdc_fall_1 = (p_words[9] & 0xFF00) >> 8;
        raw_tdc_fall_2 = p_words[10] & 0x00FF;
        raw_tdc_fall_3 = (p_words[10] & 0xFF00) >> 8;
        TDC_time_fall_0 = p_words[11] & 0x00FF;
        TDC_time_fall_1 = (p_words[11] & 0xFF00) >> 8;
        TDC_time_fall_2 = p_words[12] & 0x00FF;
        TDC_time_fall_3 = (p_words[12] & 0xFF00) >> 8;
        TDC_rise_time = p_words[13] & 0x007F;
        TDC_falling_time = (p_words[13] & 0x7F00) >> 8;
        pulse_length = p_words[14];
        post_gen_rise = p_words[15] & 0x000F;
        post_gen_rise = (p_words[15] & 0x00F0) >> 4;
        channel_number_global = p_words[16];
    }

    Event_type_2()
        : Event()
    {}
    ~Event_type_2() override {}
    void reset() override { *this = Event_type_2(); }
};

}   // namespace BmnHgndRaw

#endif