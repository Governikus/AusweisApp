/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
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

	Q_PROPERTY(double x MEMBER mXPosition CONSTANT)
	Q_PROPERTY(double y MEMBER mYPosition CONSTANT)
	Q_PROPERTY(int z MEMBER mZPosition CONSTANT)
	Q_PROPERTY(double rotation MEMBER mRotation CONSTANT)

	public:
		CardPosition(double pXPosition = 0.0, double pYPosition = 0.0, int pZPosition = 1, double pRotation = 0.0);
		CardPosition(const CardPosition& pOther);
		~CardPosition() override = default;
		CardPosition& operator=(const CardPosition& pOther);

		double mXPosition;
		double mYPosition;
		int mZPosition;
		double mRotation;
};


} // namespace governikus
