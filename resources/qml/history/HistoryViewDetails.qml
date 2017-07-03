import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2


import "../"
import "../global"

SectionPage {
	id: root
	property var modelItem

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: modelItem ? modelItem.subject : ""; font.bold: true }
	titleBarColor: Category.displayColor(modelItem ? modelItem.provider_category : "")

	content: Column {
		width: root.width

		Item { width: parent.width; height: Utils.dp(10)}
		Pane {
			id: pane
			title: qsTr("Provider Information")

			LabeledText {
				label: qsTr("Provider name")
				text: modelItem ? modelItem.subject : ""
				width: parent.width
			}

			LabeledText {
				label: qsTr("Purpose")
				text: modelItem ? modelItem.purpose : ""
				width: parent.width
			}

			LabeledText {
				label: qsTr("Date")
				text:{
					if (!modelItem) {
						return "";
					}
					return modelItem.dateTime.toLocaleString(Qt.locale(), qsTr("MM/dd/yyyy"))
				}
				width: parent.width
				fontUppercase: Font.AllUppercase
			}

			LabeledText {
				label: qsTr("Requested data")
				text: modelItem ? modelItem.requestedData : ""
				width: parent.width
			}

			LabeledText {
				label: qsTr("Terms of usage")
				text: modelItem ? modelItem.termsOfUsage : ""
				width: parent.width
			}
		}
	}

}
