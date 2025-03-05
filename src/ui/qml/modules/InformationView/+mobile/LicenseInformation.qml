/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
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

	//: LABEL ANDROID IOS
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

		Accessible.ignored: false
		displayMarginBeginning: Style.dimens.pane_padding
		displayMarginEnd: Style.dimens.pane_padding
		model: ApplicationModel.getLicenseText()

		delegate: Item {
			id: delegateItem

			required property int index
			required property string modelData

			implicitHeight: delegateText.implicitHeight
			implicitWidth: root.width

			GPaneBackgroundDelegate {
				anchors.centerIn: parent
				anchors.horizontalCenterOffset: -Style.dimens.pane_padding
				count: listView.count
				height: delegateText.implicitHeight
				idx: delegateItem.index
				width: Math.min(listView.width - Style.dimens.pane_padding, Style.dimens.max_text_width)

				GText {
					id: delegateText

					activeFocusOnTab: false
					anchors.fill: parent
					bottomPadding: parent.isLast ? Style.dimens.pane_padding : 0
					leftPadding: Style.dimens.pane_padding
					rightPadding: Style.dimens.pane_padding
					text: delegateItem.modelData
					topPadding: parent.isFirst ? Style.dimens.pane_padding : 0
				}
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
