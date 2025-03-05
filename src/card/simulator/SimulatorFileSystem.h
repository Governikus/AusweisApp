/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
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
#include <openssl/ec.h>


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
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		[[nodiscard]] QSharedPointer<EVP_PKEY> getKey(int pKeyId) const;
#else
		[[nodiscard]] QSharedPointer<EC_KEY> getKey(int pKeyId) const;
#endif
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
