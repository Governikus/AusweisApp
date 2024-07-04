#!/usr/bin/osascript

on run argv
	tell application "Finder"
		set diskname to item 1 of argv
		tell disk diskname
			open
			set current view of container window to icon view
			set toolbar visible of container window to false
			set statusbar visible of container window to false
			set pathbar visible of container window to true
			set bounds of container window to {0, 100, 500, 476}
			set theViewOptions to the icon view options of container window
			set arrangement of theViewOptions to not arranged
			set icon size of theViewOptions to 70

			set background picture of theViewOptions to file ".background:background.png"

			set file_list to every file
			repeat with i in file_list
				if the name of i is "Applications" then
					set the position of i to {350, 154}
				else if the name of i ends with ".app" then
					set the position of i to {152, 154}
				end if
			end repeat
			update without registering applications
			delay 4
			close
		end tell
	end tell
end run

