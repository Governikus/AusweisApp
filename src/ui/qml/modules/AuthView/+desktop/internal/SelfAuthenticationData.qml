/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: baseItem

	signal accept

	fillWidth: true
	margins: Constants.pane_padding * 2
	spacing: Constants.component_spacing

	titleBarAction: TitleBarAction {
		rootEnabled: false
		//: LABEL DESKTOP Title of the self authentication result data view
		text: qsTr("Read self-authentication data")
	}

	Keys.onEnterPressed: okButton.clicked()
	Keys.onEscapePressed: okButton.clicked()
	Keys.onReturnPressed: okButton.clicked()

	Connections {
		function onFireCancelWorkflow() {
			if (SelfAuthModel.workflowCancelled) {
				accept();
			}
		}

		enabled: visible
		target: SelfAuthModel
	}
	Row {
		id: statusRow

		Layout.preferredHeight: baseItem.height / 4
		spacing: Constants.component_spacing

		TintableIcon {
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name)
			sourceSize.height: Style.dimens.huge_icon_size
			tintEnabled: false
		}
		GText {
			id: successText

			Accessible.name: successText.text
			activeFocusOnTab: true
			anchors.verticalCenter: parent.verticalCenter

			//: INFO DESKTOP Status message that the self authentication successfully completed.
			text: qsTr("Successfully read data")
			textStyle: Style.text.headline

			FocusFrame {
			}
		}
	}
	GPane {
		id: pane

		Layout.fillWidth: true
		activeFocusOnTab: true

		//: LABEL DESKTOP Title of the self authentication result data view
		title: qsTr("Read data")

		Grid {
			id: grid

			columns: 3
			spacing: Constants.groupbox_spacing
			verticalItemAlignment: Grid.AlignTop

			Repeater {
				id: dataRepeater

				model: SelfAuthModel

				LabeledText {
					label: name
					text: value === "" ? "---" : value
					width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns
				}
			}
		}
	}
	GButton {
		id: okButton

		Layout.alignment: Qt.AlignHCenter

		//: LABEL DESKTOP
		text: qsTr("Back to start page")

		onClicked: baseItem.accept()
	}
}
