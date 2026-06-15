# 启动命令修改

```
执行：
notepad $PROFILE 
文件不存在，再执行：
New-Item -ItemType File -Force $PROFILE
notepad $PROFILE

在打开的文件最后加入
function claude {
    $env:HOME = $env:USERPROFILE
    $env:CLAUDE_CONFIG_DIR = "$env:USERPROFILE\.claude"
    & "$env:USERPROFILE\.local\bin\claude.exe" @args
}

保存后，关闭 PowerShell，重新打开。
验证：Get-Command claude -All


```

