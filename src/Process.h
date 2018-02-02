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


#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <sys/types.h>
#include <unistd.h>
#include <uuid/uuid.h>


class Process
{
public:
    Process(const char* progname);
    virtual ~Process();

    pid_t pid() const                   { return m_pid; }
    const char* pidFilename() const     { return m_pidfn; }

    const uuid_t* uuid() const             { return &m_uuid; }

private:
    pid_t m_pid;
    char* m_progname;
    char* m_pidfn;

    uuid_t m_uuid;
};


#endif /* __PROCESS_H__ */
