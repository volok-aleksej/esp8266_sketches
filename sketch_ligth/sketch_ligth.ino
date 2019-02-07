#include <twainet.h>
#include <logger.h>

char motion[] = "motion";
char noMotion[] = "no motion";
char ligthOff[] = "Ligth OFF";
char ligthOn[] = "Ligth ON";
char on[] = "on";
char off[] = "off";

bool status = false;
bool sensorValue = false;
int analogValue;
bool releStatus = false;
unsigned long ligthTimeStart = 0;

struct Light
{
    void doCommand(const twnstd::vector<String>& params)
    {
        if(!params.length()) {
            LOG_ERROR("absent value of  ligth command, use '%s' or '%s'", on, off);
        } else if(const_cast<twnstd::vector<String>&>(params)[0] == "on") {
            digitalWrite(12, HIGH);
            releStatus = true;
        } else if(const_cast<twnstd::vector<String>&>(params)[0] == "off") {
            digitalWrite(12, LOW);
            releStatus = false;
        } else {
            LOG_ERROR("unknown value of  ligth command, use '%s' or '%s'", on, off);
        }
    }

    twnstd::vector<String> getNextCommandArgs(const twnstd::vector<String>& params)
    {
        if(params.length() == 1 && (const_cast<twnstd::vector<String>&>(params)[0] == on || const_cast<twnstd::vector<String>&>(params)[0] == off)) {
            return twnstd::vector<String>();
        }
        if(params.length() <= 1) {
            twnstd::vector<String> args;
            args.push_back(on);
            args.push_back(off);
            return args;
        }
        return twnstd::vector<String>();
    }
};

Light onligth;

void printStatus(const twnstd::vector<String>& params)
{
  LOG_INFO("motion %s", sensorValue ? "true" : "false");
  LOG_INFO("rele %s", releStatus ? "true" : "false");
  LOG_INFO("analog value %d", analogValue);
}

void usersetup() {  
  LOG_INFO("STARTUP");

  LOG_INFO("pin set");
  pinMode(13, INPUT);
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  
  CommandLine::GetInstance().AddCommand(CreateCommand(&onligth, "ligth"));
  CommandLine::GetInstance().AddCommand(CreateCommand(&printStatus, "status"));
}

void mainloop() {
    analogValue = analogRead(A0);
    int sensorval = digitalRead(13);
    if(sensorval == HIGH && !sensorValue) {
      LOG_INFO(motion);
      sensorValue = true;
    } else if(sensorval == LOW && sensorValue){
      LOG_INFO(noMotion);
      sensorValue = false;
    }
  
    unsigned long currentTime = micros();
    if(releStatus){
      if(!sensorValue) {
        if(currentTime - ligthTimeStart > 15000000){
          digitalWrite(12, LOW);
          LOG_INFO(ligthOff);
          releStatus = false;
        }
      } else {
        ligthTimeStart = currentTime;
      }
    } else if(!releStatus) {
      bool releResult = (analogValue < 150 && sensorValue);
      if(releResult) {
        digitalWrite(12, HIGH);
        LOG_INFO(ligthOn);
        releStatus = true;
        ligthTimeStart = currentTime;
      }
    }
}

