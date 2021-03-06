#ifdef USES_C014
//#######################################################################################################
//################################# Controller Plugin 0014: Homie 3/4 ###################################
//#######################################################################################################

#define CPLUGIN_014
#define CPLUGIN_ID_014              14

// Define which Homie version to use
//#define CPLUGIN_014_V3
#define CPLUGIN_014_V4

#ifdef CPLUGIN_014_V3
  #define CPLUGIN_014_HOMIE_VERSION   "3.0.0"
  #define CPLUGIN_NAME_014            "Homie MQTT (Version 3.0.1)"
#endif
#ifdef CPLUGIN_014_V4
  #define CPLUGIN_014_HOMIE_VERSION   "4.0.0"
  #define CPLUGIN_NAME_014            "Homie MQTT (Version 4.0.0 dev)"
#endif

// subscribe and publish shemes should not be changed by the user. This will probably break the homie convention. Do @ your own risk;)
#define CPLUGIN_014_SUBSCRIBE       "homie/%sysname%/#" // "homie/%sysname%/+/+/#" causes problems (!ToDo)
#define CPLUGIN_014_PUBLISH         "homie/%sysname%/%tskname%/%valname%"

#define CPLUGIN_014_BASE_TOPIC      "homie/%sysname%/#"
#define CPLUGIN_014_BASE_VALUE      "homie/%sysname%/%device%/%node%/%property%"
#define CPLUGIN_014_INTERVAL        "90" // to prevent timeout !ToDo set by towest plugin interval
#define CPLUGIN_014_SYSTEM_DEVICE   "SYSTEM" // name for system device Plugin for cmd and GIO values
#define CPLUGIN_014_CMD_VALUE       "cmd" // name for command value
#define CPLUGIN_014_GPIO_VALUE      "gpio" // name for gpio value i.e. "gpio1"
#define CPLUGIN_014_CMD_VALUE_NAME  "Command" // human readabele name for command value

byte msgCounter=0; // counter for send Messages (currently for information / log only!


// send MQTT Message with complete Topic / Payload
bool CPlugin_014_sendMQTTmsg(String& topic, const char* payload, int& errorCounter) {
        bool mqttReturn = MQTTpublish(CPLUGIN_ID_014, topic.c_str(), payload, true);
        if (mqttReturn) msgCounter++;
          else errorCounter++;
        if (loglevelActiveFor(LOG_LEVEL_INFO) && mqttReturn) {
          String log = F("C014 : msg T:");
          log += topic;
          log += F(" P: ");
          log += payload;
          addLog(LOG_LEVEL_DEBUG_MORE, log+" success!");
        }
        if (loglevelActiveFor(LOG_LEVEL_INFO) && !mqttReturn) {
          String log = F("C014 : msg T:");
          log += topic;
          log += F(" P: ");
          log += payload;
          addLog(LOG_LEVEL_ERROR, log+" ERROR!");
        }
        return mqttReturn;
}

// send MQTT Message with CPLUGIN_014_BASE_TOPIC Topic scheme / Payload
bool CPlugin_014_sendMQTTdevice(String tmppubname, const char* topic, const char* payload, int& errorCounter) {
        tmppubname.replace(F("#"), topic);
        bool mqttReturn = MQTTpublish(CPLUGIN_ID_014, tmppubname.c_str(), payload, true);
        if (mqttReturn) msgCounter++;
          else errorCounter++;
        if (loglevelActiveFor(LOG_LEVEL_INFO) && mqttReturn) {
          String log = F("C014 : T:");
          log += topic;
          log += F(" P: ");
          log += payload;
          addLog(LOG_LEVEL_DEBUG_MORE, log+" success!");
        }
        if (loglevelActiveFor(LOG_LEVEL_INFO) && !mqttReturn) {
          String log = F("C014 : T:");
          log += topic;
          log += F(" P: ");
          log += payload;
          addLog(LOG_LEVEL_ERROR, log+" ERROR!");
        }
        processMQTTdelayQueue();
        return mqttReturn;

}

// send MQTT Message with CPLUGIN_014_BASE_VALUE Topic scheme / Payload
bool CPlugin_014_sendMQTTnode(String tmppubname, const char* node, const char* value, const char* topic, const char* payload, int& errorCounter) {
        tmppubname.replace(F("%device%"), node);
        tmppubname.replace(F("%node%"), value);
        tmppubname.replace(F("/%property%"), topic); // leading forward slash required to send "homie/device/value" topics
        bool mqttReturn = MQTTpublish(CPLUGIN_ID_014, tmppubname.c_str(), payload, true);
        if (mqttReturn) msgCounter++;
          else errorCounter++;
        if (loglevelActiveFor(LOG_LEVEL_INFO) && mqttReturn) {
          String log = F("C014 : V:");
          log += value;
          log += F(" T: ");
          log += topic;
          log += F(" P: ");
          log += payload;
          addLog(LOG_LEVEL_DEBUG_MORE, log+" success!");
        }
        if (loglevelActiveFor(LOG_LEVEL_INFO) && !mqttReturn) {
          String log = F("C014 : V:");
          log += value;
          log += F(" T: ");
          log += topic;
          log += F(" P: ");
          log += payload;
          addLog(LOG_LEVEL_ERROR, log+" ERROR!");
        }
        processMQTTdelayQueue();
        return mqttReturn;

}

// and String a comma seperated list
void CPLUGIN_014_addToList(String& valuesList, const char* node)
{
  if (valuesList.length()>0) valuesList += F(",");
  valuesList += node;
}

// search for a Plugin by (user given) Name and gives back the Number in the Device List. Unique Dames are important!
int CPlugin_014_getPluginNr(String& setNodeName)
{
  byte x = 0;

  while (x < TASKS_MAX)
  {
    LoadTaskSettings(x);
    if (strcmp(ExtraTaskSettings.TaskDeviceName,setNodeName.c_str())==0) return x;
    x++;
  }
  return x;
}

// search for a value name and gives back the first value number in the Plugin. Unique names are REQUIRED!
int CPlugin_014_getValueNr(int DeviceIndex, String& valueName)
{
  LoadTaskSettings(DeviceIndex);

  for (byte varNr = 0; varNr < Device[getDeviceIndex(Settings.TaskDeviceNumber[DeviceIndex])].ValueCount; varNr++)
  {
    if (strcmp(ExtraTaskSettings.TaskDeviceValueNames[varNr],valueName.c_str())==0) return varNr;
  }
  return -1;
}

bool CPlugin_014(byte function, struct EventStruct *event, String& string)
{
  bool success = false;
  int errorCounter = 0;
  String log = "";
  String pubname = "";
  String tmppubname = "";

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_014;
        Protocol[protocolCount].usesMQTT = true;
        Protocol[protocolCount].usesTemplate = true;
        Protocol[protocolCount].usesAccount = true;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 1883;
        Protocol[protocolCount].usesID = false;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = F(CPLUGIN_NAME_014);
        break;
      }

    case CPLUGIN_INIT:
      {
        MakeControllerSettings(ControllerSettings);
        LoadControllerSettings(event->ControllerIndex, ControllerSettings);
        MQTTDelayHandler.configureControllerSettings(ControllerSettings);
        break;
      }

    case CPLUGIN_INTERVAL:
      {
        if (MQTTclient.connected())
        {
          errorCounter = 0;

          pubname = CPLUGIN_014_BASE_TOPIC; // Scheme to form device messages
          pubname.replace(F("%sysname%"), Settings.Name);

#ifdef CPLUGIN_014_V3
          // $stats/uptime	Device → Controller	Time elapsed in seconds since the boot of the device	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$stats/uptime",toString((wdcounter / 2)*60,0).c_str(),errorCounter);

          // $stats/signal	Device → Controller	Signal strength in %	Yes	No
          float RssI = WiFi.RSSI();
          RssI = isnan(RssI) ? -100.0 : RssI;
          RssI = min(max(2 * (RssI + 100.0), 0.0), 100.0);

          CPlugin_014_sendMQTTdevice(pubname,"$stats/signal",toString(RssI,1).c_str(),errorCounter);
#endif

          if (errorCounter>0)
          {
            // alert: this is the state the device is when connected to the MQTT broker, but something wrong is happening. E.g. a sensor is not providing data and needs human intervention. You have to send this message when something is wrong.
            CPlugin_014_sendMQTTdevice(pubname,"$state","alert",errorCounter);
            success = false;
          } else {
            // ready: this is the state the device is in when it is connected to the MQTT broker, has sent all Homie messages and is ready to operate. You have to send this message after all other announcements message have been sent.
            CPlugin_014_sendMQTTdevice(pubname,"$state","ready",errorCounter);
            success = true;
          }
          if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
            String log = F("C014 : $stats information sent with ");
            if (errorCounter>0) log+=errorCounter;
              else log+=F("no");
            log+=F(" errors! (");
            log+=msgCounter;
            log+=F(" messages)");
            msgCounter=0;
            addLog(LOG_LEVEL_DEBUG, log);
          }
        }
        break;
      }

    case CPLUGIN_GOT_CONNECTED: //// call after connected to mqtt server to publich device autodicover features
      {
        MakeControllerSettings(ControllerSettings);
        LoadControllerSettings(event->ControllerIndex, ControllerSettings);
        if (!ControllerSettings.checkHostReachable(true)) {
            success = false;
            break;
        }
        statusLED(true);

        // send autodiscover header
        pubname = CPLUGIN_014_BASE_TOPIC; // Scheme to form device messages
        pubname.replace(F("%sysname%"), Settings.Name);
        int deviceCount = 1; // minimum the SYSTEM device exists
        int nodeCount = 1; // minimum the cmd node exists
        errorCounter = 0;

        if (lastBootCause!=BOOT_CAUSE_DEEP_SLEEP) // skip sending autodiscover data when returning from deep sleep
        {
          String nodename = CPLUGIN_014_BASE_VALUE; // Scheme to form node messages
          nodename.replace(F("%sysname%"), Settings.Name);
          String nodesList = ""; // build comma separated List for nodes
          String valuesList = ""; // build comma separated List for values
          byte DeviceIndex = 0;
          String deviceName = ""; // current Device Name nr:name
          String valueName = ""; // current Value Name
          String unitName = ""; // estaimate Units

          // init: this is the state the device is in when it is connected to the MQTT broker, but has not yet sent all Homie messages and is not yet ready to operate. This is the first message that must that must be sent.
          CPlugin_014_sendMQTTdevice(pubname,"$state","init",errorCounter);

          // $homie	Device → Controller	Version of the Homie convention the device conforms to	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$homie",CPLUGIN_014_HOMIE_VERSION,errorCounter);

          // $name	Device → Controller	Friendly name of the device	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$name",Settings.Name,errorCounter);

          // $localip	Device → Controller	IP of the device on the local network	Yes	Yes
#ifdef CPLUGIN_014_V3
          CPlugin_014_sendMQTTdevice(pubname,"$localip",formatIP(WiFi.localIP()).c_str(),errorCounter);

          // $mac	Device → Controller	Mac address of the device network interface. The format MUST be of the type A1:B2:C3:D4:E5:F6	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$mac",WiFi.macAddress().c_str(),errorCounter);

          // $implementation	Device → Controller	An identifier for the Homie implementation (example esp8266)	Yes	Yes
          #if defined(ESP8266)
            CPlugin_014_sendMQTTdevice(pubname,"$implementation","ESP8266",errorCounter);
          #endif
          #if defined(ESP32)
            CPlugin_014_sendMQTTdevice(pubname,"$implementation","ESP32",errorCounter);
          #endif

          // $fw/version	Device → Controller	Version of the firmware running on the device	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$fw/version",toString(Settings.Build,0).c_str(),errorCounter);

          // $fw/name	Device → Controller	Name of the firmware running on the device. Allowed characters are the same as the device ID	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$fw/name",getNodeTypeDisplayString(NODE_TYPE_ID).c_str(),errorCounter);

          // $stats/interval	Device → Controller	Interval in seconds at which the device refreshes its $stats/+: See next section for details about statistical attributes	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname,"$stats/interval",CPLUGIN_014_INTERVAL,errorCounter);
#endif

          //always send the SYSTEM device with the cmd node
          CPLUGIN_014_addToList(nodesList,CPLUGIN_014_SYSTEM_DEVICE);
          CPLUGIN_014_addToList(valuesList,CPLUGIN_014_CMD_VALUE);

          // $name	Device → Controller	Friendly name of the Node	Yes	Yes
          CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, "$name", "", CPLUGIN_014_SYSTEM_DEVICE, errorCounter);

          //$name	Device → Controller	Friendly name of the property.	Any String	Yes	No ("")
          CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, CPLUGIN_014_CMD_VALUE, "/$name", CPLUGIN_014_CMD_VALUE_NAME, errorCounter);
          //$datatype	The data type. See Payloads.	Enum: [integer, float, boolean, string, enum, color]
          CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, CPLUGIN_014_CMD_VALUE, "/$datatype", "string", errorCounter);

          //$settable	Device → Controller	Specifies whether the property is settable (true) or readonly (false)	true or false	Yes	No (false)
          CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, CPLUGIN_014_CMD_VALUE, "/$settable", "true", errorCounter);

          // enum all devices

          // FIRST Standard GPIO tasks
          int gpio = 0;
          // FIXME TD-er: Max of 17 is a limit in the Settings.PinBootStates array???
          while (gpio < MAX_GPIO  && gpio < 17) {
            if (Settings.PinBootStates[gpio]>0) // anything but default
            {
              nodeCount++;
              valueName = CPLUGIN_014_GPIO_VALUE;
              valueName += toString(gpio,0);
              CPLUGIN_014_addToList(valuesList,valueName.c_str());

              //$name	Device → Controller	Friendly name of the property.	Any String	Yes	No ("")
              CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, valueName.c_str(), "/$name", valueName.c_str(), errorCounter);
              //$datatype	The data type. See Payloads.	Enum: [integer, float, boolean,string, enum, color]
              CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, valueName.c_str(), "/$datatype", "boolean", errorCounter);
              if (Settings.PinBootStates[gpio]<3) // defined as default low or high so output
              {
                //$settable	Device → Controller	Specifies whether the property is settable (true) or readonly (false)	true or false	Yes	No (false)
                CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, valueName.c_str(), "/$settable", "true", errorCounter);
              }
            }
            ++gpio;
          }

          // $properties	Device → Controller	Properties the node exposes, with format id separated by a , if there are multiple nodes.	Yes	Yes
          CPlugin_014_sendMQTTnode(nodename, CPLUGIN_014_SYSTEM_DEVICE, "$properties", "", valuesList.c_str(), errorCounter);
          valuesList = "";
          deviceCount++;

          // SECOND Plugins
          for (byte x = 0; x < TASKS_MAX; x++)
          {
            if (Settings.TaskDeviceNumber[x] != 0)
            {
              LoadTaskSettings(x);
              DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[x]);
              deviceName = ExtraTaskSettings.TaskDeviceName;

              if (Settings.TaskDeviceEnabled[x])  // Device is enabled so send information
              { // device enabled
                CPLUGIN_014_addToList(nodesList,deviceName.c_str());
                deviceCount++;
                // $name	Device → Controller	Friendly name of the Node	Yes	Yes
                CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "$name", "",ExtraTaskSettings.TaskDeviceName, errorCounter);

                // $type	Device → Controller	Type of the node	Yes	Yes
                CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "$type", "", getPluginNameFromDeviceIndex(DeviceIndex).c_str(), errorCounter);

                valuesList="";

                if (!Device[DeviceIndex].SendDataOption) // check if device is not sending data = assume that it can receive.
                {
/*                  if (Device[DeviceIndex].Number==12) // LCD 2 or 4 Lines
                  {
                    CPLUGIN_014_addToList(valuesList,"LCD");
                    CPLUGIN_014_addToList(valuesList,"LCDCMD");
                    //$settable	Device → Controller	Specifies whether the property is settable (true) or readonly (false)	true or false	Yes	No (false)
                    CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "LCD", "/$settable", "true", errorCounter);
                    CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "LCDCMD", "/$settable", "true", errorCounter);

                    //$name	Device → Controller	Friendly name of the property.	Any String	Yes	No ("")
                    CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "LCD", "/$name","LCD text", errorCounter);
                    CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "LCDCMD", "/$name","LCD command", errorCounter);

                    //$datatype	The data type. See Payloads.	Enum: [integer, float, boolean,string, enum, color]
                    CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "LCD", "/$datatype", "string", errorCounter);
                    CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "LCDCMD", "/$datatype", "string", errorCounter);

                    nodeCount++;
                  } */
                } else {
                  // ignore cutom values for now! Assume all Values are standard float.
                  // customValues = PluginCall(PLUGIN_WEBFORM_SHOW_VALUES, &TempEvent,TXBuffer.buf);
                  byte customValues = false;
                  if (!customValues)
                  { // standard Values
                    for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
                    {
                      if (Settings.TaskDeviceNumber[x] != 0)
                      {
                        if (ExtraTaskSettings.TaskDeviceValueNames[varNr][0]!=0) // do not send if Value Name is empty!
                        {
                          CPLUGIN_014_addToList(valuesList,ExtraTaskSettings.TaskDeviceValueNames[varNr]);

                          //$name	Device → Controller	Friendly name of the property.	Any String	Yes	No ("")
                          CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), ExtraTaskSettings.TaskDeviceValueNames[varNr], "/$name", ExtraTaskSettings.TaskDeviceValueNames[varNr], errorCounter);
                          //$datatype	The data type. See Payloads.	Enum: [integer, float, boolean,string, enum, color]
                          CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), ExtraTaskSettings.TaskDeviceValueNames[varNr], "/$datatype", "float", errorCounter);

                          if (Device[DeviceIndex].Number==33) // Dummy Device can send AND receive Data
                            CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), ExtraTaskSettings.TaskDeviceValueNames[varNr], "/$settable", "true", errorCounter);

                          nodeCount++;
/*                          // because values in ESPEasy are unitless lets assueme some units by the value name (still case sensitive)
                          if (strstr(ExtraTaskSettings.TaskDeviceValueNames[varNr], "temp") != NULL )
                          {
                            unitName = "°C";
                          } else if (strstr(ExtraTaskSettings.TaskDeviceValueNames[varNr], "humi") != NULL )
                          {
                            unitName = "%";
                          } else if (strstr(ExtraTaskSettings.TaskDeviceValueNames[varNr], "press") != NULL )
                          {
                            unitName = "Pa";
                          } // ToDo: .... and more

                          if (unitName != "")  // found a unit match
                          {
                            // $unit	Device → Controller	A string containing the unit of this property. You are not limited to the recommended values, although they are the only well known ones that will have to be recognized by any Homie consumer.	Recommended: Yes	No ("")
                            CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), ExtraTaskSettings.TaskDeviceValueNames[varNr], "/$unit", unitName.c_str(), errorCounter);
                          }
                          unitName = "";
*/                      }
                      }
                    } // end loop throug values
                  } else { // Device has custom Values
                    if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
                      String log = F("C014 : Device has custom values: ");
                      log += getPluginNameFromDeviceIndex(Settings.TaskDeviceNumber[x]);
                      addLog(LOG_LEVEL_DEBUG, log+" not implemented!")
                    }
                  }
                }
                if (valuesList!="")
                {
                  // $properties	Device → Controller	Properties the node exposes, with format id separated by a , if there are multiple nodes.	Yes	Yes
                  CPlugin_014_sendMQTTnode(nodename, deviceName.c_str(), "$properties", "", valuesList.c_str(), errorCounter);
                  valuesList="";
                }
              } else { // device not enabeled
                if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
                  String log = F("C014 : Device Disabled: ");
                  log += getPluginNameFromDeviceIndex(Settings.TaskDeviceNumber[x]);
                  addLog(LOG_LEVEL_DEBUG, log+" not propagated!")
                }
              }
            } // device configured
          } // loop through devices

          // and finally ...
          // $nodes	Device → Controller	Nodes the device exposes, with format id separated by a , if there are multiple nodes. To make a node an array, append [] to the ID.	Yes	Yes
          CPlugin_014_sendMQTTdevice(pubname, "$nodes", nodesList.c_str(), errorCounter);
        }

        if (errorCounter>0)
        {
          // alert: this is the state the device is when connected to the MQTT broker, but something wrong is happening. E.g. a sensor is not providing data and needs human intervention. You have to send this message when something is wrong.
          CPlugin_014_sendMQTTdevice(pubname,"$state","alert",errorCounter);
          success = false;
        } else {
          // ready: this is the state the device is in when it is connected to the MQTT broker, has sent all Homie messages and is ready to operate. You have to send this message after all other announcements message have been sent.
          CPlugin_014_sendMQTTdevice(pubname,"$state","ready",errorCounter);
          success = true;
        }
        if (loglevelActiveFor(LOG_LEVEL_INFO)) {
          String log = F("C014 : autodiscover information of ");
          log += deviceCount;
          log += F(" Devices and ");
          log += nodeCount;
          log += F(" Nodes sent with ");
          if (errorCounter>0) log+=errorCounter;
            else log+=F("no");
          log+=F(" errors! (");
          log+=msgCounter;
          log+=F(" messages)");
          msgCounter = 0;
          errorCounter = 0;
          addLog(LOG_LEVEL_INFO, log);
        }
        break;
      }

    case CPLUGIN_PROTOCOL_TEMPLATE:
      {
        event->String1 = F(CPLUGIN_014_SUBSCRIBE);
        event->String2 = F(CPLUGIN_014_PUBLISH);
        break;
      }

    case CPLUGIN_GOT_INVALID:
      {
        pubname = CPLUGIN_014_BASE_TOPIC; // Scheme to form device messages
        pubname.replace(F("%sysname%"), Settings.Name);
        // disconnected: this is the state the device is in when it is cleanly disconnected from the MQTT broker. You must send this message before cleanly disconnecting
        success = CPlugin_014_sendMQTTdevice(pubname,"$state","disconnected",errorCounter);
        if (loglevelActiveFor(LOG_LEVEL_INFO)) {
          String log = F("C014 : Device: ");
          log += Settings.Name;
          if (success) log += F(" got invalid (disconnected).");
            else log += F(" got invaild (disconnect) failed!");
          addLog(LOG_LEVEL_INFO, log);
        }
        break;
      }

    case CPLUGIN_FLUSH:
      {
        pubname = CPLUGIN_014_BASE_TOPIC; // Scheme to form device messages
        pubname.replace(F("%sysname%"), Settings.Name);
        // sleeping: this is the state the device is in when the device is sleeping. You have to send this message before sleeping.
        success = CPlugin_014_sendMQTTdevice(pubname,"$state","sleeping",errorCounter);
        break;
      }

    case CPLUGIN_PROTOCOL_RECV:
      {
        byte ControllerID = findFirstEnabledControllerWithId(CPLUGIN_ID_014);
        bool validTopic = false;
        if (ControllerID == CONTROLLER_MAX) {
          // Controller is not enabled.
          break;
        } else {
          String cmd;
          struct EventStruct TempEvent;
          TempEvent.TaskIndex = event->TaskIndex;
          TempEvent.Source = VALUE_SOURCE_MQTT; // to trigger the correct acknowledgment
          int lastindex = event->String1.lastIndexOf('/');
          errorCounter = 0;
          if (event->String1.substring(lastindex + 1) == F("set"))
          {
            pubname = event->String1.substring(0,lastindex);
            lastindex = pubname.lastIndexOf('/');
            String nodeName = pubname.substring(0,lastindex);
            String valueName = pubname. substring(lastindex+1);
            lastindex = nodeName.lastIndexOf('/');
            nodeName = nodeName. substring(lastindex+1);
            if (loglevelActiveFor(LOG_LEVEL_INFO)) {
              log=F("C014 : MQTT received: ");
              log+=F("/set: N: ");
              log+=nodeName;
              log+=F(" V: ");
              log+=valueName;
            }
            if (nodeName == F(CPLUGIN_014_SYSTEM_DEVICE)) // msg to a system device
            {
              if (valueName.substring(0,strlen(CPLUGIN_014_GPIO_VALUE)) == F(CPLUGIN_014_GPIO_VALUE)) // msg to to set gpio values
              {
                cmd = ("GPIO,");
                cmd += valueName.substring(strlen(CPLUGIN_014_GPIO_VALUE)).toInt(); // get the GPIO
                if (event->String2=="true" || event->String2=="1") cmd += F(",1");
                  else cmd += F(",0");
                validTopic = true;
              } else if (valueName==CPLUGIN_014_CMD_VALUE) // msg to send a command
              {
                cmd = event->String2;
                validTopic = true;
              } else
              {
                cmd = F("SYSTEM/");
                cmd += valueName;
                cmd += F(" unknown!");
              }
            } else // msg to a receiving plugin
            {
              int deviceNr=CPlugin_014_getPluginNr(nodeName);
              int pluginID=Device[getDeviceIndex(Settings.TaskDeviceNumber[deviceNr])].Number;

              if (pluginID==33) // Plugin 33 Dummy Device
              { // DummyValueSet,<task/device nr>,<value nr>,<value/formula (!ToDo) >, works only with new version of P033!
                int valueNr = CPlugin_014_getValueNr(deviceNr,valueName);
                if (valueNr > -1) // value Name identified
                {
                  cmd = F("DummyValueSet,"); // Set a Dummy Device Value
                  cmd += (deviceNr+1); // set the device Number
                  cmd += F(",");
                  cmd += (valueNr+1); // set the value Number
                  cmd += F(",");
                  cmd += event->String2; // expect float as payload!
                  validTopic = true;
                }
              }
            }

            if (validTopic) {
              parseCommandString(&TempEvent, cmd);
              if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                log+=F(" cmd: ");
                log+=cmd;
                addLog(LOG_LEVEL_INFO, log+ F(" OK"));
              }
            } else {
              if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                addLog(LOG_LEVEL_INFO, log+ F(" ERROR"));
              }
            }
          }

          if (validTopic) {
            if (loglevelActiveFor(LOG_LEVEL_INFO)) {
              log=F("C014 : Exec > ");
            }
            // in case of event, store to buffer and return...
            String command = parseString(cmd, 1);
            if (command == F("event"))
            {
              if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                log+=F("Event!");
              }
              eventBuffer = cmd.substring(6);
            } else { // not an event
              if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                log=F("C014 : PluginCall:");
              }
              if (!PluginCall(PLUGIN_WRITE, &TempEvent, cmd)) {
                remoteConfig(&TempEvent, cmd);
                if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                  log +=F(" remoteConfig?");
                }
              } else {
                if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                  log +=F(" OK!");
                }
              }
              if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                addLog(LOG_LEVEL_INFO, log);
              }
            }

          }
        }
        success = validTopic;
        break;
      }

    case CPLUGIN_PROTOCOL_SEND:
      {
        MakeControllerSettings(ControllerSettings);
        LoadControllerSettings(event->ControllerIndex, ControllerSettings);
        if (!ControllerSettings.checkHostReachable(true)) {
            success = false;
            break;
        }
        statusLED(true);

        if (ExtraTaskSettings.TaskIndex != event->TaskIndex)
          PluginCall(PLUGIN_GET_DEVICEVALUENAMES, event, dummyString);

        String pubname = ControllerSettings.Publish;
        parseControllerVariables(pubname, event, false);

        String value = "";
        // byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[event->TaskIndex]);
        byte valueCount = getValueCountFromSensorType(event->sensorType);
        for (byte x = 0; x < valueCount; x++)
        {
          String tmppubname = pubname;
          tmppubname.replace(F("%valname%"), ExtraTaskSettings.TaskDeviceValueNames[x]);
          value = formatUserVarNoCheck(event, x);

          MQTTpublish(event->ControllerIndex, tmppubname.c_str(), value.c_str(), Settings.MQTTRetainFlag);
          if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
            String log = F("C014 : Sent to ");
            log += tmppubname;
            log += ' ';
            log += value;
            addLog(LOG_LEVEL_DEBUG, log);
          }
        }
        break;
      }

      case CPLUGIN_ACKNOWLEDGE:
      {
        LoadTaskSettings(event->Par1-1);
        if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
          String log = F("CPLUGIN_ACKNOWLEDGE: ");
          log += string;
          log += F(" / ");
          log += ExtraTaskSettings.TaskDeviceName;
          log += F(" / ");
          log += ExtraTaskSettings.TaskDeviceValueNames[event->Par2-1];
          log += F(" sensorType:");
          log += event->sensorType;
          log += F(" Source:");
          log += event->Source;
          log += F(" idx:");
          log += event->idx;
          log += F(" S1:");
          log += event->String1;
          log += F(" S2:");
          log += event->String2;
          log += F(" S3:");
          log += event->String3;
          log += F(" S4:");
          log += event->String4;
          log += F(" S5:");
          log += event->String5;
          log += F(" P1:");
          log += event->Par1;
          log += F(" P2:");
          log += event->Par2;
          log += F(" P3:");
          log += event->Par3;
          log += F(" P4:");
          log += event->Par4;
          log += F(" P5:");
          log += event->Par5;
          addLog(LOG_LEVEL_DEBUG, log);
        }
        success = false;
        if (string!="") {
          String commandName = parseString(string, 1); // could not find a way to get the command out of the event structure.
          if (commandName == F("gpio")) //!ToDo : As gpio is like any other plugin commands should be integrated below!
          {
            int port = event -> Par1; // parseString(string, 2).toInt();
            int valueInt = event -> Par2; //parseString(string, 3).toInt();
            String valueBool = "false";
            if (valueInt==1) valueBool = "true";

            String topic = CPLUGIN_014_PUBLISH; // ControllerSettings.Publish not used because it can be modified by the user!
            topic.replace(F("%sysname%"), Settings.Name);
            topic.replace(F("%tskname%"), CPLUGIN_014_SYSTEM_DEVICE);
            topic.replace(F("%valname%"), CPLUGIN_014_GPIO_VALUE + toString(port,0));

            success = MQTTpublish(CPLUGIN_ID_014, topic.c_str(), valueBool.c_str(), false);
            if (loglevelActiveFor(LOG_LEVEL_INFO) && success) {
              String log = F("C014 : Acknowledged GPIO");
              log += port;
              log += F(" value:");
              log += valueBool;
              log += F(" (");
              log += valueInt;
              log += F(")");
              addLog(LOG_LEVEL_INFO, log+" success!");
            }
            if (loglevelActiveFor(LOG_LEVEL_ERROR) && !success) {
              String log = F("C014 : Acknowledged GPIO");
              log += port;
              log += F(" value:");
              log += valueBool;
              log += F(" (");
              log += valueInt;
              log += F(")");
              addLog(LOG_LEVEL_ERROR, log+" ERROR!");
            }
          } else // not gpio
          {
            String topic = CPLUGIN_014_PUBLISH;
            topic.replace(F("%sysname%"), Settings.Name);
            int deviceIndex = event->Par1; //parseString(string, 2).toInt();
            LoadTaskSettings(deviceIndex-1);
            String deviceName = ExtraTaskSettings.TaskDeviceName;
            topic.replace(F("%tskname%"), deviceName);
            String valueName = ExtraTaskSettings.TaskDeviceValueNames[event->Par2-1]; //parseString(string, 3).toInt()-1];
            topic.replace(F("%valname%"), valueName);

            if ((commandName == F("taskvalueset")) || (commandName == F("dummyvalueset"))) // should work for both
            {
              String valueStr = toString(UserVar[event->BaseVarIndex+event->Par2-1],ExtraTaskSettings.TaskDeviceValueDecimals[event->Par2-1]); //parseString(string, 4);
              success = MQTTpublish(CPLUGIN_ID_014, topic.c_str(), valueStr.c_str(), false);
              if (loglevelActiveFor(LOG_LEVEL_INFO) && success) {
                String log = F("C014 : Acknowledged: ");
                log += deviceName;
                log += F(" var: ");
                log += valueName;
                log += F(" topic: ");
                log += topic;
                log += F(" value: ");
                log += valueStr;
                addLog(LOG_LEVEL_INFO, log+" success!");
              }
              if (loglevelActiveFor(LOG_LEVEL_ERROR) && !success) {
                String log = F("C014 : Aacknowledged: ");
                log += deviceName;
                log += F(" var: ");
                log += valueName;
                log += F(" topic: ");
                log += topic;
                log += F(" value: ");
                log += valueStr;
                addLog(LOG_LEVEL_ERROR, log+" ERROR!");
              }
            } else // Acknowledge not implemented yet
            {
              if (loglevelActiveFor(LOG_LEVEL_INFO)) {
                String log = F("C014 : Plugin acknowledged: ");
                log+=function;
                log+=F(" / ");
                log+=commandName;
                log+=F(" cmd: ");
                log+=string;
                log+=F(" not implemented!");
                addLog(LOG_LEVEL_ERROR, log);
              }
              success = false;
            }
          }
        }
      }
  }

  return success;
}
#endif
