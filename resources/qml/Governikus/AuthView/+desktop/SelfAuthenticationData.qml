/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.SelfAuthModel
import Governikus.Type.UiModule

SectionPage {
	id: baseItem

	signal accept

	titleBarAction: TitleBarAction {
		rootEnabled: false
		showHelp: false
		//: LABEL DESKTOP Title of the self authentication result data view
		text: qsTr("Read self-authentication data")
	}

	Keys.onEnterPressed: okButton.onClicked()
	Keys.onEscapePressed: okButton.onClicked()
	Keys.onReturnPressed: okButton.onClicked()

	Connections {
		function onFireCancelWorkflow() {
			if (SelfAuthModel.workflowCancelled) {
				accept();
			}
		}

		enabled: visible
		target: SelfAuthModel
	}
	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding * 2
		spacing: Constants.component_spacing

		Row {
			id: statusRow

			Layout.preferredHeight: baseItem.height / 4
			spacing: Constants.component_spacing

			TintableIcon {
				anchors.verticalCenter: parent.verticalCenter
				height: Style.dimens.status_icon_medium
				source: "qrc:///images/desktop/status_ok_%1.svg".arg(Style.currentTheme.name)
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
		ScrollablePane {
			id: pane

			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.maximumHeight: implicitHeight
			activeFocusOnTab: true
			enableDropShadow: true

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
			text: qsTr("OK")

			onClicked: baseItem.accept()
		}
		GSpacer {
			Layout.fillHeight: true
		}
	}
}
