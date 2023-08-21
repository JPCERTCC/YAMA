rule APT10_ANEL_InitRoutine {
      meta:
        description = "ANEL malware"
        author = "JPCERT/CC Incident Response Group"
        hash = "2371f5b63b1e44ca52ce8140840f3a8b01b7e3002f0a7f0d61aecf539566e6a1"

    	strings:
    		$GetAddress = { C7 45 ?? ?? 69 72 74 C7 45 ?? 75 61 6C 50 C7 45 ?? 72 6F 74 65 66 C7 45 ?? 63 74 [3-4] C7 45 ?? ?? 65 72 6E C7 45 ?? 65 6C 33 32 C7 45 ?? 2E 64 6C 6C [3-4] FF 15 ?? ?? ?? ?? 50 FF 15 ?? ?? ?? ?? }
    	condition:
    		$GetAddress
}