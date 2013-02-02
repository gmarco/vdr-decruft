/*
 * decruft.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 *
 * TODO: Reloading of config
 */


#include <vdr/channels.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>

typedef struct {
    int       num_cas;
    int      *cas;
    int       num_vpids;
    int      *vpids;
    int       num_apids;
    int      *apids;
    int       num_dpids;
    int      *dpids;
    int       num_tpids;
    int      *tpids;
    int       num_provs;
    char    **provs;
    regex_t **provs_regex;
    int       num_names;
    char    **names;
    regex_t **names_regex;
    int       num_sources;
    char    **sources;
} setting_t;


static int          num_clean = 0;
static setting_t  **clean     = NULL;
static int          num_keep  = 0;
static setting_t  **keep      = NULL;

/* Cleanup a settings_t */
void free_settings(setting_t *s)
{
    int   i;
    if ( s->cas ) 
        free(s->cas);
    if ( s->vpids )
        free(s->vpids);
    if ( s->apids )
        free(s->apids);
    if ( s->dpids )
        free(s->dpids);
    if ( s->tpids )
        free(s->tpids);
    for ( i = 0; i < s->num_provs; i++ ) {
        free(s->provs[i]);
        if ( s->provs_regex[i] ) {
                regfree(s->provs_regex[i]);
                free(s->provs_regex[i]);
        }
    }
    if ( s->provs ) {
        free(s->provs);
        free(s->provs_regex);
    }
    for ( i = 0; i < s->num_names; i++ ) {
        free(s->names[i]);
        if ( s->names_regex[i] ) {
            if ( s->names_regex[i] ) {
                regfree(s->names_regex[i]);
                free(s->names_regex[i]);
            }
        }
    }
    if ( s->names ) {
        free(s->names);
        free(s->names_regex);
    }
    for ( i = 0; i < s->num_sources; i++ ) {
        free(s->sources[i]);
    }
    if ( s->sources ) {
        free(s->sources);
    }
    free(s);
}


/* TODO: This parsing code (and file format) sucks, it really needs to
 *  be fixed, but it...I guess it does the job...
 */
static char *check_arg(char *ptr, char *match)
{
    while ( isspace(*ptr) )
        ptr++;
    if ( strncmp(ptr,match,strlen(match)) ) {
        return NULL;
    }
    ptr += strlen(match);
    while ( isspace(*ptr) )
        ptr++;
    return ptr;
}

static void read_ints(char *line, int *count, int **dest)
{
    char   tempbuf[10];
    int    num;
    int    pos = 0;

    while (*line && *line != ';' ) {
        if (*line == ',' || !isxdigit(*line) ) {
            tempbuf[pos] = 0;
            if ( pos == 0 ) {
                line++;
                continue;
            }
            if ( sscanf(tempbuf,"%x",&num) == 1 ) {
                *dest = (int *)realloc(*dest, (*count + 1) * sizeof(int) );
                (*dest)[*count] = num;
                (*count)++;
            }
            line++;
            pos = 0;
            continue;
        } else {
            tempbuf[pos++] = *line;
            line++;
        }
    }
    /* Pick up the last one */
    if ( pos != 0 ) {
        tempbuf[pos] = 0;
        if ( sscanf(tempbuf,"%x",&num) == 1 ) {
            *dest = (int *)realloc(*dest, (*count + 1) * sizeof(int) );
            (*dest)[*count] = num;
            (*count)++;
        }
    }
}

static int parse_line(setting_t *settings, char *line)
{
    char   *ptr = line;
    char   *temp;
    char   *end;
    int     i;


    while ( isspace(*ptr) )
        ptr++;

    while ( *ptr ) {
        if ( ( temp = check_arg(ptr,"ca=") ) != NULL ) {
            read_ints(temp,&settings->num_cas,&settings->cas);
        } else if ( (temp = check_arg(ptr,"provider=") ) != NULL ) {
            if ( (end = strchr(temp,';')) || (end=strchr(temp,'\n') ) ) {
                if ( *end == '\n' ) {
                    ptr = end + 1;
                } else {
                    ptr = end;
                }
                *end = 0;
                i = settings->num_provs++;
                settings->provs = (char **)realloc(settings->provs, settings->num_provs * sizeof(char *));
                settings->provs_regex = (regex_t **)realloc(settings->provs_regex, settings->num_provs * sizeof(char *));
                settings->provs[i] = strdup(temp);
                /* Compile the regex */
                settings->provs_regex[i] = (regex_t *)calloc(1,sizeof(regex_t));
                if ( regcomp(settings->provs_regex[i], temp, REG_EXTENDED) != 0 ) {
                    free(settings->provs_regex[i]);
                    settings->provs_regex[i] = NULL;
                }
            } else {
                /* Hmmmm, fail gracefully */
            }
        } else if ( (temp = check_arg(ptr,"name=") ) != NULL ) {
            if ( (end = strchr(temp,';')) || (end=strchr(temp,'\n') ) ) {
                if ( *end == '\n' ) {
                    ptr = end + 1;
                } else {
                    ptr = end;
                }
                *end = 0;
                i = settings->num_names++;
                settings->names = (char **)realloc(settings->names, settings->num_names * sizeof(char *));
                settings->names_regex = (regex_t **)realloc(settings->names_regex, settings->num_names * sizeof(char *));
                settings->names[i] = strdup(temp);
                /* Compile the regex */
                settings->names_regex[i] = (regex_t *)calloc(1,sizeof(regex_t));
                if ( regcomp(settings->names_regex[i], temp, REG_EXTENDED) != 0 ) {
                    free(settings->names_regex[i]);
                    settings->names_regex[i] = NULL;
                }
            } else {
                /* Hmmmm, fail gracefully */
            }
        } else if ( (temp = check_arg(ptr,"source=") ) != NULL ) {
            if ( (end = strchr(temp,';')) || (end=strchr(temp,'\n') ) ) {
                if ( *end == '\n' ) {
                    ptr = end + 1;
                } else {
                    ptr = end;
                }
                *end = 0;
                i = settings->num_sources++;
                settings->sources = (char **)realloc(settings->sources, settings->num_sources * sizeof(char *));
                settings->sources[i] = strdup(temp);
            } else {
                /* Hmmmm, fail gracefully */
            }
        } else if ( (temp = check_arg(ptr,"vpid=") ) != NULL ) {
            read_ints(temp,&settings->num_vpids,&settings->vpids);
        } else if ( (temp = check_arg(ptr,"apid=") ) != NULL ) {
            read_ints(temp,&settings->num_apids,&settings->apids);
        } else if ( (temp = check_arg(ptr,"dpid=") ) != NULL ) {
            read_ints(temp,&settings->num_apids,&settings->apids);
        } else if ( (temp = check_arg(ptr,"tpid=") ) != NULL ) {
            read_ints(temp,&settings->num_dpids,&settings->dpids);
        } else {
            temp = ptr + 1;
        }
        ptr = temp;
    }
    return 1;
}


int parse_file(const char *filename)
{
    char          buf[1024];
    FILE         *fp;
    setting_t    *setting;
    char         *ptr;
    int           i;

    if ( ( fp = fopen(filename,"r")) == NULL ) {
        return -1;
    }

    while ( fgets(buf,sizeof(buf),fp) != NULL ) {
        if ( buf[0] == '#' || buf[0] == ';' ) {
            continue;
        }
        if ( strncmp(buf,"clean:",strlen("clean:")) == 0) {
            ptr = buf + strlen("clean:") + 1;
            setting = (setting_t *)calloc(1,sizeof(*setting));
            if ( parse_line(setting,ptr) ) {
                i = num_clean++;
                clean = (setting_t **)realloc(clean, num_clean * sizeof(setting_t *));
                clean[i] = setting;
            } else {
                free(setting);
            }
        } else if ( strncmp(buf,"keep:",strlen("keep:")) == 0 ) {
            ptr = buf + strlen("keep:") + 1;
            setting = (setting_t *)calloc(1,sizeof(*setting));
            if ( parse_line(setting,ptr) ) {
                i = num_keep++;
                keep = (setting_t **)realloc(keep, num_keep * sizeof(setting_t *));
                keep[i] = setting;
            } else {
                free(setting);
            }
        }
    }
    return 1;
}

/** \brief Check a channel against an individual settings
 *
 *  \param channel
 *  \param setting
 *
 *  \retval true - matched
 *  \retval false - not matched
 */
static bool CheckSettings(cChannel *Channel, setting_t *settings)
{
    int     tests = 0;
    int     match = 0;
    int     i,j;
    /* Check CAs */

    if ( settings->num_cas ) {
        tests++;
        for ( j = 0; j < settings->num_cas; j++ ) {
            for ( i = 0; i < MAXCAIDS; i++ ) {
                int ca = Channel->Ca();
                if ( ca == settings->cas[j] ) {
                    match++;
                    goto exitca;
                }
            }
        }
    exitca:
        ;
    }
    if ( settings->num_vpids ) {
        tests++;
        for ( i = 0; i < settings->num_vpids; i++ ) {
            if ( Channel->Vpid() == settings->vpids[i] ) {
                match++;
                break;
            }
        }
    }
    if ( settings->num_apids ) {
        tests++;
        /* We only really want to check for 0 as the first pid */
        for ( j = 0; j < settings->num_apids; j++ ) {
            if ( settings->apids[j] == Channel->Apid(0) ) {
                match++;
                break;
            }
        }
    }
    if ( settings->num_dpids ) {
        tests++;
        /* We only really want to check for 0 as the first pid */
        for ( j = 0; j < settings->num_dpids; j++ ) {
            if ( settings->dpids[j] == Channel->Apid(0) ) {
                match++;
                break;
            }
        }
    }
    if ( settings->num_tpids ) {
        tests++;
        /* We only really want to check for 0 as the first pid */
        for ( j = 0; j < settings->num_tpids; j++ ) {
            if ( settings->tpids[j] == Channel->Tpid() ) {
                match++;
                break;
            }
        }
    }

    if ( settings->num_provs ) {
        tests++;
        for ( j = 0; j < settings->num_provs; j++ ) {
            if ( settings->provs_regex[j] ) {
                if ( regexec(settings->provs_regex[j],Channel->Provider(),0,NULL,0) == 0 ) {
                    match++;
                    break;
                }
            }
        }
    }

   if ( settings->num_names ) {
        tests++;
        for ( j = 0; j < settings->num_names; j++ ) {
            if ( settings->names_regex[j] ) {
                if ( regexec(settings->names_regex[j],Channel->Name(),0,NULL,0) == 0 ) {
                    match++;
                    break;
                }
            }
        }
    }

   if ( settings->num_sources ) {
       char *source = strdup(*cSource::ToString(Channel->Source()));
       tests++;
       for ( j = 0; j < settings->num_sources; j++ ) {
           if ( strcmp(source,settings->sources[j]) == 0 ) {
               match++;
               break;
           }
       }
       free(source);
   }


   if ( tests == match && tests != 0 ) {
       return true;
   }
   return false;
}

/** \brief See whether a channel should be kept or not
 *
 *  \param channel The incoming channel
 *
 *  \retval false - The channel should be removed
 *  \retval true  - The channel should be retained
 */
bool CheckChannel(cChannel *channel)
{
    bool          res = true;  /* Default to keeping it (be safe) */
    int           i;
	
    printf("Checking channel <%s> Source <%s>\n",channel->Name(), *cSource::ToString(channel->Source()));

    for ( i = 0; i < num_clean; i++ ) {
        if ( CheckSettings(channel,clean[i]) ) {
            res = false;
            break;
        }
    }

    if ( res == false ) {
        for ( i = 0; i < num_keep; i++ ) {
            if ( CheckSettings(channel,keep[i] ) ) {
                res = true;
                break;
            }
        }
    }
    return res;
}
