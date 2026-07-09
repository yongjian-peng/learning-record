# 算立卡模型

```
https://github.com/sophgo/sophon-demo/tree/release/sample/YOLOv5

bm-smi

bmrt_test --bmodel XXXX.bmodel


MODEL="$HOME/sophon_models/yolov5/BM1684X/yolov5s_v6.1_3output_int8_4b.bmodel"

bmrt_test --bmodel "$MODEL" --devid 0 --loopnum 100

echo "bmrt_test exit code: $?"


https://doc.sophgo.com/sdk-docs/v26.03.01/docs_latest_release/docs/libsophon/guide/html/1_install.html#linux

 sudo 1spci -Wwv -s 81:00.0 | grep -E "LnkCap|LnkSta"
```

![image-20260703091133069](E:\Git\learning-record\Linux\Tools\assets\image-20260703091133069.png)

查看速率

![image-20260703091157392](E:\Git\learning-record\Linux\Tools\assets\image-20260703091157392.png)



查看速率 2

![image-20260703093335995](E:\Git\learning-record\Linux\Tools\assets\image-20260703093335995.png)

