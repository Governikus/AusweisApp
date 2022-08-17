/*!
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include <eid_applet_interface.h>

namespace governikus
{
void setEphemeralResult(bool pEphemeral = true);
void setUpdateInfo(EidUpdateInfo pStatus);
void setSmartEidStatus(EidStatus pStatus);
void setInstallSmartEidResult(EidServiceResult pResult);
void setDeleteSmartEidResult(EidServiceResult pResult);
void setApduCommandResult(const GenericDataResult& pResult);
void setPersonalizationResult(const GenericDataResult& pResult);
void setInitializePersonalizationResult(const InitializeResult& pResult);
void setDeletePersonalizationResult(EidServiceResult pResult);

void initMock();
} // namespace governikus
