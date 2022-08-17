/*!
 * \brief Parses TCTokens from XML data and provides its content.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QUrl>
#include <QXmlStreamReader>

class test_TcToken;

namespace governikus
{

class TcToken
{
	Q_DISABLE_COPY(TcToken)
	friend class ::test_TcToken;

	private:
		bool mSchemaConform;
		QString mBinding;
		QString mPathSecurityProtocol;
		QByteArray mPsk;
		QByteArray mSessionIdentifier;
		QUrl mServerAddress;
		QUrl mCommunicationErrorAddress;
		QUrl mRefreshAddress;

		void parse(const QByteArray& pData);
		[[nodiscard]] bool valuesAreSchemaConform(const QString& pBinding,
				const QString& pPathSecurityProtocol,
				const QByteArray& pPsk,
				const QByteArray& pSessionIdentifier,
				const QString& pServerAddress,
				const QString& pCommunicationErrorAddress,
				const QString& pRefreshAddress) const;
		[[nodiscard]] bool isAnyUri(const QString& pCandidate) const;
		[[nodiscard]] bool isHexBinary(const QString& pCandidate) const;
		QString readElementValue(QXmlStreamReader& pReader);

	public:
		explicit TcToken(const QByteArray& pData);
		virtual ~TcToken();

		/**
		 * Check if parsing of given data was successful.
		 */
		[[nodiscard]] bool isValid() const;

		[[nodiscard]] const QString& getBinding() const;
		[[nodiscard]] const QByteArray& getSessionIdentifier() const;
		[[nodiscard]] const QUrl& getServerAddress() const;
		[[nodiscard]] const QUrl& getRefreshAddress() const;
		[[nodiscard]] const QUrl& getCommunicationErrorAddress() const;
		[[nodiscard]] bool usePsk() const;
		[[nodiscard]] const QByteArray& getPsk() const;

		[[nodiscard]] bool isSchemaConform() const
		{
			return mSchemaConform;
		}


};

} // namespace governikus
