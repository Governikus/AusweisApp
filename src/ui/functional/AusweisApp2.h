/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#ifndef AUSWEISAPP2_HEADER
#define AUSWEISAPP2_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef void (* AusweisApp2Callback)(const char* pMsg);

bool ausweisapp2_init(AusweisApp2Callback pCallback, const char* pCmdline);
void ausweisapp2_shutdown(void);
bool ausweisapp2_is_running(void);
void ausweisapp2_send(const char* pCmd);

#ifdef __cplusplus
}
#endif

#endif // AUSWEISAPP2_HEADER
