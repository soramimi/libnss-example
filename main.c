/*  
 *  Copyright (C) 2019 Shinichi Fuchita (@soramimi_jp)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  This file incorporates some code from these modules:
 *    nss-mdns, (C) 2004 Lennart Poettering.
 *    nss-gw-name, (C) 2010 Joachim Breitner.
 *    nss-openvpn, (C) 2014 Gonéri Le Bouder
 */

#include <string.h>
#include <nss.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

#define ALIGN(idx) do { \
	if (idx % sizeof(void*)) \
	idx += (sizeof(void*) - idx % sizeof(void*)); /* Align on 32 bit boundary */ \
	} while(0)

enum nss_status _nss_example_gethostbyname_r(const char *name, struct hostent *result, char *buffer, size_t buflen, int *errnop, int *h_errnop)
{
	size_t idx, astart;

	if (strcmp(name, "example") == 0) {
		char const *host = "127.0.0.1";

		*(char**)buffer = NULL;
		result->h_aliases = (char**) buffer;
		idx = sizeof(char*);

		strcpy(buffer + idx, name);
		result->h_name = buffer + idx;
		idx += strlen(name) + 1;
		ALIGN(idx);

		result->h_addrtype = AF_INET;
		result->h_length = sizeof(uint32_t);

		struct in_addr addr;

		inet_pton(AF_INET, host, &addr);

		astart = idx;
		memcpy(buffer+astart, &addr.s_addr, sizeof(uint32_t));
		idx += sizeof(uint32_t);

		result->h_addr_list = (char**)(buffer + idx);
		result->h_addr_list[0] = buffer + astart;
		result->h_addr_list[1] = NULL;

		return NSS_STATUS_SUCCESS;
	}

	*errnop = EINVAL;
	*h_errnop = NO_RECOVERY;
	return NSS_STATUS_UNAVAIL;

}

enum nss_status _nss_example_gethostbyname2_r(const char *name, int af, struct hostent *result, char *buffer, size_t buflen, int *errnop, int *h_errnop)
{
	if (af != AF_INET) {
		*errnop = EAGAIN;
		*h_errnop = NO_RECOVERY;
		return NSS_STATUS_TRYAGAIN;
	} else {
		return _nss_example_gethostbyname_r(name, result, buffer, buflen, errnop, h_errnop);
	}
}

