/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: root
	property var historyModelItem

	height: columnLayout.height

	Accessible.name: qsTr("Details for history entry") + SettingsModel.translationTrigger

	ColumnLayout {
		id: columnLayout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Constants.pane_spacing

		GText {
			text: qsTr("Provider Information") + SettingsModel.translationTrigger
			textStyle: Style.text.header_accent
		}

		LabeledText {
			id: providerName

			Layout.fillWidth: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			label: qsTr("Provider name") + SettingsModel.translationTrigger
			text: historyModelItem ? historyModelItem.subject : ""
		}

		LabeledText {
			id: purpose

			Layout.fillWidth: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			label: qsTr("Purpose") + SettingsModel.translationTrigger
			text: historyModelItem ? historyModelItem.purpose : ""
		}

		LabeledText {
			id: date

			Layout.fillWidth: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			label: qsTr("Date") + SettingsModel.translationTrigger
			text:{
				if (!historyModelItem) {
					return "";
				}
				return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")) + SettingsModel.translationTrigger
			}
			textUppercase: Font.AllUppercase
		}

		LabeledText {
			id: requestedData

			Layout.fillWidth: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			label: qsTr("Submitted data") + SettingsModel.translationTrigger
			text: historyModelItem ? historyModelItem.requestedData : ""
		}

		LabeledText {
			id: termsOfUsage

			Layout.fillWidth: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			label: qsTr("Terms of usage") + SettingsModel.translationTrigger
			text: historyModelItem ? historyModelItem.termsOfUsage : ""
		}
	}
}
