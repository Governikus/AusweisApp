/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0


Item {
	id: root

	property alias releaseNotes: textReleaseNotes.text

	function scrollUp(){ scrollBar.decrease() }
	function scrollDown(){ scrollBar.increase() }

	implicitHeight: textReleaseNotes.implicitHeight

	Flickable {
		id: flickable

		anchors.fill: parent
		topMargin: Constants.pane_padding
		bottomMargin: topMargin

		activeFocusOnTab: true

		boundsBehavior: Flickable.StopAtBounds
		contentHeight: textReleaseNotes.implicitHeight
		clip: true
		ScrollBar.vertical: ScrollBar {
			id: scrollBar

			policy: size === 1.0 ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
			minimumSize: 0.1
		}

		GText {
			id: textReleaseNotes

			width: parent.width - scrollBar.width

			textFormat: Text.RichText
			textStyle: Style.text.normal
		}
	}

	ScrollGradients {
		leftMargin: 0
		anchors.fill: parent
		color: Style.color.background_pane
	}

	FocusFrame {
		scope: root
		framee: flickable
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_PageDown)
			scrollBar.increase()
		else if (event.key === Qt.Key_PageUp)
			scrollBar.decrease()
	}
}
