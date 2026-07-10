# 微信读书导出工具

```
cd E:\Git\weread-exporter-main

$chromeCandidates = @(
    "$env:ProgramFiles\Google\Chrome\Application",
    "${env:ProgramFiles(x86)}\Google\Chrome\Application",
    "$env:LOCALAPPDATA\Google\Chrome\Application"
)

$chromeDir = $chromeCandidates |
    Where-Object { Test-Path (Join-Path $_ "chrome.exe") } |
    Select-Object -First 1

if (-not $chromeDir) {
    throw "没有找到 chrome.exe，请确认 Google Chrome 已经安装。"
}

$env:Path = "$chromeDir;$env:Path"

Write-Host "Chrome 路径：$chromeDir"
where.exe chrome
chrome.exe --version

python -m weread_exporter -b 40932a30813abb609g016b31kf7132c6022cf7177163c01c -o epub -o pdf

https://github.com/drunkdream/weread-exporter 参考项目
```

