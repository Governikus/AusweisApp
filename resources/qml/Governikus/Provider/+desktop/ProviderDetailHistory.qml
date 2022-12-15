/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Provider 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

Item {
	id: baseItem
	Accessible.description: HistoryModel.nameFilter.count === 0 ? qsTr("The list is empty, no recorded interaction with this provider.") : ""
	Accessible.name: qsTr("List of your past interactions with this provider")
	Accessible.role: Accessible.List
	height: columnLayout.height

	ColumnLayout {
		id: columnLayout
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		spacing: Constants.text_spacing

		Repeater {
			id: repeater
			model: HistoryModel.nameFilter

			ColumnLayout {
				Layout.fillWidth: true
				spacing: columnLayout.spacing

				GSeparator {
					Layout.fillWidth: true
					visible: index !== 0
				}
				ProviderDetailHistoryItem {
					id: historyItem
					Layout.fillWidth: true
					activeFocusOnTab: true
					providerName: subject
					purposeText: purpose
				}
			}
		}
		GText {
			activeFocusOnTab: true

			//: INFO DESKTOP No authentication history, placeholder text.
			text: qsTr("Currently there are no history entries.")
			textStyle: Style.text.normal
			visible: repeater.count === 0
			width: parent.width

			FocusFrame {
			}
		}
	}
}
