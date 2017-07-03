#pragma once

#include <QLabel>

namespace governikus
{

class ListItemTitle
	: public QLabel
{
	Q_OBJECT

	public:
		ListItemTitle(QWidget* pParent = nullptr);
		ListItemTitle(const QString& pText, QWidget* pParent = nullptr);
		virtual ~ListItemTitle();
};

} /* namespace governikus */
