module("luci.controller.guest-wifi-ntagmod", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/guest-wifi-ntagmod") then
		return
	end
	
	entry({"admin", "network", "guest-wifi-ntagmod"}, cbi("guest-wifi-ntagmod"), translate("Guest-wifi-ntagmod"), 19).dependent = true
end
