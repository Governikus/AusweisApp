import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0

SectionPage
{
	id: root
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: name; font.bold: true }

	property string name

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

				Text {
					height: implicitHeight * 2
					verticalAlignment: Text.AlignVCenter
					text: qsTr("Provider Information") + settingsModel.translationTrigger
					color: Constants.blue
					font.pixelSize: Constants.header_font_size
					elide: Text.ElideRight
				}

				Repeater {
					id: listView
					model: certificateDescriptionModel

					LabeledText {
						id: delegate
						label: model.label
						text: model.text
						textFormat: Text.PlainText
						width: parent.width
					}
				}
			}
		}
	}
}
