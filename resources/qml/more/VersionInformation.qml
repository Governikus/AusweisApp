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

	content: Item
	{
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column
		{
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			Pane {
				id: pane

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
}
