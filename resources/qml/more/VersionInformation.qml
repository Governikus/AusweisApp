import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "../"
import "../global"


SectionPage
{
	id: root
	leftTitleBarAction: TitleBarMenuAction { state: stack.depth > 1 ? "back" : ""; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Version Information"); font.bold: true }


	content: Column{
		width: root.width
		Item { width: parent.width;  height: Constants.titlebar_height }
		Pane {
			id: pane
			width: root.width
			height: childrenRect.height
			Repeater {
				model: versionInformationModel

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
