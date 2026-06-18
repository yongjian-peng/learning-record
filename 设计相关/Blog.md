# 博客提交命令

```
cd E:\Git\gblog

Remove-Item Env:S3_ENABLE -ErrorAction SilentlyContinue
Remove-Item Env:S3_ENDPOINT -ErrorAction SilentlyContinue
Remove-Item Env:S3_BUCKET -ErrorAction SilentlyContinue
Remove-Item Env:S3_ACCESS_KEY -ErrorAction SilentlyContinue
Remove-Item Env:S3_SECRET_ACCESS_KEY -ErrorAction SilentlyContinue
Remove-Item Env:ASTRO_IMAGE_OPTIMIZE -ErrorAction SilentlyContinue

Remove-Item -Recurse -Force dist -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force .astro -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force .vite -ErrorAction SilentlyContinue

pnpm run build

cd E:\Git\yongjian-peng.github.io

Get-ChildItem -Force |
  Where-Object {
    $_.Name -notin @('.git', 'CNAME')
  } |
  Remove-Item -Recurse -Force

Copy-Item -Path E:\Git\gblog\dist\* -Destination E:\Git\yongjian-peng.github.io -Recurse -Force
New-Item -ItemType File -Path E:\Git\yongjian-peng.github.io\.nojekyll -Force

git add .
git commit -m "Deploy Astro blog"
git push origin main

https://github.com/mearashadowfax/ScrewFast
ScrewFast
```

