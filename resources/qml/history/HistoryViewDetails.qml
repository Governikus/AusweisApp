import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2

import "../"
import "../global"

SectionPage {
	id: root
	property var historyModelItem

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
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
				title: qsTr("Provider Information")

				LabeledText {
					label: qsTr("Provider name")
					text: historyModelItem ? historyModelItem.subject : ""
					width: parent.width
				}

				LabeledText {
					label: qsTr("Purpose")
					text: historyModelItem ? historyModelItem.purpose : ""
					width: parent.width
				}

				LabeledText {
					label: qsTr("Date")
					text:{
						if (!historyModelItem) {
							return "";
						}
						return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("MM/dd/yyyy"))
					}
					width: parent.width
					fontUppercase: Font.AllUppercase
				}

				LabeledText {
					label: qsTr("Requested data")
					text: historyModelItem ? historyModelItem.requestedData : ""
					width: parent.width
				}

				LabeledText {
					label: qsTr("Terms of usage")
					text: historyModelItem ? historyModelItem.termsOfUsage : ""
					width: parent.width
				}
			}
		}
	}
}
