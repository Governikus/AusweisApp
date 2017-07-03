import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "../"
import "../global"


SectionPage {
	id: root
	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Support"); font.bold: true }

	ListModel {
		id: supportModel
		ListElement {
			label: qsTr("Via Email")
			text: qsTr("You can reach our email support at <a href=\"mailto:support@ausweisapp.de\">support@ausweisapp.de</a>")
		}
		ListElement {
			label: qsTr("Via Phone")
			text: qsTr("You can reach our phone support at <a href=\"tel:+491805348743\">+49 1805 348 743</a> (*14 ct./Minute from German landlines, mobile price may vary)")
		}
		ListElement {
			label: qsTr("Support availability")
			text: qsTr("Support is available from 9.00 to 17.00 on Monday-Friday, except on national holidays.")
		}
	}

	content: Column{
		width: root.width
		Item { width: parent.width;  height: Constants.titlebar_height }
		Pane {
			id: pane
			width: root.width
			height: childrenRect.height
			Repeater {
				model: supportModel

				LabeledText {
					id: delegate
					label: model.label
					text: model.text
					width: pane.width
				}
			}
		}
	}
}
