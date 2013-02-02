#include "menu.h"



using namespace std;



cDecruftSetup g_DecruftSetup;



cDecruftSetup::cDecruftSetup()
{
               cleanEvery=12;
}



cDecruftSetup::~cDecruftSetup()
{}



cSetupMenu::cSetupMenu() : m_tmpSetup(g_DecruftSetup)
{
    Add(new cMenuEditIntItem(tr("Clean at earliest after (in Hours)"), &m_tmpSetup.cleanEvery,0,1440));
}



cSetupMenu::~cSetupMenu()
{}



void cSetupMenu::Store()
{
    g_DecruftSetup = m_tmpSetup;
    SetupStore("CleanEvery", g_DecruftSetup.cleanEvery);
}
