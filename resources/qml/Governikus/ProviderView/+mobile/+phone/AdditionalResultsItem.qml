/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

ListItem {
	property int totalHits: 0

	Accessible.name: qsTr("%1 additional results in other categories").arg(totalHits)
	Accessible.description: qsTr("Click to remove category filter and show additional results.")

	icon: Category.imageSource("all")
	text: qsTr("Additional results:") + ' '  + totalHits
	showLinkIcon: false
	showSeparator: false
}
