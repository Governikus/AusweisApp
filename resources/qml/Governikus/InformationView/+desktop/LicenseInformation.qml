/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	height: tabbedPane.availableHeight
	width: parent.width

	readonly property string helpTopic: "helpLicenseinformation"

	// Padding used by the tabbed pane
	readonly property int contentPadding: 0

	GListView {
		id: listView

		anchors {
			fill: parent
			topMargin: Constants.pane_padding
			leftMargin: Constants.pane_padding
			bottomMargin: Constants.pane_padding
		}

		activeFocusOnTab: true

		displayMarginBeginning: Constants.pane_padding
		displayMarginEnd: Constants.pane_padding

		model: ApplicationModel.getLicenseText()

		delegate: GText {
			width: listView.width - Constants.pane_padding

			text: model.modelData
			textStyle: Style.text.normal
		}
	}

	FocusFrame {
		scope: listView
		framee: listView
		borderColor: Style.color.focus_indicator
	}
}
