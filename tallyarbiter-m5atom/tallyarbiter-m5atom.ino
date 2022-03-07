#include <M5Atom.h>
#include <WiFi.h>
#include <SocketIoClient.h>
#include <Arduino_JSON.h>
#include <PinButton.h>
#include <stdint.h>
#include <Arduino.h>
#include <Preferences.h>
#define DATA_PIN_LED 27

//General Variables
bool networkConnected = false;
int currentScreen;
uint8_t FSM = 0;

//M5 variables
PinButton btnAction(39); //the "Action" button on the device
Preferences preferences;

/* USER CONFIG VARIABLES
    Change the following variables before compiling and sending the code to your device.
*/

//Wifi SSID and password
const char * networkSSID = "YourNetwork";
const char * networkPass = "YourPassword";

//Tally Arbiter Server
const char * tallyarbiter_host = "TALLYARBITERSERVERIP";
const int tallyarbiter_port = 4455;

// Local Default Camera Number.
// Only used for local display of the number in the screen. It does NOT impact any function. "0" results in a full screen color displayed.
int camNumber = 0;

//Tally Arbiter variables
SocketIoClient socket;
JSONVar BusOptions;
JSONVar Devices;
JSONVar DeviceStates;
String DeviceId = "unassigned";
String DeviceName = "unassigned";
String ListenerType = "m5-atom";
bool mode_preview = true;
bool mode_program = false;


// default color values
int RGB_COLOR_WHITE = 0xffffff;
int RGB_COLOR_BLACK = 0x000000;
int RGB_COLOR_RED = 0xff0000;
int RGB_COLOR_ORANGE = 0xffa500;
int RGB_COLOR_YELLOW = 0xffff00;
int RGB_COLOR_GREEN = 0x00ff00;
int RGB_COLOR_BLUE = 0x0000ff;
int RGB_COLOR_PURPLE = 0x800080;

int numbercolor = RGB_COLOR_ORANGE;

int programcolor[] = {RGB_COLOR_RED, numbercolor};
int previewcolor[] = {RGB_COLOR_GREEN, numbercolor};
int mixedcolor[] = {RGB_COLOR_RED, numbercolor};
int flashcolor[] = {RGB_COLOR_WHITE, RGB_COLOR_WHITE};
int offcolor[] = {RGB_COLOR_BLACK, numbercolor};
int readycolour[] = {RGB_COLOR_BLUE, RGB_COLOR_BLUE};
int alloffcolor[] = {RGB_COLOR_BLACK, RGB_COLOR_BLACK};

int wificolor[] = {RGB_COLOR_BLACK, RGB_COLOR_BLUE};
int wifi_dead_color[] = {RGB_COLOR_BLACK, RGB_COLOR_RED};

int currentBrightness = 100;

int wifi_icon[25] = {
  0, 0, 1, 1, 1,
  0, 1, 0, 0, 0,
  1, 0, 0, 1, 1,
  1, 0, 1, 0, 0,
  1, 0, 1, 0, 1
};

int wifi_dead_icon[25] = {
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 1, 1,
  0, 0, 1, 0, 0,
  0, 0, 1, 0, 1
};

int all_leds_on[25] = {
  1, 1, 1, 1, 1,
  1, 1, 1, 1, 1,
  1, 1, 1, 1, 1,
  1, 1, 1, 1, 1,
  1, 1, 1, 1, 1
};

//this is the array that stores the number layout
int number[17][25] = {{
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    0, 1, 0, 0, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 1, 1, 0, 1,
    1, 0, 1, 0, 1,
    1, 0, 1, 1, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 0, 1, 0, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 1, 0, 0,
    1, 1, 1, 0, 0,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 0, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 1, 1, 0, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 0, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0,
    1, 0, 1, 0, 0,
    1, 0, 0, 1, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 1, 1, 0, 1,
    0, 0, 0, 0, 0
  },
  { 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1
  },
  { 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0
  },
  { 1, 1, 1, 0, 1,
    1, 0, 1, 0, 1,
    1, 0, 1, 1, 1,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1
  },
  { 1, 1, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 0, 1, 0, 1,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1
  },
  { 1, 1, 1, 1, 1,
    0, 0, 1, 0, 0,
    1, 1, 1, 0, 0,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1
  },
  { 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 1, 1, 0, 1,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1
  },
  { 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1
  },
};

// Logger - logs to serial number
void logger(String strLog, String strType) {
  if (strType == "info") {
    Serial.println(strLog);
  }
  else {
    Serial.println(strLog);
  }
}

// Set Device name
void setDeviceName()
{
  for (int i = 0; i < Devices.length(); i++) {
    if (JSON.stringify(Devices[i]["id"]) == "\"" + DeviceId + "\"") {
      String strDevice = JSON.stringify(Devices[i]["Type"]);
      DeviceName = strDevice.substring(1, strDevice.length() - 1);
      break;
    }
  }
  preferences.begin("tally-arbiter", false);
  preferences.putString("devicename", DeviceName);
  preferences.putString("deviceid", DeviceId);
  preferences.end();
  logger("-------------------------------------------------", "info-quiet");
  logger("DeviceName:" + String(DeviceName), "info-quiet");
  logger("DeviceId:" + String(DeviceId), "info-quiet");
  logger("-------------------------------------------------", "info-quiet");
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      logger("Network connected!", "info");
      logger(String(WiFi.localIP()), "info");
      networkConnected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      logger("Network connection lost!", "info");
      networkConnected = false;
      break;
  }
}

//---------------------------------------------------------------
//HERE IS THE MAIN LED DRAWING ROUTINE aka drawNumber
void drawNumber(int arr[], int colors[])
{
  M5.dis.setBrightness(currentBrightness);
  for (int i = 0; i < 25; i++)
  {
    M5.dis.drawpix(i, colors[arr[i]]);
  }
}
//---------------------------------------------------------------

// Determine if the device is currently in preview, program, or both
void evaluateMode() {
  if (mode_preview && !mode_program) {
    logger("The device is in preview.", "info-quiet");
    M5.dis.clear();
    drawNumber(number[camNumber], previewcolor);
  }
  else if (!mode_preview && mode_program) {
    logger("The device is in program.", "info-quiet");
    M5.dis.clear();
    drawNumber(number[camNumber], programcolor);
  }
  else if (mode_preview && mode_program) {
    logger("The device is in preview and program.", "info-quiet");
    M5.dis.clear();
    drawNumber(number[camNumber], mixedcolor);
  }
  else {
    logger("The device is clear.", "info-quiet");
    M5.dis.clear();
    drawNumber(number[camNumber], offcolor);
  }
}



void showDeviceInfo() {
  //displays the currently assigned device and tally data
  evaluateMode();
}

// Here are all the socket listen events - messages sent from Tally Arbiter to the M5

void socket_Flash(const char * payload, size_t length) {
  //flash the screen white 3 times
  logger("The device flashed.", "info-quiet");
  drawNumber(all_leds_on, alloffcolor);
  delay(100);
  drawNumber(all_leds_on, flashcolor);
  delay(100);
  drawNumber(all_leds_on, alloffcolor);
  delay(100);
  drawNumber(all_leds_on, flashcolor);
  delay(100);
  drawNumber(all_leds_on, alloffcolor);
  delay(100);
  drawNumber(all_leds_on, flashcolor);
  delay(100);
  drawNumber(all_leds_on, alloffcolor);
  delay(100);
  //then resume normal operation
  switch (currentScreen) {
    case 0:
      showDeviceInfo();
      break;
    case 1:
      //  showSettings();
      break;
  }
}


void socket_Reassign(const char * payload, size_t length) {
  logger("Socket Reassign: " + String(payload), "info-quiet");
  Serial.println(payload);
  String oldDeviceId = String(payload).substring(0, 8);
  String newDeviceId = String(payload).substring(11);
  String reassignObj = "{\"oldDeviceId\": \"" + oldDeviceId + "\", \"newDeviceId\": \"" + newDeviceId + "\"}";
  char charReassignObj[1024];
  strcpy(charReassignObj, reassignObj.c_str());
  socket.emit("listener_reassign_object", charReassignObj);
  // Flash 2 times

  drawNumber(wifi_icon, alloffcolor);
  delay(200);
  drawNumber(wifi_icon, wificolor);
  delay(300);
  drawNumber(wifi_icon, alloffcolor);
  delay(200);
  drawNumber(wifi_icon, wificolor);
  delay(300);
  drawNumber(wifi_icon, alloffcolor);
  delay(200);
  //

  DeviceId = newDeviceId;
  preferences.begin("tally-arbiter", false);
  preferences.putString("deviceid", newDeviceId);
  preferences.end();
  setDeviceName();
  logger("Socket Reassign pt2: " + String(newDeviceId), "info-quiet");
}

void connectToServer() {
  logger("---------------------------------", "info-quiet");
  logger("Connecting to Tally Arbiter host: " + String(tallyarbiter_host), "info-quiet");
  socket.on("connect", socket_Connected);
  socket.on("bus_options", socket_BusOptions);
  socket.on("deviceId", socket_DeviceId);
  socket.on("devices", socket_Devices);
  socket.on("device_states", socket_DeviceStates);
  socket.on("flash", socket_Flash);
  socket.on("reassign", socket_Reassign);
  socket.begin(tallyarbiter_host, tallyarbiter_port);
  logger("---------------------------------", "info-quiet");
}

void socket_Connected(const char * payload, size_t length) {
  logger("---------------------------------", "info-quiet");
  logger("Connected to Tally Arbiter host: " + String(tallyarbiter_host), "info-quiet");
  String deviceObj = "{\"deviceId\": \"" + DeviceId + "\", \"listenerType\": \"" + ListenerType + "\"}";
  logger("deviceObj = " + String(deviceObj), "info-quiet");
  logger("DeviceId = " + String(DeviceId), "info-quiet");
  char charDeviceObj[1024];
  strcpy(charDeviceObj, deviceObj.c_str());
  socket.emit("bus_options");
  socket.emit("device_listen_m5", charDeviceObj);
  logger("charDeviceObj = " + String(charDeviceObj), "info-quiet");
  logger("---------------------------------", "info-quiet");
}

void socket_BusOptions(const char * payload, size_t length) {
  logger("Socket Message BusOptions: " + String(payload), "info-quiet");
  BusOptions = JSON.parse(payload);
}

void socket_Devices(const char * payload, size_t length) {
  logger("Socket Message Devices: " + String(payload), "info-quiet");
  Devices = JSON.parse(payload);
  setDeviceName();
}

void socket_DeviceId(const char * payload, size_t length) {
  logger("Socket Message DeviceId: " + String(payload), "info-quiet");
  DeviceId = String(payload);
  setDeviceName();
}


void processTallyData() {
  for (int i = 0; i < DeviceStates.length(); i++) {
    if (getBusTypeById(JSON.stringify(DeviceStates[i]["busId"])) == "\"preview\"") {
      if (DeviceStates[i]["sources"].length() > 0) {
        mode_preview = true;
      }
      else {
        mode_preview = false;
      }
    }
    if (getBusTypeById(JSON.stringify(DeviceStates[i]["busId"])) == "\"program\"") {
      if (DeviceStates[i]["sources"].length() > 0) {
        mode_program = true;
      }
      else {
        mode_program = false;
      }
    }
  }

  evaluateMode();
}

String getBusTypeById(String busId) {
  for (int i = 0; i < BusOptions.length(); i++) {
    if (JSON.stringify(BusOptions[i]["id"]) == busId) {
      return JSON.stringify(BusOptions[i]["type"]);
    }
  }

  return "invalid";
}


void socket_DeviceStates(const char * payload, size_t length) {
  logger("Socket Message DeviceStates: " + String(payload), "info-quiet");
  DeviceStates = JSON.parse(payload);
  processTallyData();
}


void connectToNetwork() {
  logger("", "info");
  logger("Connecting to SSID: " + String(networkSSID), "info");

  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);

  WiFi.mode(WIFI_STA); //station
  WiFi.setSleep(false);

  WiFi.begin(networkSSID, networkPass);
}

// --------------------------------------------------------------------------------------------------------------------
// Setup is the pre-loop running program

void setup() {
  Serial.begin(115200);
  while (!Serial);
  logger("Initializing M5-Atom.", "info-quiet");

  M5.begin(true, false, true);
  delay(50);
  M5.dis.drawpix(0, 0xf00000);

  // blanks out the screen
  drawNumber(all_leds_on, alloffcolor);
  delay(100); //wait 100ms before moving on

  connectToNetwork(); //starts Wifi connection
  while (!networkConnected) {
    drawNumber(wifi_dead_icon, wifi_dead_color);
    delay(300);
    drawNumber(wifi_dead_icon, alloffcolor);
    delay(1200);
  }

  // Flash screen if connected to wifi.
  // Would like to animate this as a wifi logo in the future
  drawNumber(wifi_icon, alloffcolor);
  delay(100);
  drawNumber(wifi_icon, wificolor);
  delay(800);
  drawNumber(wifi_icon, alloffcolor);
  delay(100);

  preferences.begin("tally-arbiter", false);
  if (preferences.getString("deviceid").length() > 0) {
    DeviceId = preferences.getString("deviceid");
    //DeviceId = "unassigned";
  }
  if (preferences.getString("devicename").length() > 0) {
    DeviceName = preferences.getString("devicename");
    //DeviceName = "unassigned";
  }
  preferences.end();

  connectToServer();
  delay (100);
}
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// This is the main program loop
void loop()
{
  socket.loop();
  if (M5.Btn.wasPressed())
  {
    // Lets get some info sent out the serial connection for debugging
    logger("", "info-quiet");
    logger("---------------------------------", "info-quiet");
    logger("Button Pressed.", "info-quiet");
    logger("M5Atom IP Address: " + String(WiFi.localIP()), "info-quiet");
    logger("Tally Arbiter Server: " + String(tallyarbiter_host), "info-quiet");
    logger("Device ID: " + String(DeviceId), "info-quiet");
    logger("Device Name: " + String(DeviceName), "info-quiet");
    logger("---------------------------------", "info-quiet");
    logger("", "info-quiet");
    // Switch action below
    if (FSM < 17)
    {
      camNumber = FSM;
      drawNumber(number[FSM], offcolor);
    }
    FSM++;
    if (FSM > 16)
    {
      FSM = 0;
    }
  }

  delay(50);
  M5.update();
}
// --------------------------------------------------------------------------------------------------------------------
