# 使用说明

```
\
@ponytail full
  ↓
$grill-with-docs
  ↓
$to-prd
  ↓
$to-issues
  ↓
$tdd
  ↓
@ponytail-review
  ↓
$diagnosing-bugs
  ↓
$handoff

意思是：

grill-with-docs 负责问清楚；
to-prd 负责写清楚；
to-issues 负责拆小；
tdd 负责安全实现；
ponytail 负责防止过度设计；
ponytail-review 负责审查“哪些代码其实不该写”；
diagnosing-bugs 负责出问题时按证据修；
handoff 负责最后交接。
```

# 更强约束版，防止 Codex 乱封装

```
@ponytail ultra
$tdd

实现 Dashboard Basic Info 刷新按钮。

强约束：
1. 禁止新增 composable，除非已有同类 composable 必须复用；
2. 禁止新增 pinia store；
3. 禁止新增 npm 依赖；
4. 禁止新增通用 Button 组件；
5. 禁止重构 Dashboard 页面；
6. 禁止修改无关页面；
7. 优先在现有 Dashboard 文件和现有 API 文件中完成；
8. 如果现有接口已经能满足需求，不新增后端接口；
9. 如果只需要一行 API 调用，不要包装成三层服务；
10. 代码改动越少越好。

验收：
1. 页面上能看到刷新按钮；
2. 点击后请求后端；
3. loading 不造成页面闪烁；
4. 返回成功后刷新 Basic Info；
5. 失败时有错误提示；
6. pnpm build 通过。

开始前先检查当前代码。如果发现已有能力可以复用，直接复用，不要重复造。
```

