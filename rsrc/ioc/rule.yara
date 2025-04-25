rule Cobalt_Strike_NewBeacon {
  meta:
    author = "CloudSafe"
    description = "Place EXE, DLL, and INI as a set in the same folder. When the EXE is executed, this rule detects artifacts that are deployed in the EXE process memory."

  strings:
    $c1 = { CC CC 40 53 55 56 57 41 54 41 55 41 56 41 57 }
    $s1 = "69W8Y3G469RVG2W2="
    $s2 = "v8.20"
    $s3 = "MYhXSMGVvcr7PtOTMdABvA=="
    $s4 = "v5.10"
    $s5 = "VPGH7WQQPR"
    $s6 = "STE7U5WILZII"
    $s7 = "MYhXSMGV"
    $s8 = "jli.dll"
    $s9 = "datasophos"

  condition:
    $c1 or $s1 or $s2 or $s3 or $s4 or $s5 or $s6 or $s7 or $s8 or $s9
}
