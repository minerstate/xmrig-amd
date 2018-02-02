/* XMRig
 * Copyright 2018   MinerState
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>

#include "log/Log.h"
#include "Process.h"


Process::Process(const char* progname) :
    m_pid(0),
    m_progname(strdup(progname)),
    m_pidfn(nullptr)
{
    m_pid = getpid();

    const char* sitedir = getenv("SITE_DIR");
    if (!sitedir) {
        sitedir = "/tmp";
    }

    char tmpfile[PATH_MAX];
    snprintf(tmpfile, sizeof(tmpfile), "%s/%s.pid", sitedir, progname);
    m_pidfn = strdup(tmpfile);

    FILE* fp = nullptr;

    fp = fopen(m_pidfn, "w");
    if (fp) {
        fprintf(fp, "%d\n", m_pid);
        fclose(fp);
    }

    snprintf(tmpfile, sizeof(tmpfile), "%s/.%s.instance", getenv("HOME"), progname);

    bool uuid_read = false;
    char uuid_s[48];

    fp = fopen(tmpfile, "r");
    if (fp) {
        char* line = nullptr;
        size_t len = 0;
        if (getline(&line, &len, fp) != -1) {
            line[36] = 0;
            if (uuid_parse(line, m_uuid)) {
                LOG_ERR("Failed to parse uuid %s.", line);
            } else {
                uuid_read = true;
            }
        }
        free(line);
        fclose(fp); fp = nullptr;
    }
    
    if (!uuid_read) {
        fp = fopen(tmpfile, "w");
        if (fp) {
            uuid_generate(m_uuid);
            uuid_unparse(m_uuid, uuid_s);
            fprintf(fp, "%s\n", uuid_s);
            fclose(fp);
        }
    }

    uuid_unparse(m_uuid, uuid_s);

    LOG_INFO("%s instance id - %s", progname, uuid_s);
}

Process::~Process()
{
    remove(m_pidfn);

    free(m_progname);
    free(m_pidfn);
}

