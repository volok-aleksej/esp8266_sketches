#include <Ticker.h>
#include <twainet.h>
#include <logger.h>

bool relestatus = false, releLogSt = false;
unsigned int timerValue = 0;

Ticker m_resetTimer;
Ticker m_sysTimer;

void onTimerSecond() {
  timerValue++;
  //set rele every 24 hours
  if(timerValue == 86400) {
    pullRele(twnstd::vector<String>());
    timerValue = 0;
  }
}

void resetRele() {
  if(relestatus) {
    relestatus = false;
    digitalWrite(D5, LOW);
  }
}

void pullRele(const twnstd::vector<String>& params) {
  if(!relestatus) {
      relestatus = true;
      digitalWrite(D5, HIGH);
      m_resetTimer.once(2, &resetRele);
  }
}

void usersetup() {  
  LOG_INFO("STARTUP");
  m_sysTimer.attach(1, &onTimerSecond);

  LOG_INFO("pin set");
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  
  CommandLine::GetInstance().AddCommand(CreateCommand(&pullRele, "pull"));
}

void mainloop() {
  if(releLogSt != relestatus) {
    if(releLogSt)
      LOG_INFO("rele reset");
    else
      LOG_INFO("rele set");
    releLogSt = relestatus;
  }
}
