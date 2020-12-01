/*!
 * \brief Object hold the paos connection handle.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class ConnectionHandle
{
	friend class ConnectionHandleParser;
	friend class DIDList;

	private:
		QString mContextHandle;
		QString mIfdName;
		QString mSlotIndex;
		QString mCardApplication;
		QString mSlotHandle;

		void setSlotHandle(const QString& pSlotHandle);
		void setCardApplication(const QString& pCardApplication);
		void setSlotIndex(const QString& pSlotIndex);
		void setIfdName(const QString& pIfdName);
		void setContextHandle(const QString& pContextHandle);

	public:
		const QString& getSlotHandle() const;
		const QString& getCardApplication() const;
		const QString& getSlotIndex() const;
		const QString& getIfdName() const;
		const QString& getContextHandle() const;
};

} // namespace governikus
