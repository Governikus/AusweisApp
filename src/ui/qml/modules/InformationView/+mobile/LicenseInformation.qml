/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
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

	Connections {
		function onActivate() {
			listView.highlightScrollbar();
		}
	}
	GListView {
		id: listView

		displayMarginBeginning: Style.dimens.pane_padding
		displayMarginEnd: Style.dimens.pane_padding
		model: ApplicationModel.getLicenseText()

		delegate: Item {
			id: delegateItem

			required property int index
			required property string modelData

			implicitHeight: delegateText.implicitHeight
			implicitWidth: root.width

			GText {
				id: delegateText

				Accessible.role: Utils.useSpecialAppleTabRole(Accessible.StaticText)
				anchors.centerIn: parent
				anchors.horizontalCenterOffset: -Style.dimens.pane_padding
				text: delegateItem.modelData
				width: Math.min(listView.width - Style.dimens.pane_padding, Style.dimens.max_text_width)

				Accessible.onScrollDownAction: listView.scrollPageDown()
				Accessible.onScrollUpAction: listView.scrollPageUp()
			}
		}

		anchors {
			bottomMargin: Style.dimens.pane_padding
			fill: parent
			leftMargin: Style.dimens.pane_padding
			topMargin: Style.dimens.pane_padding
		}
	}
	layer {
		enabled: true

		effect: GDropShadow {
		}
	}
}
