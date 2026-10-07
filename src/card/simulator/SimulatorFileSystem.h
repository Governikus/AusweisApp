/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "SmartCardDefinitions.h"
#include "apdu/ResponseApdu.h"
#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/CVCertificate.h"
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
		QMap<PacePasswordId, QByteArray> mPasswords;
		QMap<int, QByteArray> mKeys;
		QMap<QByteArray, QByteArray> mFiles;
		QMap<QByteArray, QByteArray> mFileIds;
		QSharedPointer<const CVCertificate> mTrustPoint;

		void initMandatoryData();
		void parseKey(const QJsonObject& pKey);

	public:
		SimulatorFileSystem();
		explicit SimulatorFileSystem(const QJsonObject& pData);

		[[nodiscard]] StatusCode select(const QByteArray& pFileId);
		[[nodiscard]] QByteArray read(qsizetype pOffset, int pLength, bool pExtendedLen) const;
		[[nodiscard]] StatusCode write(qsizetype pOffset, const QByteArray& pData);

		[[nodiscard]] QByteArray getEfCardAccess() const;
		[[nodiscard]] QByteArray getPassword(PacePasswordId pPasswordId) const;
		[[nodiscard]] QByteArray getKey(int pKeyId) const;
		[[nodiscard]] QSharedPointer<const CVCertificate> getTrustPoint() const;
		void setTrustPoint(const QSharedPointer<const CVCertificate>& pTrustPoint);

		[[nodiscard]] StatusCode verify(const Oid& pOid, const QSharedPointer<AuthenticatedAuxiliaryData>& pAuxiliaryData) const;

	private:
		void createFile(const QByteArray& pFileId, const QByteArray& pShortFileId, const QByteArray& pContent);
		void createFile(const QByteArray& pShortFileId, const QByteArray& pContent);
		void createFile(const QByteArray& pShortFileId, const char* pStr, const QByteArray& pConfig = QByteArray());
		void createFile(const QByteArray& pShortFileId, const char* pStr, const QString& pFile);
};

} // namespace governikus
