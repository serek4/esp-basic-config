#pragma once

//* ARDUINOJSON_USE_DOUBLE 1 is default from version 6.19
#define ARDUINOJSON_USE_DOUBLE 0
//* ARDUINOJSON_USE_LONG_LONG 1 is default from version 6.19 for 32-bits & 64-bit platforms
#ifdef ARDUINO_ARCH_ESP32    // time_t = int32_t(long)
#define ARDUINOJSON_USE_LONG_LONG 0
#elif defined(ARDUINO_ARCH_ESP8266)    // time_t = int64_t(signed long long)
#define ARDUINOJSON_USE_LONG_LONG 1
#endif

#include <ArduinoJson.h>
#include <esp-basic-fs.hpp>
#include <vector>

// #define BASIC_CONFIG_DEBUG
// debug printing macros
// clang-format off
#ifdef BASIC_CONFIG_DEBUG
#define DEBUG_PRINTER Serial
#define BASIC_CONFIG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASIC_CONFIG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#define BASIC_CONFIG_PRINTF(...) { DEBUG_PRINTER.printf(__VA_ARGS__); }
#else
#define BASIC_CONFIG_PRINT(...) {}
#define BASIC_CONFIG_PRINTLN(...) {}
#define BASIC_CONFIG_PRINTF(...) {}
#endif
// clang-format on

//* calculate Arduino json document size at https://arduinojson.org/v6/assistant/

#define DEFAULT_FILE_NAME "config"

class BasicConfig {
  public:
	typedef std::function<void(JsonObject& config)> SerializeHandler;
	typedef std::function<void(JsonObject& config)> DeserializeHandler;

	BasicConfig(size_t capacity);
	BasicConfig(size_t capacity, const char* configFileName);

	void addLogger(void (*logger)(String logLevel, String msg));
	void setup();
	void serialize(const SerializeHandler& serializeHandler);
	void deserialize(const DeserializeHandler& deserializeHandler);
	void load();
	void save();
	String print(bool pretty = false);

  private:
	size_t _JsonConfigSize;
	String _configFileName;
	std::vector<SerializeHandler> _serializeHandlers;
	std::vector<DeserializeHandler> _deserializeHandlers;
	void (*_logger)(String logLevel, String msg);

	String _serialize(bool pretty = false);
	bool _deserialize(String& jsonConfig);
	bool _saveToFile(String filename, bool pretty = true);
	bool _readFromFile(String filename);
};
