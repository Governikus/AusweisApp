/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0


ListItem {
	readonly property bool isProvider: itemType === "provider"

	Accessible.description: (isProvider ? qsTr("Open provider details for %1").arg(display) : qsTr("Click to set category filter to %1").arg(text))

	text: (isProvider ? display : Category.displayString(providerCategory))
	icon: isProvider ? "" : Category.imageSource(providerCategory)
}
