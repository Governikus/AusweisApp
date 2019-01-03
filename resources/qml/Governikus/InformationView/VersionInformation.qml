import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0


SectionPage
{
	id: root
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Version Information") + settingsModel.translationTrigger; font.bold: true }

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
