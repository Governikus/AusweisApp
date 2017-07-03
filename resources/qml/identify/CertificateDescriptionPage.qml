import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "../"
import "../global"

SectionPage
{
	id: root
	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
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
					text: qsTr("Provider Information")
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
