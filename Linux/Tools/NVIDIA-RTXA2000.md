# 测试命令

```
./gpu_burn -m 80% -tc 60


watch -n 1 nvidia-smi


./gpu_burn -m 85% -tc 300

gpu-burn 的 -m 85% 表示使用可用显存的 85%，-tc 表示尝试使用 Tensor Core。
```

正常表现应为：

GPU-Util 接近 100%
性能状态由 P8 进入 P0 或 P2
功耗逐步接近 70W
显存占用约 5GB
输出中没有 FAULTY、error
温度升高后逐渐稳定



一、安装编译工具
sudo apt install -y build-essential git wget

检查 GCC：

gcc --version | head -n 1

查看文件：

ls -lh cuda_11.8.0_520.61.05_linux.run

文件大约数 GB，需要保证磁盘空间足够：

df -h /usr/local

三、只安装 Toolkit

执行：

sudo sh cuda_11.8.0_520.61.05_linux.run --silent --toolkit

这里的 --toolkit 表示只安装 CUDA Toolkit，不安装 runfile 中附带的旧版显卡驱动。默认安装位置是 /usr/local/cuda-11.8。


安装完成后检查：

ls -l /usr/local/cuda-11.8/bin/nvcc


四、配置环境变量

执行：

echo 'export PATH=/usr/local/cuda-11.8/bin:$PATH' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/cuda-11.8/lib64:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc

检查：

nvcc --version

正常应看到类似：

Cuda compilation tools, release 11.8


五、编译 gpu-burn

下载源码：

cd ~
git clone https://github.com/wilicc/gpu-burn.git

编译：

cd ~/gpu-burn
make COMPUTE=86 CUDAPATH=/usr/local/cuda-11.8

gpu-burn 官方支持通过 COMPUTE 指定计算能力，也支持通过 CUDAPATH 指定 CUDA Toolkit 路径。

检查结果：

ls -lh gpu_burn compare.fatbin

列出显卡：

./gpu_burn -l



图片一

![image-20260805180103654](E:\Git\learning-record\Linux\Tools\assets\image-20260805180103654.png)

图片二

![image-20260805180115877](E:\Git\learning-record\Linux\Tools\assets\image-20260805180115877.png)

图片三

![image-20260805180128407](E:\Git\learning-record\Linux\Tools\assets\image-20260805180128407.png)

图片四

![image-20260805180139197](E:\Git\learning-record\Linux\Tools\assets\image-20260805180139197.png)

图片五

![image-20260805180153310](E:\Git\learning-record\Linux\Tools\assets\image-20260805180153310.png)

图片六

![image-20260805180206137](E:\Git\learning-record\Linux\Tools\assets\image-20260805180206137.png)

