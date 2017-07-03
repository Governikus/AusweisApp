/*!
 * TcToken.h
 *
 * \brief Parses TCTokens from XML data and provides its content.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QUrl>
#include <QXmlStreamReader>

namespace governikus
{

class TcToken
{
	private:
		bool mSchemaConform;
		QString mBinding;
		QString mPathSecurityProtocol;
		QByteArray mPsk;
		QByteArray mSessionIdentifier;
		QUrl mServerAddress;
		QUrl mCommunicationErrorAddress;
		QUrl mRefreshAddress;

		Q_DISABLE_COPY(TcToken)
		void parse(const QByteArray& pData);
		bool valuesAreSchemaConform(const QString& pBinding,
				const QString& pPathSecurityProtocol,
				const QByteArray& pPsk,
				const QByteArray& pSessionIdentifier,
				const QString& pServerAddress,
				const QString& pCommunicationErrorAddress,
				const QString& pRefreshAddress) const;
		bool isAnyUri(const QString& pCandidate) const;
		bool isHexBinary(const QString& pCandidate) const;
		QString readElementValue(QXmlStreamReader& pReader);

	public:
		TcToken(const QByteArray& pData);
		virtual ~TcToken();

		/**
		 * Check if parsing of given data was successful.
		 */
		bool isValid() const;

		const QString& getBinding() const;
		const QByteArray& getSessionIdentifier() const;
		const QUrl& getServerAddress() const;
		const QUrl& getRefreshAddress() const;
		const QUrl& getCommunicationErrorAddress() const;
		const QByteArray& getPsk() const;
		void clearPsk();

		bool isSchemaConform() const
		{
			return mSchemaConform;
		}


};

} /* namespace governikus */
