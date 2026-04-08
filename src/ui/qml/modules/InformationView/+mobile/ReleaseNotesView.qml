/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style
import Governikus.TitleBar
import Governikus.View

SectionPage {
	id: root

	contentIsScrolled: !releaseNotes.atYBeginning

	//: MOBILE
	title: qsTr("Release notes")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	Connections {
		function onActivate() {
			releaseNotes.highlightScrollbar();
		}
	}
	ReleaseNotes {
		id: releaseNotes

		maximumContentWidth: Style.dimens.max_text_width

		// when making changes, make sure that GButton in GListView footer is still
		// centered in the application window
		anchors {
			bottomMargin: Style.dimens.pane_padding
			fill: parent
			leftMargin: Style.dimens.pane_padding
			topMargin: Style.dimens.pane_padding
		}
	}
}
