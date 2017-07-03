#pragma once

#include <QLabel>

namespace governikus
{

class ListItemIconLeft
	: public QLabel
{
	Q_OBJECT

	public:
		ListItemIconLeft(QWidget* pParent = nullptr);
		ListItemIconLeft(const QString& pText, QWidget* pParent = nullptr);
		virtual ~ListItemIconLeft();
};

} /* namespace governikus */
