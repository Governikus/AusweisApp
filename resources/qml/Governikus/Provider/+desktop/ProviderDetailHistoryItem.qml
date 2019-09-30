/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: baseItem

	property string providerName: ""
	property string purposeText: ""

	height: columnLayout.height

	ColumnLayout {
		id: columnLayout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Constants.text_spacing

		GText {
			id: date

			textStyle: Style.text.normal_inverse
			font.capitalization: Font.AllUppercase
			text: ( Utils.isToday(dateTime) ? qsTr("today") :
				  Utils.isYesterday(dateTime) ? qsTr("yesterday") :
				  Utils.isThisWeek(dateTime) ? dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
				  dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
				  ) + SettingsModel.translationTrigger
		}

		Row {
			Layout.fillWidth: true

			GText {
				width: parent.width * 0.25

				textStyle: Style.text.normal_inverse
				//: LABEL DESKTOP_QML
				text: qsTr("Service:") + SettingsModel.translationTrigger
				font.weight: Font.Bold
			}

			GText {
				width: parent.width * 0.75

				textStyle: Style.text.normal_inverse
				text: purposeText
			}
		}

		Row {
			Layout.fillWidth: true

			GText {
				width: parent.width * 0.25

				textStyle: Style.text.normal_inverse
				//: LABEL DESKTOP_QML
				text: qsTr("Provider:") + SettingsModel.translationTrigger
				font.weight: Font.Bold
			}

			GText {
				width: parent.width * 0.75

				textStyle: Style.text.normal_inverse
				text: providerName
			}
		}
	}
}
