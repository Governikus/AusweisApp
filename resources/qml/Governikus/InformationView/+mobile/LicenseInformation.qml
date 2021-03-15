/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


SectionPage
{
	id: root

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Software license")

	sectionPageFlickable: listView

	GPane {
		id: pane

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}

		clip: true

		GListView {
			id: listView

			height: pane.height - 2 * Constants.pane_padding
			width: pane.width - Constants.pane_padding

			displayMarginBeginning: Constants.pane_padding
			displayMarginEnd: Constants.pane_padding

			model: ApplicationModel.getLicenseText()

			delegate: GText {
				width: listView.width - Constants.pane_padding
				text: model.modelData
			}
		}
	}
}
