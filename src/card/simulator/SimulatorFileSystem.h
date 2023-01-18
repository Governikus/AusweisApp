/*!
 * \brief Implementation of Simulator.
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "apdu/ResponseApdu.h"
#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/Oid.h"

#include <QByteArray>
#include <QJsonObject>
#include <QMap>


namespace governikus
{


class SimulatorFileSystem
{
	private:
		QByteArray mSelectedFile;
		QMap<QByteArray, QByteArray> mFiles;
		QMap<QByteArray, QByteArray> mFileIds;

	public:
		SimulatorFileSystem();
		explicit SimulatorFileSystem(const QJsonObject& pData);

		[[nodiscard]] StatusCode select(const QByteArray& pFileId);
		[[nodiscard]] QByteArray read(int pOffset, int pLength, bool pExtendedLen);
		[[nodiscard]] StatusCode write(int pOffset, const QByteArray& pData);

		[[nodiscard]] QByteArray getEfCardAccess() const;
		[[nodiscard]] QByteArray getCardAuthenticationKey() const;
		[[nodiscard]] QByteArray getRestrictedIdentificationKey(int pKeyId) const;

		[[nodiscard]] StatusCode verify(const Oid& pOid, const QSharedPointer<AuthenticatedAuxiliaryData>& pAuxiliaryData);

	private:
		void createFile(const QByteArray& pFileId, const QByteArray& pShortFileId, const QByteArray& pContent);
		void createFile(const QByteArray& pShortFileId, const QByteArray& pContent);
		void createFile(const QByteArray& pShortFileId, const char* pStr, const QByteArray& pConfig = QByteArray());
		void createFile(const QByteArray& pShortFileId, const char* pStr, const QString& pFile);
};

} // namespace governikus
