rule Cobalt_Strike_NewBeacon {
  meta:
    author = "CloudSafe"
    description = "Place EXE, DLL, and INI as a set in the same folder. When the EXE is executed, this rule detects artifacts that are deployed in the EXE process memory."

  strings:
    $c1 = { CC CC 40 53 55 56 57 41 54 41 55 41 56 41 57 }
    $s1 = "69W8Y3G469RVG2W2="
    $s2 = "v8.20"
    $s3 = "MYhXSMGVvcr"
    $s4 = "VPGH7WQQPR"
    $s5 = "7PtOTMdABvA=="
    $s6 = "v5.10"
    $s7 = "datasophos"
    $s8 = "VPGH7WQQPR"
    $s9 = "STE7U5WILZII"

  condition:
    $c1 or $s1 or $s2 or $s3 or $s4 or $s5 or $s6 or $s7 or $s8 or $s9
}
