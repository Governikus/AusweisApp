/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	contentIsScrolled: !releaseNotes.atYBeginning

	//: LABEL ANDROID IOS
	title: qsTr("Release notes")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	Component.onCompleted: releaseInformationModel.update()

	Connections {
		function onActivate() {
			releaseNotes.highlightScrollbar();
		}
	}
	ReleaseInformationModel {
		id: releaseInformationModel

	}
	ColumnLayout {
		ReleaseNotesView {
			id: releaseNotes

			Layout.fillHeight: true
			Layout.fillWidth: true
			maximumContentWidth: Style.dimens.max_text_width
			model: releaseInformationModel.currentRelease
		}
		GSpacer {
			Layout.fillHeight: retryButton.visible
		}
		GButton {
			id: retryButton

			Layout.alignment: Qt.AlignHCenter
			Layout.rightMargin: Constants.pane_padding
			icon.source: "qrc:///images/material_refresh.svg"
			//: LABEL ANDROID IOS
			text: qsTr("Retry")
			tintIcon: true
			visible: releaseInformationModel.allowRetry

			onClicked: releaseInformationModel.update()
		}
		anchors {
			bottomMargin: Constants.pane_padding
			fill: parent
			leftMargin: Constants.pane_padding
			topMargin: Constants.pane_padding
		}
	}
}
