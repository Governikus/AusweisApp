/*
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ReleaseInformationModel 1.0

SectionPage
{
	id: root

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Release notes")

	sectionPageFlickable: releaseNotes

	Component.onCompleted: ReleaseInformationModel.update()

	GPane {
		id: pane

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}

		ColumnLayout {
			height: pane.height - pane.anchors.topMargin - pane.anchors.bottomMargin
			width: pane.width - pane.anchors.leftMargin

			ReleaseNotesView {
				id: releaseNotes

				Layout.fillHeight: true
				Layout.fillWidth: true

				model: ReleaseInformationModel.currentRelease
			}

			GButton {
				visible: ReleaseInformationModel.allowRetry

				Layout.alignment: Qt.AlignHCenter
				Layout.rightMargin: pane.anchors.rightMargin

				//: LABEL ANDROID IOS
				text: qsTr("Retry")
				icon.source: "qrc:///images/material_refresh.svg"
				tintIcon: true

				onClicked: ReleaseInformationModel.update()
			}
		}
	}
}
