# 重新创建Java

```
docker compose up -d \
  --no-deps \
  --force-recreate \
  board-java
```

重新创建React

```
docker compose up -d \
  --no-deps \
  --force-recreate \
  board-react-web
```

#  检查 Compose 配置

```
docker compose config --quiet
```

