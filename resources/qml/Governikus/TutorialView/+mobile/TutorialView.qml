import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtQml.Models 2.10
import QtQml 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	property int lastYPosition: 0
	property var lastVisibleItem
	property int contentWidth: Constants.is_tablet ? root.width * 0.5 : root.width

	leftTitleBarAction: TitleBarAction {
		state: topLevelPage ? "" : "back";
		onClicked: state == "back" ? leaveView() : root.state = ""
	}
	headerTitleBarAction: TitleBarAction { id: header; text: qsTr("Tutorial") + settingsModel.translationTrigger; font.bold: true }

	onVisibleChanged: {
		if (visible) {
			flickable.contentY = lastYPosition
			navBar.lockedAndHidden = true
		} else {
			lastYPosition = flickable.contentY
		}
	}

	Component.onCompleted: {
		if (visible) {
			navBar.lockedAndHidden = true
		}
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
			TutorialCollapseAnimation { targetContent: whereContent }
			TutorialCollapseAnimation { targetContent: howContent }
			TutorialCollapseAnimation { targetContent: importantContent }
		},
		Transition {
			to: "where"
			TutorialExpandAnimation {
				targetContent: whereContent
				targetHeader: whereHeader
			}
			TutorialCollapseAnimation { targetContent: whatContent }
			TutorialCollapseAnimation { targetContent: howContent }
			TutorialCollapseAnimation { targetContent: importantContent }
		},
		Transition {
			to: "how"
			TutorialExpandAnimation {
				targetContent: howContent
				targetHeader: howHeader
			}
			TutorialCollapseAnimation { targetContent: whatContent }
			TutorialCollapseAnimation { targetContent: whereContent }
			TutorialCollapseAnimation { targetContent: importantContent }
		},
		Transition {
			to: "important"
			TutorialExpandAnimation {
				targetContent: importantContent
				targetHeader: importantHeader
			}
			TutorialCollapseAnimation { targetContent: whatContent }
			TutorialCollapseAnimation { targetContent: whereContent }
			TutorialCollapseAnimation { targetContent: howContent }
		},
		Transition {
			to: ""
			NumberAnimation {
				target: flickable
				property: "contentY"
				easing.type: Easing.InOutQuad
				to: 0
				duration: 500
			}
			TutorialCollapseAnimation { targetContent: whatContent }
			TutorialCollapseAnimation { targetContent: whereContent }
			TutorialCollapseAnimation { targetContent: howContent }
			TutorialCollapseAnimation { targetContent: importantContent }
		}
	]

	function leaveView() {
		flickable.contentY = 0
		state = ""
		collapseAllAnimation.start()
		navBar.lockedAndHidden = false
		if (Constants.is_layout_ios){
			firePop()
		} else {
			navBar.state = "identify"
			navBar.currentIndex = 0
		}
	}

	SequentialAnimation {
		id: collapseAllAnimation

		NumberAnimation {
			target: flickable
			property: "contentY"
			easing.type: Easing.InOutQuad
			to: 0
			duration: 500
		}

		PropertyAction {
			target: whatContent
			property: "visible"
			value: false
		}

		PropertyAction {
			target: whereContent
			property: "visible"
			value: false
		}

		PropertyAction {
			target: howContent
			property: "visible"
			value: false
		}

		PropertyAction {
			target: importantContent
			property: "visible"
			value: false
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Constants.white
	}

	Flickable {
		id: flickable
		height: parent.height - footer.height
		width: root.width
		contentWidth: flickableContent.width
		contentHeight: flickableContent.height
		maximumFlickVelocity: Constants.scrolling_speed
		flickableDirection: Flickable.VerticalFlick

		Item {
			width: root.width

			Column {
				id: flickableContent
				width: root.width
				anchors.horizontalCenter: parent.horizontalCenter


				TutorialHeader {
					id: whatHeader
					width: root.width
					height: (flickable.height / 13.0 ) * 3.0
					headerImageSource: "qrc:///images/tutorial/main_menu_what_caret.svg"
					categoryAbove: false
					titleText: qsTr("What?") + settingsModel.translationTrigger
					initY: 0
					z: 40

					onClicked: {
						if (!whatContent.visible){
							root.state = "what"
						} else {
							root.state = ""
						}
					}

					miniIconSource: "qrc:///images/tutorial/icon_circle.svg"
					miniIconCoordinates: [
						{"x": 0.0625,   "y": 0.5},
						{"x": 0.1875,   "y": 0.15625},
						{"x": 0.2,   "y": 0.59375},
						{"x": 0.390625, "y": 0.78125},
						{"x": 0.65625,  "y": 0.15625},
						{"x": 0.703125, "y": 0.65625},
						{"x": 0.890625, "y": 0.625},
						{"x": 0.90625,  "y": 0.3125},
					]
				}

				TutorialWhat {
					id: whatContent
					width: root.contentWidth
					anchors.horizontalCenter: parent.horizontalCenter
				}

				TutorialHeader {
					id: whereHeader
					width: root.width
					height: (flickable.height / 13.0 ) * 3.0
					headerImageSource: "qrc:///images/tutorial/main_menu_where_caret.svg"
					titleText: qsTr("Where?") + settingsModel.translationTrigger
					initY: whatHeader.height
					z: 30

					onClicked: {
						if (!whereContent.visible){
							root.state = "where"
						} else {
							root.state = ""
						}
					}

					miniIconSource: "qrc:///images/tutorial/icon_star.svg"
					miniIconCoordinates: [
						{"x": 0.046875, "y": 0.34375},
						{"x": 0.1875,   "y": 0.09375},
						{"x": 0.21875,  "y": 0.65625},
						{"x": 0.4,      "y": 0.62},
						{"x": 0.55,     "y": 0.36},
						{"x": 0.65,     "y": 0.28125},
						{"x": 0.75,     "y": 0.5625},
						{"x": 0.890625, "y": 0.5}
					]
				}

				TutorialWhere {
					id: whereContent
					width: root.contentWidth
					anchors.horizontalCenter: parent.horizontalCenter
				}

				TutorialHeader {
					id: howHeader
					width: root.width
					height: (flickable.height / 13.0 ) * 3.0
					headerImageSource: "qrc:///images/tutorial/main_menu_how_caret.svg"
					titleText: qsTr("How?") + settingsModel.translationTrigger
					initY: whatHeader.height + whereHeader.height
					z: 20

					onClicked: {
						if (!howContent.visible){
							root.state = "how"
						} else {
							root.state = ""
						}
					}

					miniIconSource: "qrc:///images/tutorial/icon_box.svg"
					miniIconCoordinates: [
						{"x": 0.03125,  "y": 0.125},
						{"x": 0.078125, "y": 0.46875},
						{"x": 0.203125, "y": 0.4375},
						{"x": 0.32,     "y": 0.68},
						{"x": 0.64,     "y": 0.21875},
						{"x": 0.78125,  "y": 0.5625},
						{"x": 0.875,    "y": 0.0625},
						{"x": 0.9,      "y": 0.6}
					]
				}

				TutorialHow {
					id: howContent
					width: root.contentWidth
					anchors.horizontalCenter: parent.horizontalCenter

					onFirePush: {
						root.firePush(pSectionPage)
					}
					onQuitTutorialClicked: leaveView()
				}

				TutorialHeader {
					id: importantHeader
					width: root.width
					height: (flickable.height / 13.0 ) * 4.0
					overlapping: false
					headerImageSource: "qrc:///images/tutorial/main_menu_important_caret.svg"
					titleText: qsTr("Important!") + settingsModel.translationTrigger
					initY: whatHeader.height + whereHeader.height + howHeader.height
					z: 10

					onClicked: {
						if (!importantContent.visible){
							root.state = "important"
						} else {
							root.state = ""
						}
					}

					miniIconSource: "qrc:///images/tutorial/icon_diamond.svg"
					miniIconCoordinates: [
						{"x": 0.046875, "y": 0.46875},
						{"x": 0.14,     "y": 0.22},
						{"x": 0.25,     "y": 0.71875},
						{"x": 0.62,     "y": 0.7},
						{"x": 0.67,     "y": 0.24},
						{"x": 0.78125,  "y": 0.4375},
						{"x": 0.796875, "y": 0.65625},
						{"x": 0.9375,   "y": 0.1875}
					]
				}

				TutorialImportant {
					id: importantContent
					width: root.contentWidth
					anchors.horizontalCenter: parent.horizontalCenter
				}
			}
		}
	}

	TutorialFooter {
		id: footer
		width: root.width
		anchors.horizontalCenter: parent.horizontalCenter
		color: importantContent.visible && flickable.contentY > importantHeader.y - 1? Constants.tutorial_red
			 : howContent.visible && flickable.contentY > howHeader.y - 1? Constants.tutorial_blue
			 : whereContent.visible && flickable.contentY > whereHeader.y - 1? Constants.tutorial_green
			 : Constants.tutorial_orange
		anchors.bottom: parent.bottom
		backToMenuActive: root.state !== ""

		onMenuClicked: root.state = ""
		onQuitTutorialClicked: leaveView()
	}
}
