$binaryFile = $args[0]
$headerFile = $args[1]

$bytes = [System.IO.File]::ReadAllBytes($binaryFile)
$hexString = [BitConverter]::ToString($bytes).Replace("-", "")
$hexString = $hexString -replace '(.{2})', '0x$1, '

$headerContent = @"
unsigned char GeodeURIHandler[] = {
$hexString
};
"@

[System.IO.File]::WriteAllText($headerFile, $headerContent)