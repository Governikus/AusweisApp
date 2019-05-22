/*!
 * \brief Class to create http response.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <http_parser.h>
#include <QByteArray>
#include <QMap>

namespace governikus
{

class HttpResponse
{
	private:
		http_status mStatus;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;

		QByteArray getStatusMessage() const;

	public:
		HttpResponse(http_status pStatus = HTTP_STATUS_INTERNAL_SERVER_ERROR,
				const QByteArray& pBody = QByteArray(),
				const QByteArray& pContentType = QByteArray());

		QByteArray getHeader(const QByteArray& pKey) const;
		const QMap<QByteArray, QByteArray>& getHeaders() const;
		void setHeader(const QByteArray& pKey, const QByteArray& pValue);

		http_status getStatus() const;
		void setStatus(http_status pStatus);

		const QByteArray& getBody() const;
		void setBody(const QByteArray& pBody, const QByteArray& pContentType = QByteArray());

		QByteArray getMessage() const;
};

} // namespace governikus
