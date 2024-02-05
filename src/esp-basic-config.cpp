#include "esp-basic-config.hpp"

BasicConfig::BasicConfig()
    : _configFileName(DEFAULT_FILE_NAME)
    , _logger(nullptr) {
}
BasicConfig::BasicConfig(const char* configFileName)
    : _configFileName(configFileName)
    , _logger(nullptr) {
}

void BasicConfig::addLogger(void (*logger)(String logLevel, String msg)) {
	_logger = logger;
}
void BasicConfig::setup() {
	if (!(filesystem._fsStarted)) {
		BASIC_FS_PRINTLN("file system not mounted yet, mounting");
		filesystem.setup();
	}
}
void BasicConfig::serialize(const SerializeHandler& serializeHandler) {
	_serializeHandlers.push_back(serializeHandler);
}
void BasicConfig::deserialize(const DeserializeHandler& deserializeHandler) {
	_deserializeHandlers.push_back(deserializeHandler);
}
String BasicConfig::print(bool pretty) {
	return _serialize(pretty);
}
void BasicConfig::load() {
	// read file config from file
	if (_readFromFile(_configFileName)) {
		if (_logger != nullptr) { (*_logger)("config", _configFileName + ".json loaded"); }
		// create/update backup file on success
		String configMd5 = filesystem.fileMd5(_configFileName + ".json");
		BASIC_CONFIG_PRINTF("config md5: %s\n", configMd5.c_str());
		String backupMd5 = filesystem.fileMd5(_configFileName + "-backup.json");
		BASIC_CONFIG_PRINTF("backup md5: %s\n", backupMd5.c_str());
		if (configMd5 != backupMd5) {
			BASIC_CONFIG_PRINTF("saving/updating %s\n", (_configFileName + "-backup.json").c_str());
			_saveToFile(_configFileName + "-backup");
			if (_logger != nullptr) { (*_logger)("config", _configFileName + "-backup.json updated"); }
		}
		return;
	}
	// read backup file on error
	if (_readFromFile(_configFileName + "-backup")) {
		if (_logger != nullptr) { (*_logger)("config", _configFileName + "-backup.json loaded"); }
		// overwrite config file with backup file on success
		_saveToFile(_configFileName);
		return;
	}
	if (_logger != nullptr) { (*_logger)("config", "loaded default config"); }
	// write new file with default config on backup error
	_saveToFile(_configFileName);
}
void BasicConfig::save() {
	// backup old config
	filesystem.renameFile(_configFileName + ".json", _configFileName + "-old.json");
	if (!_saveToFile(_configFileName)) {
		// restore old config on error
		BASIC_CONFIG_PRINTLN("config update failed restoring old file");
		filesystem.renameFile(_configFileName + "-old.json", _configFileName + ".json");
	}
	BASIC_CONFIG_PRINTLN("config updated");
}

String BasicConfig::_serialize(bool pretty) {
	String jsonConfig = "";
	JsonDocument doc;
	JsonObject config = doc.to<JsonObject>();
	for (const auto& handler : _serializeHandlers) handler(config);
	if (pretty) {
		serializeJsonPretty(doc, jsonConfig);
	} else {
		serializeJson(doc, jsonConfig);
	}
	return jsonConfig + "\n";
}
bool BasicConfig::_deserialize(String& jsonConfig) {
	JsonDocument doc;
	DeserializationError error = deserializeJson(doc, jsonConfig);
	if (error) {
		BASIC_CONFIG_PRINTF("deserializeJson() failed: %s\n", error.c_str());
		return false;
	}
	JsonObject config = doc.as<JsonObject>();
	for (const auto& handler : _deserializeHandlers) handler(config);
	return true;
}
bool BasicConfig::_saveToFile(String filename, bool pretty) {
	String jsonConfig = _serialize(pretty);
	File configFile;
	BASIC_CONFIG_PRINTF("saving config to: %s\n", (filename + ".json").c_str());
	return filesystem.writeFile(jsonConfig, filename + ".json", configFile);
}
bool BasicConfig::_readFromFile(String filename) {
	String jsonConfig = "";
	File configFile;
	BASIC_CONFIG_PRINTF("reading config from: %s\n", (filename + ".json").c_str());
	if (filesystem.readFile(filename + ".json", configFile) == BasicFS::read_ok) {
		jsonConfig = configFile.readString();
		configFile.close();
		if (_deserialize(jsonConfig)) {
			return true;
		}
		filesystem.renameFile(filename + ".json", filename + "-invalid.json");
		BASIC_CONFIG_PRINTF("invalid config renamed to %s\n",
		                    (filename + "-invalid.json").c_str());
		return false;
	}
	configFile.close();
	BASIC_CONFIG_PRINTF("%s not found\n", (filename + ".json").c_str());
	return false;
}
