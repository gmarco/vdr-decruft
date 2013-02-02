/*
 * decruft.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: decruft.c,v 1.3 2005/03/11 18:49:11 dom Exp $
 *
 * TODO: Configuration
 * TODO: Option to scan on demand (0.0.4)
 */

#include <vdr/plugin.h>
#include "cleanthread.h"
#include "menu.h"
#include <time.h>
#include "cruft.h"

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = "Remove the cruft from your channels";
static const char *MAINMENUENTRY  = "Decruft";

class cPluginDecruft : public cPlugin {
private:
  // Add any member variables or functions you may need here.
       time_t last_decruft;
public:
  cPluginDecruft(void);
  virtual ~cPluginDecruft();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
       virtual const char **SVDRPHelpPages(void);
       virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginDecruft::cPluginDecruft(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
       g_DecruftSetup.cleanEvery=12;
}

cPluginDecruft::~cPluginDecruft()
{
  // Clean up after yourself!
}

const char *cPluginDecruft::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginDecruft::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginDecruft::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginDecruft::Start(void)
{
  // Start any background activities the plugin shall perform.
  parse_file(AddDirectory(cPlugin::ConfigDirectory(), "decruft.conf"));
  cCruftCleanThread::Init();
  time(&last_decruft);
  return true;
}

void cPluginDecruft::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginDecruft::Housekeeping(void)
{
       time_t now;
       time(&now);
       esyslog("housekeeping decruft %d %d %d",now,last_decruft,now-last_decruft);
       if (now-last_decruft > g_DecruftSetup.cleanEvery * 3600){ // every 12 std
               last_decruft = now;
               startDecruft();
       }

  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginDecruft::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  startDecruft();
  return NULL;
}

cMenuSetupPage *cPluginDecruft::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
       return new cSetupMenu();
}

bool cPluginDecruft::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
       if      (!strcasecmp(Name, "CleanEvery"))  g_DecruftSetup.cleanEvery = atoi(Value);
       else return false;
       return true;

}
const char **cPluginDecruft::SVDRPHelpPages(void)
{
    // Return help text for SVDRP commands this plugin implements
   static const char *HelpPage[] =
    {
        "CLEAN\n"
        "     Clean now channels.conf.",
        NULL
    };
    return HelpPage;
    return NULL;
}



cString cPluginDecruft::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
    // Process SVDRP commands this plugin implements
               if (!strcasecmp(Command,"CLEAN")){
                       startDecruft();
                       return tr("decruft cleaning channels.conf ...");
               }
    return NULL;
}


VDRPLUGINCREATOR(cPluginDecruft); // Don't touch this!
