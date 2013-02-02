
#ifndef CRUFT_CLEAN_H
#define CRUFT_CLEAN_H


#include <vdr/thread.h>
#include "cruft.h"


extern void startDecruft();

class cCruftCleanThread : public cThread {
private:
    bool   m_Active;
    time_t nextUpdate;
protected:
    virtual void Action(void);
    void Stop(void);
    bool NeedUpdate();
public:
    cCruftCleanThread(void);
    virtual ~cCruftCleanThread();
    void Trigger();
    static void Init(void);
    static void Exit(void);
};

#endif // CRUFT_CLEAN_H
