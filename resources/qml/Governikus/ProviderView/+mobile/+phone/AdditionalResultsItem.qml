/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

ListItem {
	property int totalHits: ProviderCategoryFilterModel.additionalResultCount

	visible: totalHits > 0 && ProviderCategoryFilterModel.categories.length > 0 && ProviderCategoryFilterModel.categories.indexOf("all") === -1

	Accessible.name: qsTr("%1 additional results in other categories").arg(totalHits) + SettingsModel.translationTrigger
	Accessible.description: qsTr("Click to remove category filter and show additional results.") + SettingsModel.translationTrigger

	icon: Category.imageSource("all")
	text: qsTr("Additional results:") + ' '  + totalHits + SettingsModel.translationTrigger
	showLinkIcon: false
	showSeparator: false

	onClicked: ProviderCategoryFilterModel.setCategorySelection("")
}
