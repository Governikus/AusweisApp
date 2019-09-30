/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.10

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0


ListItem {
	property string sectionName

	Accessible.description: qsTr("Click to set category filter to %1").arg(text) + SettingsModel.translationTrigger

	icon: Category.imageSource(sectionName)
	text: Category.displayString(sectionName) + SettingsModel.translationTrigger

	onClicked: ProviderCategoryFilterModel.setCategorySelection(sectionName)
}
