/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#ifndef AUSWEISAPP_HEADER
#define AUSWEISAPP_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef void (* AusweisAppCallback)(const char* pMsg);

bool ausweisapp_init(AusweisAppCallback pCallback, const char* pCmdline);
void ausweisapp_shutdown(void);
bool ausweisapp_is_running(void);
void ausweisapp_send(const char* pCmd);


#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) || (defined(__cplusplus) && __cplusplus >= 201402L) || defined(__clang__) || defined(__GNUC__)
	#define DEPRECATED [[deprecated]]
#else
	#define DEPRECATED
#endif

typedef AusweisAppCallback AusweisApp2Callback;

DEPRECATED bool ausweisapp2_init(AusweisApp2Callback pCallback, const char* pCmdline);
DEPRECATED void ausweisapp2_shutdown(void);
DEPRECATED bool ausweisapp2_is_running(void);
DEPRECATED void ausweisapp2_send(const char* pCmd);

#ifdef __cplusplus
}
#endif

#endif // AUSWEISAPP_HEADER
