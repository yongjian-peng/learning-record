Ubuntu20.04 使用 Clash

```
function main(config) {
    if (!config || !Array.isArray(config.proxies)) {
        return config;
    }

    // 所有 AnyTLS 节点
    const removedNames = new Set();

    for (const proxy of config.proxies) {
        if (
            proxy &&
            typeof proxy.type === "string" &&
            proxy.type.toLowerCase() === "anytls"
        ) {
            removedNames.add(proxy.name);
        }
    }

    // 删除 AnyTLS
    config.proxies = config.proxies.filter(proxy => {
        return !(
            proxy &&
            typeof proxy.type === "string" &&
            proxy.type.toLowerCase() === "anytls"
        );
    });

    // 清理代理组
    if (Array.isArray(config["proxy-groups"])) {
        config["proxy-groups"] = config["proxy-groups"].map(group => {
            if (Array.isArray(group.proxies)) {
                group.proxies = group.proxies.filter(
                    name => !removedNames.has(name)
                );
            }

            return group;
        });
    }

    console.log("===== Ubuntu 20.04 compatibility =====");
    console.log("Removed AnyTLS nodes:", Array.from(removedNames));
    console.log("Remaining proxies:", config.proxies.length);

    return config;
}
```

