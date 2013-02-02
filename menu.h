#ifndef SETUPMENU_H
#define SETUPMENU_H



#include <vdr/plugin.h>
#include <string>



class cDecruftSetup
{
public:
    cDecruftSetup();
    ~cDecruftSetup();
    int cleanEvery;
};



class cSetupMenu : public cMenuSetupPage
{
public:
    cSetupMenu();
    ~cSetupMenu();
private:
    cDecruftSetup m_tmpSetup;
protected:
    virtual void Store();
};



extern cDecruftSetup g_DecruftSetup;



#endif
