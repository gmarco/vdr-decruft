
#ifndef CRUFT_CLEAN_H
#define CRUFT_CLEAN_H


#include <vdr/thread.h>
#include "cruft.h"


class cCruftCleanThread : public cThread {
private:
    bool   m_Active;
    static cCruftCleanThread *m_Instance;
    time_t m_lastUpdate;
protected:
    virtual void Action(void);
    void Stop(void);
    bool NeedUpdate();
public:
    cCruftCleanThread(void);
    virtual ~cCruftCleanThread();
    static void Init(void);
    static void Exit(void);
};

#endif // CRUFT_CLEAN_H
