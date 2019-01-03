import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0

SectionPage {
	id: root
	leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: root.done() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }

	signal done()

	content: Item {
		height: content.height + okButton.height + Constants.component_spacing
		width: root.width

		Column {
			id: content
			width: parent.width
			padding: Constants.component_spacing
			spacing: Constants.component_spacing

			Item {
				id: message
				height: Utils.dp(60)
				width: resultIcon.width + Constants.component_spacing + successText.width
				anchors.horizontalCenter: parent.horizontalCenter

				StatusIcon {
					id: resultIcon
					height: parent.height
					anchors.top: parent.top
					anchors.verticalCenter: parent.verticalCenter
					source: "qrc:///images/status_ok.svg"
				}

				Text {
					id: successText
					anchors.left: resultIcon.right
					anchors.leftMargin: Constants.component_spacing
					anchors.verticalCenter: resultIcon.verticalCenter
					text: qsTr("Successfull reading data") + settingsModel.translationTrigger
					font.pixelSize: Constants.is_tablet ? Constants.header_font_size : Constants.normal_font_size
					color: Constants.blue
				}
			}

			Pane {
				id: pane
				anchors.leftMargin: Constants.component_spacing
				anchors.rightMargin: Constants.component_spacing

				Grid {
					id: grid
					width: parent.width
					columns: Constants.is_tablet ? 3 : 1
					spacing: Utils.dp(15)
					verticalItemAlignment: Grid.AlignBottom
					Repeater {
						model: selfAuthModel

						LabeledText {
							label: name
							text: value === "" ? "---" : value
							width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns
						}
					}
				}
			}
		}
	}

	GButton {
		id: okButton
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Constants.component_spacing
		text: qsTr("OK") + settingsModel.translationTrigger
		onClicked: root.done()
	}
}
