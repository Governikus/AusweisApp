/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: root
	sectionPageFlickable: listView
	//: LABEL ANDROID IOS
	title: qsTr("Software license")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	GPane {
		id: pane
		clip: true

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}
		GListView {
			id: listView
			displayMarginBeginning: Constants.pane_padding
			displayMarginEnd: Constants.pane_padding
			height: pane.height - 2 * Constants.pane_padding
			model: ApplicationModel.getLicenseText()
			width: pane.width - Constants.pane_padding

			delegate: GText {
				text: model.modelData
				width: listView.width - Constants.pane_padding
			}
		}
	}
}
