#ifndef BMN_ALIASES
#define BMN_ALIASES

typedef std::pair<uint32_t, uint16_t> PlMapKey;      // board placement key (serial, slot)
typedef std::pair<std::string, uint16_t> T0MapKey;   // raw T0 map key (trig name, iModule)
typedef std::map<T0MapKey, uint16_t> T0ChMap;        // (trig name, iModule) -> iNumber in array
typedef std::unordered_map<std::string, uint16_t> NameChMap;
typedef std::unordered_map<uint16_t, std::string> ChNameMap;
typedef std::unordered_map<std::string, std::pair<uint32_t, uint16_t>> Name2SerCh;
typedef std::map<std::pair<uint32_t, uint16_t>, std::string> SerCh2Name;
typedef std::unordered_map<std::string, uint64_t> MapString2Cnt;

#endif
