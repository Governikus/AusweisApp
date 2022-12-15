/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import QtQml 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: root

	property int contentWidth: Math.min(Style.dimens.max_text_width, root.width)
	property var lastVisibleItem
	property int lastYPosition: 0

	signal leave

	function leaveView() {
		flickable.contentY = 0;
		state = "";
		collapseAllAnimation.start();
		root.leave();
	}

	automaticSafeAreaMarginHandling: false
	//: LABEL ANDROID IOS
	title: qsTr("Tutorial")
	titleBarVisible: false

	navigationAction: NavigationAction {
		onClicked: root.state !== "" ? root.state = "" // collapse sections
		 : leaveView()
	}
	states: [
		State {
			name: "what"
		},
		State {
			name: "where"
		},
		State {
			name: "how"
		},
		State {
			name: "important"
		}
	]
	transitions: [
		Transition {
			to: "what"

			TutorialExpandAnimation {
				targetContent: whatContent
				targetHeader: whatHeader
			}
			TutorialCollapseAnimation {
				targetContent: whereContent
			}
			TutorialCollapseAnimation {
				targetContent: howContent
			}
			TutorialCollapseAnimation {
				targetContent: importantContent
			}
		},
		Transition {
			to: "where"

			TutorialExpandAnimation {
				targetContent: whereContent
				targetHeader: whereHeader
			}
			TutorialCollapseAnimation {
				targetContent: whatContent
			}
			TutorialCollapseAnimation {
				targetContent: howContent
			}
			TutorialCollapseAnimation {
				targetContent: importantContent
			}
		},
		Transition {
			to: "how"

			TutorialExpandAnimation {
				targetContent: howContent
				targetHeader: howHeader
			}
			TutorialCollapseAnimation {
				targetContent: whatContent
			}
			TutorialCollapseAnimation {
				targetContent: whereContent
			}
			TutorialCollapseAnimation {
				targetContent: importantContent
			}
		},
		Transition {
			to: "important"

			TutorialExpandAnimation {
				targetContent: importantContent
				targetHeader: importantHeader
			}
			TutorialCollapseAnimation {
				targetContent: whatContent
			}
			TutorialCollapseAnimation {
				targetContent: whereContent
			}
			TutorialCollapseAnimation {
				targetContent: howContent
			}
		},
		Transition {
			to: ""

			NumberAnimation {
				duration: 500
				easing.type: Easing.InOutQuad
				property: "contentY"
				target: flickable
				to: 0
			}
			TutorialCollapseAnimation {
				targetContent: whatContent
			}
			TutorialCollapseAnimation {
				targetContent: whereContent
			}
			TutorialCollapseAnimation {
				targetContent: howContent
			}
			TutorialCollapseAnimation {
				targetContent: importantContent
			}
		}
	]

	Component.onCompleted: {
		if (visible) {
			setLockedAndHidden();
		}
	}
	onVisibleChanged: {
		if (visible) {
			flickable.contentY = lastYPosition;
			setLockedAndHidden();
		} else {
			lastYPosition = flickable.contentY;
		}
	}

	SequentialAnimation {
		id: collapseAllAnimation
		NumberAnimation {
			duration: 500
			easing.type: Easing.InOutQuad
			property: "contentY"
			target: flickable
			to: 0
		}
		PropertyAction {
			property: "visible"
			target: whatContent
			value: false
		}
		PropertyAction {
			property: "visible"
			target: whereContent
			value: false
		}
		PropertyAction {
			property: "visible"
			target: howContent
			value: false
		}
		PropertyAction {
			property: "visible"
			target: importantContent
			value: false
		}
	}
	Rectangle {
		anchors.fill: parent
		color: Constants.white
	}
	GFlickable {
		id: flickable
		contentHeight: flickableContent.height
		contentWidth: flickableContent.width
		height: parent.height
		scrollBarBottomPadding: footer.height
		scrollBarTopPadding: plugin.safeAreaMargins.top
		topMargin: statusBar.height
		width: root.width

		Item {
			width: root.width

			Column {
				id: flickableContent
				anchors.horizontalCenter: parent.horizontalCenter
				width: root.width

				TutorialHeader {
					id: whatHeader
					categoryAbove: false
					headerImageSource: "qrc:///images/tutorial/main_menu_what_caret.svg"
					height: ((flickable.height - flickable.topMargin - footer.height) / 13.0) * 3.0
					initY: 0
					miniIconCoordinates: [{
							"x": 0.0625,
							"y": 0.5
						}, {
							"x": 0.1875,
							"y": 0.15625
						}, {
							"x": 0.2,
							"y": 0.59375
						}, {
							"x": 0.390625,
							"y": 0.78125
						}, {
							"x": 0.65625,
							"y": 0.15625
						}, {
							"x": 0.703125,
							"y": 0.65625
						}, {
							"x": 0.890625,
							"y": 0.625
						}, {
							"x": 0.90625,
							"y": 0.3125
						}]
					miniIconSource: "qrc:///images/tutorial/icon_circle.svg"
					//: LABEL ANDROID IOS
					titleText: qsTr("What?")
					width: root.width
					z: 40

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
					onClicked: {
						if (!whatContent.visible) {
							root.state = "what";
						} else {
							root.state = "";
						}
					}
				}
				TutorialWhat {
					id: whatContent
					anchors.horizontalCenter: parent.horizontalCenter
					width: root.contentWidth

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
				}
				TutorialHeader {
					id: whereHeader
					headerImageSource: "qrc:///images/tutorial/main_menu_where_caret.svg"
					height: ((flickable.height - flickable.topMargin - footer.height) / 13.0) * 3.0
					initY: whatHeader.height
					miniIconCoordinates: [{
							"x": 0.046875,
							"y": 0.34375
						}, {
							"x": 0.1875,
							"y": 0.09375
						}, {
							"x": 0.21875,
							"y": 0.65625
						}, {
							"x": 0.4,
							"y": 0.62
						}, {
							"x": 0.55,
							"y": 0.36
						}, {
							"x": 0.65,
							"y": 0.28125
						}, {
							"x": 0.75,
							"y": 0.5625
						}, {
							"x": 0.890625,
							"y": 0.5
						}]
					miniIconSource: "qrc:///images/tutorial/icon_star.svg"
					//: LABEL ANDROID IOS
					titleText: qsTr("Where?")
					width: root.width
					z: 30

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
					onClicked: {
						if (!whereContent.visible) {
							root.state = "where";
						} else {
							root.state = "";
						}
					}
				}
				TutorialWhere {
					id: whereContent
					anchors.horizontalCenter: parent.horizontalCenter
					width: root.contentWidth

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
				}
				TutorialHeader {
					id: howHeader
					headerImageSource: "qrc:///images/tutorial/main_menu_how_caret.svg"
					height: ((flickable.height - flickable.topMargin - footer.height) / 13.0) * 3.0
					initY: whatHeader.height + whereHeader.height
					miniIconCoordinates: [{
							"x": 0.03125,
							"y": 0.125
						}, {
							"x": 0.078125,
							"y": 0.46875
						}, {
							"x": 0.203125,
							"y": 0.4375
						}, {
							"x": 0.32,
							"y": 0.68
						}, {
							"x": 0.64,
							"y": 0.21875
						}, {
							"x": 0.78125,
							"y": 0.5625
						}, {
							"x": 0.875,
							"y": 0.0625
						}, {
							"x": 0.9,
							"y": 0.6
						}]
					miniIconSource: "qrc:///images/tutorial/icon_box.svg"
					//: LABEL ANDROID IOS
					titleText: qsTr("How?")
					width: root.width
					z: 20

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
					onClicked: {
						if (!howContent.visible) {
							root.state = "how";
						} else {
							root.state = "";
						}
					}
				}
				TutorialHow {
					id: howContent
					anchors.horizontalCenter: parent.horizontalCenter
					width: root.contentWidth

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
					onFirePush: pSectionPage => push(pSectionPage)
					onQuitTutorialClicked: leaveView()
				}
				TutorialHeader {
					id: importantHeader
					headerImageSource: "qrc:///images/tutorial/main_menu_important_caret.svg"
					height: ((flickable.height - flickable.topMargin - footer.height) / 13.0) * 4.0
					initY: whatHeader.height + whereHeader.height + howHeader.height
					miniIconCoordinates: [{
							"x": 0.046875,
							"y": 0.46875
						}, {
							"x": 0.14,
							"y": 0.22
						}, {
							"x": 0.25,
							"y": 0.71875
						}, {
							"x": 0.62,
							"y": 0.7
						}, {
							"x": 0.67,
							"y": 0.24
						}, {
							"x": 0.78125,
							"y": 0.4375
						}, {
							"x": 0.796875,
							"y": 0.65625
						}, {
							"x": 0.9375,
							"y": 0.1875
						}]
					miniIconSource: "qrc:///images/tutorial/icon_diamond.svg"
					overlapping: false
					//: LABEL ANDROID IOS
					titleText: qsTr("Important!")
					width: root.width
					z: 10

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
					onClicked: {
						if (!importantContent.visible) {
							root.state = "important";
						} else {
							root.state = "";
						}
					}
				}
				TutorialImportant {
					id: importantContent
					anchors.horizontalCenter: parent.horizontalCenter
					width: root.contentWidth

					Accessible.onScrollDownAction: flickable.scrollPageDown()
					Accessible.onScrollUpAction: flickable.scrollPageUp()
					onLetsGoClicked: leaveView()
					onShowPinManagement: {
						SettingsModel.transportPinReminder = false;
						root.show(UiModule.PINMANAGEMENT);
					}
				}

				// We could use a bottom margin instead of this rectangle, but that would result in the content suddenly
				// disappearing below the TutorialFooter at the end of the collapse animation (because the section's
				// content is NOT clipped between its two surrounding TutorialHeaders).
				Rectangle {
					color: Constants.white
					height: footer.height
					width: root.width
				}
			}
		}
	}
	TutorialStatusBar {
		id: statusBar
	}
	TutorialFooter {
		id: footer
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		backToMenuActive: root.state !== ""
		color: importantContent.visible && flickable.contentY > importantHeader.y - 1 ? Style.color.tutorial_important : howContent.visible && flickable.contentY > howHeader.y - 1 ? Style.color.tutorial_how : whereContent.visible && flickable.contentY > whereHeader.y - 1 ? Style.color.tutorial_where : Style.color.tutorial_what
		width: root.width

		onMenuClicked: root.state = ""
		onQuitTutorialClicked: leaveView()
	}
}
