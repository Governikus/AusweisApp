/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

Item {
	id: root

	property var historyModelItem

	Accessible.name: qsTr("Details for history entry")
	height: columnLayout.height

	ColumnLayout {
		id: columnLayout
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		spacing: Constants.pane_spacing

		GText {
			text: qsTr("Provider Information")
			textStyle: Style.text.header_accent
		}
		LabeledText {
			id: providerName
			Layout.fillWidth: true

			//: LABEL DESKTOP
			label: qsTr("Provider name")
			text: historyModelItem ? historyModelItem.subject : ""
		}
		LabeledText {
			id: purpose
			Layout.fillWidth: true

			//: LABEL DESKTOP
			label: qsTr("Purpose")
			text: historyModelItem ? historyModelItem.purpose : ""
			visible: text !== ""
		}
		LabeledText {
			id: date
			Layout.fillWidth: true

			//: LABEL DESKTOP
			label: qsTr("Date")
			text: {
				if (!historyModelItem) {
					return "";
				}
				//: LABEL DESKTOP Date format according to https://doc.qt.io/qt/qdate.html#toString
				return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"));
			}
			textUppercase: Font.AllUppercase
		}
		LabeledText {
			id: writtenData
			Layout.fillWidth: true

			//: LABEL DESKTOP
			label: qsTr("Write access (update)")
			text: historyModelItem ? historyModelItem.writtenData : ""
			visible: text !== ""
		}
		LabeledText {
			id: requestedData
			Layout.fillWidth: true

			//: LABEL DESKTOP
			label: qsTr("Read access")
			text: historyModelItem ? historyModelItem.requestedData : ""
		}
		LabeledText {
			id: termsOfUsage
			Layout.fillWidth: true

			//: LABEL DESKTOP
			label: qsTr("Terms of usage")
			text: historyModelItem ? historyModelItem.termsOfUsage : ""
		}
	}
}
