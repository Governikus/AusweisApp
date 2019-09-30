/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0


Column {
	id: baseItem
	spacing: Constants.pane_spacing

	signal scrollHistoryUp()
	signal scrollHistoryDown()

	property var openHistoryInfoFunc: function() {
	}

	GText {
		id: headerText

		Accessible.role: Accessible.StaticText
		Accessible.name: text

		//: LABEL ANDROID_TABLET IOS_TABLET
		text: qsTr("History") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	Repeater {
		model: HistoryModel.nameFilter

		ProviderDetailHistoryItem {
			width: parent.width

			Accessible.onScrollDownAction: baseItem.scrollHistoryDown()
			Accessible.onScrollUpAction: baseItem.scrollHistoryUp()

			providerName: subject
			providerPostalAddress: providerPostalAddress
			dateTime: model.dateTime
			//: LABEL ANDROID_TABLET IOS_TABLET
			infoText: qsTr("Purpose for reading out requested data") + SettingsModel.translationTrigger
			purposeText: purpose
			requestedDataText: requestedData
			termsOfUsageText: termsOfUsage

			openInfoFunction: baseItem.openHistoryInfoFunc
		}
	}
}
