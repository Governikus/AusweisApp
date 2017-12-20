import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: root
	property var historyModelItem

	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: historyModelItem ? historyModelItem.subject : ""; font.bold: true }
	titleBarColor: Category.displayColor(historyModelItem ? historyModelItem.providerCategory : "")

	content: Item {
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column {
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			Pane {
				id: pane
				title: qsTr("Provider Information") + settingsModel.translationTrigger

				LabeledText {
					label: qsTr("Provider name") + settingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.subject : ""
					width: parent.width
				}

				LabeledText {
					label: qsTr("Purpose") + settingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.purpose : ""
					width: parent.width
				}

				LabeledText {
					label: qsTr("Date") + settingsModel.translationTrigger
					text:{
						if (!historyModelItem) {
							return "";
						}
						return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")) + settingsModel.translationTrigger
					}
					width: parent.width
					fontUppercase: Font.AllUppercase
				}

				LabeledText {
					label: qsTr("Requested data") + settingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.requestedData : ""
					width: parent.width
				}

				LabeledText {
					label: qsTr("Terms of usage") + settingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.termsOfUsage : ""
					width: parent.width
				}
			}
		}
	}
}
