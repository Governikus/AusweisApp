import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0

import Governikus.Type.ApplicationModel 1.0


SectionPage
{
	titleBarAction: TitleBarAction {
		text: qsTr("Read data")
		showSettings: false
		showHelp: false
	}

	Row {
		id: statusRow

		anchors.top: parent.top
		anchors.topMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter
		height: parent.height / 4
		spacing: Constants.component_spacing

		StatusIcon {
			height: ApplicationModel.scaleFactor * 200
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:///images/status_ok.svg"
		}

		Text {
			id: successText
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Successfull reading data") + settingsModel.translationTrigger
			font.pixelSize: Constants.header_font_size
			color: Constants.white
		}
	}

	Pane {
		id: pane
		anchors.top: statusRow.bottom
		anchors.margins: Constants.pane_padding
		title: qsTr("Read data") + settingsModel.translationTrigger

		Grid {
			id: grid
			width: parent.width
			columns: 3
			spacing: Utils.dp(15)
			verticalItemAlignment: Grid.AlignTop
			Repeater {
				model: selfAuthModel

				LabeledText {
					label: name
					text: value === "" ? "---" : value
					width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns
				}
			}
		}

		GButton {
			id: okButton
			anchors.right: parent.right
			text: qsTr("OK") + settingsModel.translationTrigger
			onClicked: AuthModel.continueWorkflow()
		}
	}
}
