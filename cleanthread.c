/*
 * decruft: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 *
 * Main thread - based on the epgsearch timer thread code
 *
 * TODO: Trigger a rescan 
 */


#include <vdr/channels.h>
#include <vdr/timers.h>
#include "cleanthread.h"
#include "cruft.h"


void sleepMSec(long ms)
{
#if VDRVERSNUM >= 10314
    cCondWait::SleepMs(ms);
#else
    usleep(1000 * ms);
#endif
}

void sleepSec(long s)
{
    sleepMSec(s * 1000);
}


cCruftCleanThread *cCruftCleanThread::m_Instance = NULL;

cCruftCleanThread::cCruftCleanThread(void)
    : cThread("Decruft: cleaning")
{
    m_Active = false;
    m_lastUpdate = time(NULL);
}

cCruftCleanThread::~cCruftCleanThread()
{
    if (m_Active )
        Stop();
}

void cCruftCleanThread::Init(void)
{
    if ( m_Instance == NULL ) {
        m_Instance = new cCruftCleanThread;
        m_Instance->Start();
    }
}

void cCruftCleanThread::Exit(void)
{
    if (m_Instance != NULL ) {
        m_Instance->Stop();
        DELETENULL(m_Instance);
    }
}

void cCruftCleanThread::Stop(void)
{
    m_Active = false;
    Cancel(3);
}

bool cCruftCleanThread::NeedUpdate(void)
{
    return false;	// fixme check config file times
}

void cCruftCleanThread::Action(void)
{
    m_Active = true;

    // let VDR do its startup
    sleepSec(15);
    time_t nextUpdate = time(NULL);

    while (m_Active ) {
        time_t now = time(NULL);
        if (now >= nextUpdate || NeedUpdate()) {
            if ( Channels.BeingEdited() ) {
		printf("Channels are being edited");
                sleepSec(1);
                continue;
            }

	printf("Cleaning\n");
            /* Grab the channels lock */
            if (!Channels.Lock(true,10)) {
		printf("Can't lock channels\n");
                continue;
            }

            for ( cChannel *channel = Channels.First(); channel ; ) {
                cChannel *next = Channels.Next(channel);

                /* If it's not a separator then process it */
                if ( !channel->GroupSep() ) {
                    if ( CheckChannel(channel) == false ) {
                        /* Check for active timers */
                        bool del = true;
                        for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti)) {
                            if (ti->Channel() == channel) {
                                del = false;
                            }
                        }
                        if ( del ) {
                            esyslog("Deleting channel <%s;%s>\n",channel->Name(),channel->Provider());
                            Channels.Del(channel);
                            Channels.ReNumber();
                        } else {
                            /* We might hit the same channels again, but the
                               second time they won't be moved...nasty 
                            */
                            CheckChannelMove(channel);
                        }

                    } else {
			CheckChannelMove(channel);
		    }
                }
                channel = next;
            }
            /* And unlock them */
            Channels.SetModified(true);
            Channels.Unlock();
            m_lastUpdate = time(NULL);
            nextUpdate = m_lastUpdate + 600; /* FIXME: run every 10 minutes */
        }
        sleepSec(5);
    }
    dsyslog("Decruft: Leaving cleaning thread");
}
                
