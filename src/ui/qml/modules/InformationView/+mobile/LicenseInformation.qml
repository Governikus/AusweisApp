/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
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

		onClicked: pop()
	}

	Connections {
		function onActivate() {
			listView.highlightScrollbar();
		}
	}
	GListView {
		id: listView

		Accessible.ignored: false
		displayMarginBeginning: Constants.pane_padding
		displayMarginEnd: Constants.pane_padding
		model: ApplicationModel.getLicenseText()

		delegate: Item {
			implicitHeight: delegateText.implicitHeight
			implicitWidth: root.width

			GPaneBackgroundDelegate {
				anchors.centerIn: parent
				anchors.horizontalCenterOffset: -Constants.pane_padding
				count: listView.count
				height: delegateText.implicitHeight
				idx: index
				width: Math.min(listView.width - Constants.pane_padding, Style.dimens.max_text_width)

				GText {
					id: delegateText

					anchors.fill: parent
					bottomPadding: parent.isLast ? Constants.pane_padding : 0
					leftPadding: Constants.pane_padding
					rightPadding: Constants.pane_padding
					text: modelData
					topPadding: parent.isFirst ? Constants.pane_padding : 0
				}
			}
		}

		anchors {
			bottomMargin: Constants.pane_padding
			fill: parent
			leftMargin: Constants.pane_padding
			topMargin: Constants.pane_padding
		}
	}
	layer {
		enabled: true

		effect: GDropShadow {
		}
	}
}
