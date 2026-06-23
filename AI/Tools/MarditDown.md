## 安装

```
所以优先安装项目依赖文件：
cd D:\WorkSpace\Git\markitdown\apps\api

.\.venv\Scripts\python.exe -m pip install --upgrade pip
.\.venv\Scripts\python.exe -m pip install -r requirements.txt

然后重新启动：

.\.venv\Scripts\python.exe -m uvicorn main:app --reload --host 127.0.0.1 --port 8000



建议你现在一次性执行这一组
cd D:\WorkSpace\Git\markitdown\apps\api

.\.venv\Scripts\python.exe -m pip install --upgrade pip
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
.\.venv\Scripts\python.exe -m pip install Markdown bleach fastapi "uvicorn[standard]" python-multipart

.\.venv\Scripts\python.exe -m uvicorn main:app --reload --host 127.0.0.1 --port 8000

```

