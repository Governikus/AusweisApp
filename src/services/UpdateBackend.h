/*!
 * UpdateBackend.h
 *
 * \brief Interface for classes that perform some processing with
 *        data that has been downloaded during an update.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#pragma once


#include <QByteArray>
#include <QDateTime>


namespace governikus
{
class UpdateBackend
{
	public:
		UpdateBackend();

		virtual ~UpdateBackend();

		virtual QDateTime getIssueDate() const = 0;

		virtual void processSuccess(const QByteArray& pData) = 0;

		virtual void processError() = 0;
};

}
