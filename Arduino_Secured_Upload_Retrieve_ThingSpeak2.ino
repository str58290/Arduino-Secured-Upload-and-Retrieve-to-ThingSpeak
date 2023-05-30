/*
  Credit/Reference:
  https://randomnerdtutorials.com/esp32-https-requests/#esp32-https-requests-httpclient-certificate
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "WIFIssid";
const char* password = "WIFIpw";

//ThingSpeak credentials
String channelID = "channlID";
String writeAPIKey = "channelWRITEapiKey";
String readAPIKey = "channelREADapiKey";

int test_data = 0;

// This is the cert of ThingSpeak:
const char* rootCACertificate = \
      "-----BEGIN CERTIFICATE-----\n" \
      "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
      "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
      "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
      "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
      "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
      "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
      "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"\
      "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
      "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
      "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
      "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
      "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
      "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
      "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
      "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
      "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
      "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
      "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
      "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"\
      "-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
  Serial.println();

  connect_wifi();
  thingspeak_post();
  thingspeak_get();
}

void loop() {
}

void connect_wifi(void){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void thingspeak_post(void){
  WiFiClientSecure *client = new WiFiClientSecure;
  HTTPClient https;

  if (WiFi.status() == WL_CONNECTED && client){
    client->setCACert(rootCACertificate); // Set/Insert ThingSpeak certificate into client here

    Serial.print("[HTTPS] POST begin ... \n");
    https.begin(*client, "https://api.thingspeak.com/update"); // Notice link starts with 'https' instead of 'http' as we are using a secured connection to ThingSpeak via TLS/SSL
    https.addHeader("Content-Type","application/x-www-form-urlencoded");

    String httpsPostData = "api_key=" + writeAPIKey + "&field2=" + String(test_data); // Channel num is irrelevant here as API key is unique to all channels

    int httpCode = https.POST(httpsPostData); // POST data to field in this section

    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);
    if (httpCode == 200) {
      Serial.println("Channel updated successful.");
    }
  }

  // Close the connection
  https.end();
  delay(20000); // Mandatory upload delay interval for free ThingSpeak version
}

void thingspeak_get(void){
  WiFiClientSecure *client = new WiFiClientSecure;
  HTTPClient https;

  if (WiFi.status() == WL_CONNECTED && client){
    client->setCACert(rootCACertificate); // Set/Insert ThingSpeak certificate into client here

    Serial.print("[HTTPS] GET begin ... \n");
    String GETurl = "https://api.thingspeak.com/channels/"; // Notice link starts with 'https' instead of 'http' as we are using a secured connection to ThingSpeak via TLS/SSL
    GETurl += channelID;
    GETurl += "/fields/2/last.json?api_key="; // Get the latest value from field 2
    GETurl += readAPIKey;
    https.begin(*client, GETurl);

    int httpsResponseCode = https.GET();
    if (httpsResponseCode == HTTP_CODE_OK) {
      String payload = https.getString();

      // Parse the JSON response to collect only the latest value from field2
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.println("Error parsing JSON");
        return;
      }
      // End

      // Get the latest field value
      const char* fieldValue = doc["field2"];
      Serial.print("Latest Field 2 value: ");
      Serial.println(fieldValue);

      // Here, you can process the received data as per your requirements
    } 
    
    else {
      Serial.print("Error occurred during HTTPS request. Error code: ");
      Serial.println(httpsResponseCode);
    }

    // Close the connection
    https.end();
  }  
}