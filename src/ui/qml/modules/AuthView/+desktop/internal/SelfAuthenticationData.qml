/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	required property string okButtonText

	fillWidth: true
	spacing: Style.dimens.pane_spacing

	Connections {
		function onFireCancelWorkflow() {
			if (SelfAuthModel.workflowCancelled) {
				root.leaveView();
			}
		}

		enabled: root.visible
		target: SelfAuthModel
	}
	SelfAuthenticationHeader {
		iconSize: Style.dimens.large_icon_size
	}
	GPane {
		id: pane

		Layout.fillWidth: true

		//: DESKTOP Title of the self authentication result data view
		title: qsTr("Read data")

		Grid {
			id: grid

			columns: 3
			spacing: Style.dimens.groupbox_spacing
			verticalItemAlignment: Grid.AlignTop

			Repeater {
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

		Layout.alignment: Style.scanPatternAlignment
		text: root.okButtonText

		onClicked: root.leaveView()
	}
}
