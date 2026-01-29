/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

Colors {
	id: root

	background: "#232323"
	border: "#ffffff"
	error: "#eb0000"
	focus_indicator: "#0098eb"
	image: "#0098eb"
	success: "#5fcb01"
	warning: "#ff9b29"

	control: DefaultControlColors {
		background.basic_checked: "#232323"
		background.basic_unchecked: "#0098eb"
		border.basic_checked: "#ffffff"
		border.basic_unchecked: "#0098eb"
		content.basic_checked: "#ffffff"
		content.basic_unchecked: "#232323"
	}
	controlCheckbox: DefaultControlColors {
		background.basic_checked: root.transparent
		background.pressed_checked: "#80cdec"
		background.pressed_unchecked: "#80cdec"
		border.pressed_checked: "#80cdec"
		border.pressed_unchecked: "#80cdec"
		content.basic_checked: "#0098eb"
		content.basic_unchecked: "#ffffff"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	controlOptional: DefaultControlColors {
		background.basic_unchecked: root.transparent
		background.pressed_checked: "#0098eb"
		background.pressed_unchecked: "#0098eb"
		border.basic_unchecked: "#0098eb"
		border.pressed_checked: "#0098eb"
		border.pressed_unchecked: "#0098eb"
		content.basic_unchecked: "#0098eb"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	controlRadiobutton: DefaultControlColors {
		background.hovered_checked: "#3d3d3d"
		background.hovered_unchecked: "#3d3d3d"
		border.hovered_checked: "#ffffff"
		border.hovered_unchecked: "#ffffff"
		border.pressed_checked: "#ffffff"
		border.pressed_unchecked: "#ffffff"
	}
	controlScrollbar: DefaultControlColors {
		background.basic_checked: "#232323"
		background.basic_unchecked: "#0098eb"
		background.pressed_checked: "#80cdec"
		background.pressed_unchecked: "#80cdec"
		border.basic_checked: "#ffffff"
		border.basic_unchecked: "#0098eb"
		border.pressed_checked: "#80cdec"
		border.pressed_unchecked: "#80cdec"
		content.basic_checked: "#ffffff"
		content.basic_unchecked: "#232323"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	controlStagedProgressBar: DefaultControlColors {
		background.basic_unchecked: "#80cdec"
		border.basic_unchecked: root.transparent
		content.basic_unchecked: "#0098eb"
	}
	controlSwitch: DefaultControlColors {
		background.pressed_checked: "#80cdec"
		background.pressed_unchecked: "#80cdec"
		border.pressed_checked: "#80cdec"
		border.pressed_unchecked: "#80cdec"
		content.basic_unchecked: "#ffffff"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	linkBasic: DefaultLinkColors {
	}
	linkNavigation: DefaultLinkColors {
	}
	linkTitle: DefaultLinkColors {
		basic_unchecked: "#ffffff"
		hovered_checked: "#80cdec"
		hovered_unchecked: "#80cdec"
		pressed_checked: "#0098eb"
		pressed_unchecked: "#0098eb"
	}
	pane: DefaultPaneColors {
	}
	paneSublevel: DefaultPaneColors {
		background.basic_unchecked: root.pane.background.hovered_unchecked
		border.basic_unchecked: "#576164"
	}
	remoteIndicator: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#0098eb"
		disabled_checked: "#798183"
		disabled_unchecked: "#798183"
		hovered_checked: basic_unchecked
		hovered_unchecked: basic_unchecked
		pressed_checked: basic_unchecked
		pressed_unchecked: basic_unchecked
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		basic_checked: "#000000"
	}
	textSubline: DefaultTextColors {
		basic_unchecked: "#80cdec"
		disabled_checked: "#798183"
		disabled_unchecked: "#798183"
	}
	textTitle: DefaultTextColors {
		basic_checked: "#ffffff"
		basic_unchecked: "#80cdec"
		disabled_checked: "#798183"
		disabled_unchecked: "#798183"
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic_checked: "#0098eb"
			basic_unchecked: "#232323"
			disabled_checked: "#303030"
			disabled_unchecked: "#303030"
			hovered_checked: "#80cdec"
			hovered_unchecked: "#80cdec"
			pressed_checked: root.transparent
			pressed_unchecked: root.transparent
		}
		border: ComponentColors {
			basic_checked: "#0098eb"
			basic_unchecked: "#ffffff"
			disabled_checked: "#798183"
			disabled_unchecked: "#798183"
			hovered_checked: "#80cdec"
			hovered_unchecked: "#80cdec"
			pressed_checked: "#0098eb"
			pressed_unchecked: "#0098eb"
		}
		content: ComponentColors {
			basic_checked: "#232323"
			basic_unchecked: "#ffffff"
			disabled_checked: "#798183"
			disabled_unchecked: "#798183"
			hovered_checked: "#ffffff"
			hovered_unchecked: "#ffffff"
			pressed_checked: "#0098eb"
			pressed_unchecked: "#0098eb"
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#80cdec"
		disabled_checked: "#bcc0c1"
		disabled_unchecked: "#bcc0c1"
		hovered_checked: "#0098eb"
		hovered_unchecked: "#0098eb"
		pressed_checked: "#0b7ab7"
		pressed_unchecked: "#0b7ab7"
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic_checked: "#0098eb"
			basic_unchecked: "#303030"
			disabled_checked: "#303030"
			disabled_unchecked: "#303030"
			hovered_checked: root.lighter(basic_unchecked, 0.05)
			hovered_unchecked: root.lighter(basic_unchecked, 0.05)
			pressed_checked: root.lighter(basic_unchecked, 0.10)
			pressed_unchecked: root.lighter(basic_unchecked, 0.10)
		}
		border: ComponentColors {
			basic_checked: basic_unchecked
			basic_unchecked: root.software_renderer ? "#ffffff" : root.transparent
			disabled_checked: basic_unchecked
			disabled_unchecked: basic_unchecked
			hovered_checked: basic_unchecked
			hovered_unchecked: basic_unchecked
			pressed_checked: basic_unchecked
			pressed_unchecked: basic_unchecked
		}
	}
	component DefaultTextColors: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#ffffff"
		disabled_checked: "#bcc0c1"
		disabled_unchecked: "#bcc0c1"
		hovered_checked: basic_unchecked
		hovered_unchecked: basic_unchecked
		pressed_checked: basic_unchecked
		pressed_unchecked: basic_unchecked
	}
}
