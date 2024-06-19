#pragma once

//* ARDUINOJSON_USE_DOUBLE 1 is default from version 6.19
#define ARDUINOJSON_USE_DOUBLE 0
//* ARDUINOJSON_USE_LONG_LONG 1 is default from version 6.19 for 32-bits & 64-bit platforms
#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>
#include <esp-basic-fs.hpp>
#include <list>

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

#define DEFAULT_FILE_NAME "config"

class BasicConfig {
  public:
	typedef std::function<void(JsonObject config)> SerializeHandler;
	typedef std::function<void(JsonObject config)> DeserializeHandler;

	BasicConfig();
	BasicConfig(const char* configFileName);

	void addLogger(void (*logger)(String logLevel, String msg));
	void setup();
	void serialize(const SerializeHandler& serializeHandler);
	void deserialize(const DeserializeHandler& deserializeHandler);
	void load();
	void save();
	String print(bool pretty = false);

  private:
	String _configFileName;
	std::list<SerializeHandler> _serializeHandlers;
	std::list<DeserializeHandler> _deserializeHandlers;
	void (*_logger)(String logLevel, String msg);

	String _serialize(bool pretty = false);
	bool _deserialize(String& jsonConfig);
	bool _saveToFile(String filename, bool pretty = true);
	bool _readFromFile(String filename);
};
