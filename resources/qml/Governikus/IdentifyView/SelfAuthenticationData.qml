import QtQuick 2.7

import Governikus.Global 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: root
	leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: root.close() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }

	function close() {
		numberModel.continueWorkflow()
		firePopAll()
		navBar.lockedAndHidden = false
	}

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
				width: resultImage.width + Constants.component_spacing + successText.width
				anchors.horizontalCenter: parent.horizontalCenter

				Image {
					id: resultImage
					anchors.top: parent.top
					anchors.verticalCenter: parent.verticalCenter
					height: parent.height
					width: height
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/gruener_Haken.svg"
				}

				Text {
					id: successText
					anchors.left: resultImage.right
					anchors.leftMargin: Constants.component_spacing
					anchors.verticalCenter: resultImage.verticalCenter
					text: qsTr("Successfull reading data") + settingsModel.translationTrigger
					font.pixelSize: PlatformConstants.is_tablet ? Constants.header_font_size : Constants.normal_font_size
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
					columns: PlatformConstants.is_tablet ? 3 : 1
					spacing: Utils.dp(15)
					verticalItemAlignment: Grid.AlignBottom
					Repeater {
						model: selfAuthenticationModel

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
		text: qsTr("Ok") + settingsModel.translationTrigger
		onClicked: root.close()
	}
}
