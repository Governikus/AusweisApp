/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	required property string okButtonText

	fillWidth: true
	margins: Style.dimens.pane_padding * 2
	spacing: Style.dimens.pane_spacing

	//: LABEL DESKTOP Title of the self authentication result data view
	title: qsTr("Read self-authentication data")

	Keys.onEnterPressed: okButton.clicked()
	Keys.onReturnPressed: okButton.clicked()

	Connections {
		function onFireCancelWorkflow() {
			if (SelfAuthModel.workflowCancelled) {
				root.leaveView();
			}
		}

		enabled: root.visible
		target: SelfAuthModel
	}
	RowLayout {
		Layout.preferredHeight: root.height / 4
		spacing: Style.dimens.pane_spacing

		StatusAnimation {
			Layout.alignment: Qt.AlignVCenter
			sourceSize.height: Style.dimens.huge_icon_size
			symbol.type: Symbol.Type.CHECK
		}
		GText {
			Layout.alignment: Qt.AlignVCenter

			//: INFO DESKTOP Status message that the self authentication successfully completed.
			text: qsTr("Successfully read data.") + "<br>" + qsTr("You may now remove your ID card from the device.")
			textStyle: Style.text.headline
		}
	}
	GPane {
		id: pane

		Layout.fillWidth: true

		//: LABEL DESKTOP Title of the self authentication result data view
		title: qsTr("Read data")

		Grid {
			id: grid

			columns: 3
			spacing: Style.dimens.groupbox_spacing
			verticalItemAlignment: Grid.AlignTop

			Repeater {
				id: dataRepeater

				model: SelfAuthModel

				LabeledText {
					required property string name
					required property string value

					label: name
					text: value === "" ? "---" : value
					width: (pane.width - 2 * Style.dimens.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns
				}
			}
		}
	}
	GButton {
		id: okButton

		Layout.alignment: Qt.AlignHCenter
		text: root.okButtonText

		onClicked: root.leaveView()
	}
}
