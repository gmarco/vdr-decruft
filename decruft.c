/*
 * decruft.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: decruft.c,v 1.2 2005/02/20 17:30:16 dom Exp $
 *
 * TODO: Configuration
 * TODO: Option to scan on demand
 */

#include <vdr/plugin.h>
#include "cleanthread.h"
#include "cruft.h"

static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = "Remove the cruft from your channels";
static const char *MAINMENUENTRY  = "Decruft";

class cPluginDecruft : public cPlugin {
private:
  // Add any member variables or functions you may need here.
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
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginDecruft::cPluginDecruft(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
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
  return true;
}

void cPluginDecruft::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginDecruft::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginDecruft::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginDecruft::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginDecruft::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

VDRPLUGINCREATOR(cPluginDecruft); // Don't touch this!
