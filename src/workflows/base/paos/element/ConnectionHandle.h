/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Object hold the paos connection handle.
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
		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const QString& getCardApplication() const;
		[[nodiscard]] const QString& getSlotIndex() const;
		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] const QString& getContextHandle() const;
};

} // namespace governikus
