#pragma once

#include <QLabel>

namespace governikus
{

class ListItemIconRight
	: public QLabel
{
	Q_OBJECT

	public:
		ListItemIconRight(QWidget* pParent = nullptr);
		ListItemIconRight(const QString& pText, QWidget* pParent = nullptr);
		virtual ~ListItemIconRight();
};

} /* namespace governikus */
