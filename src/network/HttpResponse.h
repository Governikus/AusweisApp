/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class to create http response.
 */

#pragma once

#include <QByteArray>
#include <QMap>
#include <http_parser.h>

namespace governikus
{

class HttpResponse
{
	private:
		http_status mStatus;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;

		[[nodiscard]] QByteArray getStatusMessage() const;

	public:
		HttpResponse(http_status pStatus = HTTP_STATUS_INTERNAL_SERVER_ERROR,
				const QByteArray& pBody = QByteArray(),
				const QByteArray& pContentType = QByteArray());

		[[nodiscard]] QByteArray getHeader(const QByteArray& pKey) const;
		[[nodiscard]] const QMap<QByteArray, QByteArray>& getHeaders() const;
		void setHeader(const QByteArray& pKey, const QByteArray& pValue);

		[[nodiscard]] http_status getStatus() const;
		void setStatus(http_status pStatus);

		[[nodiscard]] const QByteArray& getBody() const;
		void setBody(const QByteArray& pBody, const QByteArray& pContentType = QByteArray());

		[[nodiscard]] QByteArray getMessage() const;
};

} // namespace governikus
