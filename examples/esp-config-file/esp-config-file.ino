#include <esp-basic-config.h>

struct Config {
	uint8_t mode = 2;
	std::string OTAhost = "esp_test";
	std::string mqttBroker = "mqtt-broker.lan";
	uint16_t mqttBrokerPort = 1883;
} myConfig;

BasicConfig config("userConfig");

void setup() {
	Serial.begin(115200);
	Serial.println("");
	filesystem.setup(true);
	config.setup();
	config.serialize(serializeConfig);
	config.deserialize(deserializeConfig);
	config.load();
	Serial.printf("Loaded config: %s", config.print().c_str());
}

void loop() {
	delay(10);
}

void serializeConfig(JsonObject doc) {
	doc["mode"] = myConfig.mode;
	doc["OTA"]["host"] = myConfig.OTAhost;

	JsonObject MQTT = doc["MQTT"].to<JsonObject>();
	MQTT["broker"] = myConfig.mqttBroker;
	MQTT["broker_port"] = myConfig.mqttBrokerPort;
}
void deserializeConfig(JsonObject doc) {
	myConfig.mode = doc["mode"];
	myConfig.OTAhost = doc["OTA"]["host"].as<std::string>();

	JsonObject MQTT = doc["MQTT"];
	myConfig.mqttBroker = MQTT["broker"].as<std::string>();
	myConfig.mqttBrokerPort = MQTT["broker_port"];
}
