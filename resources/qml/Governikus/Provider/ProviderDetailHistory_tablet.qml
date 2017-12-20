import QtQuick 2.6

import Governikus.Global 1.0
import Governikus.Provider 1.0

Column {
	id: baseItem
	spacing: Constants.pane_spacing

	property var openHistoryInfoFunc: function() {
	}

	readonly property int historyItemHeight: Utils.dp(66)
	readonly property int historyItemMargin: Utils.dp(8)

	Text {
		id: headerText

		font.pixelSize: Constants.header_font_size
		color: Constants.blue
		text: qsTr("History") + settingsModel.translationTrigger
	}

	Repeater {
		model: historyModel.nameFilter

		ProviderDetailHistoryItem_tablet {
			itemHeight: baseItem.historyItemHeight
			itemMargin: baseItem.historyItemMargin

			width: parent.width

			providerName: subject
			providerPostalAddress: providerPostalAddress
			dateTime: model.dateTime
			infoText: qsTr("Purpose for reading out requested data") + settingsModel.translationTrigger
			purposeText: purpose
			requestedDataText: requestedData
			termsOfUsageText: termsOfUsage

			openInfoFunction: baseItem.openHistoryInfoFunc
		}
	}
}
