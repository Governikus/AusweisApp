/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>


namespace governikus
{

class CardPosition
	: public QObject
{
	Q_OBJECT
	QML_ELEMENT

	private:
		Q_PROPERTY(double x READ getXPosition CONSTANT)
		Q_PROPERTY(double y READ getYPosition CONSTANT)
		Q_PROPERTY(double rotation READ getRotation CONSTANT)
		Q_PROPERTY(int z READ getZPosition CONSTANT)

	public:
		struct Data
		{
			double mXPosition;
			double mYPosition;
			double mRotation = 0;
			int mZPosition = -1;
		};

	private:
		Data mData;

	public:
		explicit CardPosition(const Data& pData = {0.0, 0.0, 0.0, -1});
		~CardPosition() override = default;

		[[nodiscard]] double getXPosition() const;
		[[nodiscard]] double getYPosition() const;
		[[nodiscard]] double getRotation() const;
		[[nodiscard]] int getZPosition() const;

#ifndef QT_NO_DEBUG
		bool operator==(const CardPosition& pOther) const;
#endif


};


} // namespace governikus
