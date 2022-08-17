/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.15
import QtQuick 2.15


Repeater {
	id: root

	property real maxItemWidth: 0
	property real tmpWidth: 0

	onItemAdded: (pIndex, pItem) => {
		if (pIndex === 0 || tmpWidth < pItem.implicitWidth) tmpWidth = pItem.implicitWidth

		if (root.count === pIndex + 1) {
			maxItemWidth = tmpWidth
		}
	}
}
