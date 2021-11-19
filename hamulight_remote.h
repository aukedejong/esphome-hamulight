#include "esphome.h"
#include <bitset>
#include <string>

class HamulightRemote : public Component, public LightOutput {
 private:
   std::string signalToggle = "01010111111000000100010001110111010001110100011101000100010001110111010001110111011101000111010001000100010001000100011101000100010001110100011101010";
   std::string dimMesId = "010101111110000001000100011101110100011101000111010001000100011101110100011101110100";
   std::string dimGlue = "0101";
   std::string dimSignals[100];
   int minDimLevel = 20479;
   int dimStep = 289;
   int pin = 25;
   int period = 200;
   int percentage = 0;
   bool isOn = false;

  
 public:
  void setup() override {
    pinMode(pin, OUTPUT);
    for(int i = 0; i < 100; i++) {
      dimSignals[i] = createDimSignal(i + 1);
    }
  }
  
  LightTraits get_traits() override {
    auto traits = LightTraits();
    traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS, ColorMode::ON_OFF});
    return traits;
  }

  void write_state(LightState *state) override {

    float newBrightness = state->current_values.get_brightness();
    bool newIsOn = state->current_values.is_on();
    int newPercentage = round(newBrightness * 100);
    
    if (newIsOn != isOn) {
      if (newIsOn == true) {
        sendSignal(signalToggle, 10, 0);
        percentage = 1;
        ESP_LOGD("main", "Turn light on");

      } else {
        slowDim(newPercentage, 1);
        sendSignal(signalToggle, 10, 0);
        ESP_LOGD("main", "Turn light off");
      }
    }

    if (newIsOn && percentage != newPercentage) {
      slowDim(percentage, newPercentage);
      ESP_LOGD("main", "Send brightness signal, old=%d, new=%d", percentage, newPercentage);
    }
    
    ESP_LOGD("main", "------------------------------------");
    percentage = newPercentage;
    isOn = newIsOn;
  }
  
  void slowDim(int percentage, int newPercentage) {
    if (percentage < newPercentage) {
        for (int p = percentage; p < newPercentage; p = p + 2) {
          sendSignal(dimSignals[p - 1], 1, 10000);
        }
    } else {
        for (int p = percentage; p > newPercentage; p = p - 2) {
          sendSignal(dimSignals[p - 1], 1, 10000);
        }    
    }
    sendSignal(dimSignals[newPercentage - 1], 2, 45000);
  }
  
  void sendSignal(std::string signal, int count, int messageSpacing) {
    
    for(int j = 0; j < count; j++) {
      
      for(int i = 0; i < signal.length(); i++) {
          if (i == 0 && signal[i] == '0') {
            digitalWrite(pin, HIGH);
            delayMicroseconds(84);
            digitalWrite(pin, LOW);
          }
        
          if (signal[i] == '0') {
            digitalWrite(pin, LOW);
          } else {
            digitalWrite(pin, HIGH);
          }
          delayMicroseconds(period);
          
      }
      digitalWrite(pin, LOW);
      delayMicroseconds(messageSpacing);
    }
  }
    
  std::string createDimSignal(int percentage) {
    
    int dimLevel = minDimLevel - (dimStep * percentage) + dimStep;
    if (dimLevel < 249) {
      dimLevel = dimLevel + 32222;
    }
    if (percentage == 100) {
      dimLevel = 24357;
    }

    std::string binary = std::bitset<15>(dimLevel).to_string();
    std::string dimEncodedSignal = "";
    for(int i = 0; i < binary.length(); i++) {
      if (binary[i] == '1') {
        dimEncodedSignal = dimEncodedSignal + "0111";
      } else { 
        dimEncodedSignal = dimEncodedSignal + "0100";
      }
    }
    return dimMesId + dimEncodedSignal + dimGlue + dimMesId + dimEncodedSignal;
    
  }

};

