/*!
 * ReturnCodeUtil.h
 *
 * \brief Global ReturnCode utility functions.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QCoreApplication>
#include <QPair>
#include <QString>

#include "Result.h"
#include "ReturnCode.h"

namespace governikus
{


class ReturnCodeUtil
{
	Q_DECLARE_TR_FUNCTIONS(governikus::ReturnCodeUtil)

	struct Data
	{
		Result::Minor trCode;
		const char* longText;
		const char* shortText;
	};

	typedef QMap<ReturnCode, Data> MapType;
	static const MapType mCodeTable;

	ReturnCodeUtil() = delete;
	~ReturnCodeUtil() = delete;

	public:
		/*!
		 * \brief Returns a short internationalized string for a given error code.
		 * \param pCode The error code.
		 * \return The internationalized string.
		 */
		static QString toString(ReturnCode pCode);

		/*!
		 * \brief Returns an internationalized error message string (a sentence) for a given error code.
		 * \param pCode The error code.
		 * \return The internationalized message string.
		 */
		static QString toMessage(ReturnCode pCode);

		/*!
		 * \brief Translate an error code to a type "Result::Minor" error code.
		 * \param pCode The error code.
		 * \return The type "Result::Minor" error code.
		 */
		static Result::Minor toResult(ReturnCode pCode);

		/*!
		 * \brief Translate an error code to a type "Result::Minor" error code and
		 * an internationalized error message string.
		 * \param pCode The error code.
		 * \return A QPair containinf the type "Result::Minor" error code and the
		 * error message string.
		 */
		static QPair<Result::Minor, QString> toError(ReturnCode pCode);
};

} /* namespace governikus */
