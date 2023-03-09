/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: baseItem

	property string providerName: ""
	property string purposeText: ""

	Accessible.name: date.text + ". " + providerName + ". " + purposeText
	Accessible.role: Accessible.ListItem
	height: columnLayout.height

	ColumnLayout {
		id: columnLayout
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		spacing: Constants.text_spacing

		GText {
			id: date
			font.capitalization: Font.AllUppercase
			text: (Utils.isToday(dateTime) ? qsTr("today") : Utils.isYesterday(dateTime) ? qsTr("yesterday") : Utils.isThisWeek(dateTime) ? dateTime.toLocaleString(Qt.locale(SettingsModel.language), "dddd") : dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")))
			textStyle: Style.text.normal
		}
		GridLayout {
			Layout.fillWidth: true
			columnSpacing: Constants.text_spacing
			columns: 2

			GText {
				maximumLineCount: 1
				//: LABEL DESKTOP
				text: qsTr("Service:")
				textStyle: Style.text.normal_highlight
			}
			GText {
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: 1
				text: purposeText
				textStyle: Style.text.normal
			}
			GText {
				maximumLineCount: 1
				//: LABEL DESKTOP
				text: qsTr("Provider:")
				textStyle: Style.text.normal_highlight
			}
			GText {
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: 1
				text: providerName
				textStyle: Style.text.normal
			}
		}
	}
	FocusFrame {
	}
}
