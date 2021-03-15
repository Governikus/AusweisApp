/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0


Item {
	id: root
	property var historyModelItem

	height: columnLayout.height

	Accessible.name: qsTr("Details for history entry")

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

			//: LABEL DESKTOP_QML
			label: qsTr("Provider name")
			text: historyModelItem ? historyModelItem.subject : ""
		}

		LabeledText {
			id: purpose

			visible: text !== ""
			Layout.fillWidth: true

			//: LABEL DESKTOP_QML
			label: qsTr("Purpose")
			text: historyModelItem ? historyModelItem.purpose : ""
		}

		LabeledText {
			id: date

			Layout.fillWidth: true

			//: LABEL DESKTOP_QML
			label: qsTr("Date")
			text:{
				if (!historyModelItem) {
					return "";
				}
				return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
			}
			textUppercase: Font.AllUppercase
		}

		LabeledText {
			id: writtenData

			visible: text !== ""
			Layout.fillWidth: true

			//: LABEL DESKTOP_QML
			label: qsTr("Write access (update)")
			text: historyModelItem ? historyModelItem.writtenData : ""
		}

		LabeledText {
			id: requestedData

			Layout.fillWidth: true

			//: LABEL DESKTOP_QML
			label: qsTr("Read access")
			text: historyModelItem ? historyModelItem.requestedData : ""
		}

		LabeledText {
			id: termsOfUsage

			Layout.fillWidth: true

			//: LABEL DESKTOP_QML
			label: qsTr("Terms of usage")
			text: historyModelItem ? historyModelItem.termsOfUsage : ""
		}
	}
}
