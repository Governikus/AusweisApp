/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	contentIsScrolled: !listView.atYBeginning

	//: MOBILE
	title: qsTr("Software license")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	onActiveFocusChanged: if (activeFocus) {
		listView.highlightScrollbar();
		if (!ApplicationModel.screenReaderRunning)
			listView.forceActiveFocus(Qt.MouseFocusReason);
	}

	GListView {
		id: listView

		displayMarginBeginning: Style.dimens.pane_padding
		displayMarginEnd: Style.dimens.pane_padding
		keyNavigationChangesCurrentIndex: false
		model: ApplicationModel.getLicenseText()

		delegate: GText {
			required property int index
			required property string modelData

			Accessible.role: Utils.useSpecialAppleTabRole(Accessible.StaticText)
			anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
			anchors.horizontalCenterOffset: -Style.dimens.pane_padding / 2
			enabled: modelData !== ""
			focusFrameVisible: false
			text: modelData
			width: Math.min(listView.width - Style.dimens.pane_padding, Style.dimens.max_text_width)

			Accessible.onScrollDownAction: listView.scrollPageDown()
			Accessible.onScrollUpAction: listView.scrollPageUp()
			onActiveFocusChanged: if (activeFocus) {
				listView.centerViewAtIndex(index);
			}
		}

		onActiveFocusChanged: if (activeFocus)
			highlightScrollbar()

		anchors {
			bottomMargin: Style.dimens.pane_padding
			fill: parent
			leftMargin: Style.dimens.pane_padding
			topMargin: Style.dimens.pane_padding
		}
	}
	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software

		effect: GDropShadow {
		}
	}
}
