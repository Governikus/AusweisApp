/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include <eid_applet_interface.h>

#include <QString>

namespace governikus
{
void setEphemeralResult(bool pEphemeral = true);
void setSmartEidSupportStatus(EidSupportStatus pStatus);
void setSmartEidSupportStatusResult(EidSupportStatusResult pStatus);
void setSmartEidStatus(EidStatus pStatus);
void setServiceInformation(const ServiceInformationResult& pResult);
void setInstallSmartEidResult(EidServiceResult pResult);
void setDeleteSmartEidResult(EidServiceResult pResult);
void setApduCommandResult(const GenericDataResult& pResult);
void setPersonalizationResult(const GenericDataResult& pResult);
void setInitializePersonalizationResult(const InitializeResult& pResult);
void setDeletePersonalizationResult(EidServiceResult pResult);

void initMock();
QString dequeueReceivedParameter();
} // namespace governikus
