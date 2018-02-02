/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 *
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

#include <assert.h>
#include <string.h>

#include <microhttpd.h>

#include "App.h"
#include "api/Api.h"
#include "api/ApiState.h"
#include "log/Log.h"
#include "net/Network.h"


ApiState *Api::m_state = nullptr;
uv_mutex_t Api::m_mutex;


bool Api::start()
{
    uv_mutex_init(&m_mutex);
    m_state = new ApiState();

    return true;
}


void Api::release()
{
    delete m_state;
}


char *Api::get(const char *url, int *status)
{
    if (!m_state) {
        return nullptr;
    }

    uv_mutex_lock(&m_mutex);
    char *buf = m_state->get(url, status);
    uv_mutex_unlock(&m_mutex);

    return buf;
}


char *Api::switchPool(const char *url, int *status, int next)
{
    assert(nullptr != url && nullptr != status);
    assert(next >= 0);

    uv_mutex_lock(&m_mutex);
    App* app = App::i();
    if (nullptr == app) {
        LOG_ERR("Failed to retrieve App object.");
        *status = MHD_HTTP_BAD_REQUEST;
        return nullptr;
    }
    Network* network = App::i()->network();
    if (nullptr == network) {
        LOG_ERR("Failed to retrieve Network object.");
        *status = MHD_HTTP_BAD_REQUEST;
        return nullptr;
    }

    network->switchPool((size_t) next);
    uv_mutex_unlock(&m_mutex);

    char resp[4096];
    snprintf(resp, sizeof(resp), "{\n\t\"status\": \"OK\",\n\t\"next\": %d\n}\n", next);

    return strdup(resp);
}


void Api::tick(const Hashrate *hashrate)
{
    if (!m_state) {
        return;
    }

    uv_mutex_lock(&m_mutex);
    m_state->tick(hashrate);
    uv_mutex_unlock(&m_mutex);
}


void Api::tick(const NetworkState &network)
{
    if (!m_state) {
        return;
    }

    uv_mutex_lock(&m_mutex);
    m_state->tick(network);
    uv_mutex_unlock(&m_mutex);
}


