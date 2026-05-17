#### 安装依赖库

```
npm install lucide-react
# 使用 legacy-peer-deps 标志，按照 npm v6 的宽松规则安装
npm install @react-pdf-viewer/core @react-pdf-viewer/default-layout pdfjs-dist --legacy-peer-deps

RUN apt-get update && apt-get install -y libmagickwand-dev && pecl install imagick && docker-php-ext-enable imagick 安装 php-fpm
```





#### ai 提问记录

```
https://pro.ant.design/zh-cn/docs/openapi/ react 中，使用 Mock 给出参考示例，并结合 https://pro.ant.design/zh-cn/docs/openapi/ 开源项目。

npm run openapi 执行后，则前端停止了，怎么验证mock 的测试数据，怎么再次启动。
怎么修改所有的路由权限，从后端返回的权限数据，前端页面打开 layout: false,

根据后端返回的 json 数据，动态控制是 layout: false 或者 layout: true 修改哪里

参考项目 https://gitee.com/whiteshader/ruoyi-react 后台新增业务

前端采用React 18、Ant Design Pro 6、TypeScript 5。
后端采用JDK17, Spring Boot v3、Spring Security、Redis & Jwt。

参考项目 https://gitee.com/whiteshader/ruoyi-react 后台新增业务 用户图片表，用户文件表，用户有图片和PDF文件，支持个文件，设计数据表结构，参考 用户岗位表

https://www.neea.edu.cn/res/Home/2501/285001f3dddda55ce5fd05169976cd24.pdf?download=%E4%BA%8C%E7%BA%A7Web%E7%A8%8B%E5%BA%8F%E8%AE%BE%E8%AE%A1%E6%A0%B7%E9%A2%98%E5%8F%8A%E5%8F%82%E8%80%83%E7%AD%94%E6%A1%88.pdf

参考项目 https://gitee.com/whiteshader/ruoyi-react 安装好了插件，npm install react-photo-album yet-another-react-lightbox  写出兼容 ruoyi-react 的 示例 demo， 能够使用  react-photo-album 展示图片，yet-another-react-lightbox 打开图片后，使用灯箱 可以下一张导航，




```



仓库

```
https://github.com/TimboKZ/chonky-website/tree/master
https://github.com/react-dropzone/react-dropzone
https://github.com/igordanchenko/react-photo-album
```





#### 首页展示

![image-20260421093617199](E:\Git\learning-record\MCU\Project\assets\image-20260421093617199.png)

报工：数量，良率。

工艺路线，产能数据，批次，报工，派工等信息。

产品编码，产品名称，型号，规格，计划数量，人员信息。





项目对应目录

```
vue-view E:\WorkSpace\esop\zip\RuoYi-Vue3-master
vue-admin E:\WorkSpace\esop\zip\RuoYi-Vue-master (1)
react-view E:\WorkSpace\esop\zip\ruoyi-react-master
react-admin E:\WorkSpace\esop\zip\ruoyi-react-master

vue https://gitee.com/y_project/RuoYi-Vue
react https://gitee.com/whiteshader/ruoyi-react 

```



#### 用户数据返回格式

```
{
  "@type": "com.ruoyi.common.core.domain.model.LoginUser",
  "browser": "Chrome 14",
  "deptId": 103,
  "expireTime": 1777355973644,
  "ipaddr": "127.0.0.1",
  "loginLocation": "内网IP",
  "loginTime": 1777354173644,
  "os": "Windows 10",
  "permissions": "Se",
  "token": "792c9e2a-82dc-48ef-b2a8-42ce473d7879",
  "user": {
    "admin": true,
    "avatar": "/profile/avatar/2026/04/25/blob_20260425095636A001.png",
    "createBy": "admin",
    "createTime": "2026-04-21 08:03:35",
    "delFlag": "0",
    "dept": {
      "ancestors": "0,100,101",
      "children": [],
      "deptId": 103,
      "deptName": "研发部门",
      "leader": "若依",
      "orderNum": 1,
      "params": {
        "@type": "java.util.HashMap"
      },
      "parentId": 101,
      "status": "0"
    },
    "work": {
      "ancestors": "0,100,101",
      "children": [],
      "deptId": 103,
      "deptName": "研发部门",
      "leader": "若依",
      "orderNum": 1,
      "params": {
        "@type": "java.util.HashMap"
      },
      "parentId": 101,
      "status": "0"
    },
    "work_files": [{
      "admin": true,
      "dataScope": "1",
      "deptCheckStrictly": false,
      "flag": false,
      "menuCheckStrictly": false,
      "params": {
        "@type": "java.util.HashMap"
      }
    }],
    "work_image": [{
      "admin": true,
      "dataScope": "1",
      "deptCheckStrictly": false,
      "flag": false,
      "menuCheckStrictly": false,
      "params": {
        "@type": "java.util.HashMap"
      }
    }],
    "deptId": 103,
    "email": "ry@163.com",
    "loginDate": "2026-04-28 13:27:46",
    "loginIp": "127.0.0.1",
    "nickName": "若依",
    "params": {
      "@type": "java.util.HashMap"
    },
    "password": "$2a$10$7JB720yubVSZvUI0rEqK/.VqGOZTH.ulu33dHOiBE8ByOhJIrdAu2",
    "phonenumber": "15888888888",
    "remark": "管理员",
    "roles": [{
      "admin": true,
      "dataScope": "1",
      "deptCheckStrictly": false,
      "flag": false,
      "menuCheckStrictly": false,
      "params": {
        "@type": "java.util.HashMap"
      },
      "roleId": 1,
      "roleKey": "admin",
      "roleName": "超级管理员",
      "roleSort": 1,
      "status": "0"
    }],
    "sex": "1",
    "status": "0",
    "userId": 1,
    "userName": "admin"
  },
  "userId": 1,
  "username": "admin"
}
```

