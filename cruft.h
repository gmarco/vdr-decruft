
#ifndef CRUFT_CRUFT_H
#define CRUFT_CRUFT_H

#include <vdr/channels.h>

extern int      parse_file(const char *filename);
extern bool     CheckChannel(cChannel *channel);
extern bool     CheckChannelMove(cChannel *channel);

#endif // CRUFT_CRUFT_H
