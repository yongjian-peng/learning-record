# 使用

```
--segment-seconds 600    每段 10 分钟
--whisper-model small    先跑通流程，速度比 medium 快
--device cpu             CPU 模式
--compute-type int8      CPU 推荐
--voice                  中文配音声音
--rate "+5%"             中文配音语速稍微加快
--burn-subtitles         烧录中文字幕

 .\.venv\Scripts\activate 进入 python 
 
 修复报错：
 cd E:\Git\video_translate_prod

deactivate
Remove-Item -Recurse -Force .\.venv

python -m venv .venv
.\.venv\Scripts\Activate.ps1

python -m pip install --upgrade pip
python -m pip install requests 
 
 
 python .\long_video_translate.py `
  --video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  --workdir "E:\Git\video_translate_pipeline\jobs" `
  --segment-seconds 60 `
  --whisper-model small `
  --device cpu `
  --compute-type int8 `
  --translator libretranslate `
  --libretranslate-url "http://192.168.126.135:5000/translate" `
  --source-lang en `
  --target-lang zh `
  --voice zh-CN-XiaoxiaoNeural `
  --rate "+5%" `
  --burn-subtitles
  
  分阶段执行：Whisper 识别英文
  视频 → 切音频 → Whisper 识别英文 → 生成英文 SRT
 .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage transcribe `
  -SegmentSeconds 60 `
  -WhisperModel small `
  -Device cpu `
  -ComputeType int8
  
  2. 分阶段执行：DeepL 翻译英文到中文
  英文识别结果 → DeepL 翻译 → 中文字幕
  .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage translate `
  -Translator deepl `
  -ForceStep translate
  
  
  3. 分阶段执行：Edge TTS 中文配音
  中文字幕 → Edge TTS → 中文配音音频片段
  .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage tts `
  -OutputMode dub `
  -Voice zh-CN-XiaoxiaoNeural `
  -Rate "+5%" `
  -ForceStep tts
  
  4. 分阶段执行：保留英文原音 + 烧录中文字幕
  原视频英文声音不变 → 烧录中文字幕 → 输出英文原音中文字幕视频
  .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage merge `
  -OutputMode subtitle `
  -BurnSubtitles `
  -ForceStep merge
  
  5. 分阶段执行：中文配音 + 合成/烧录字幕
  已有中文字幕 → Edge TTS 中文配音 → 合成中文配音视频 → 烧录中文字幕
 .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage tts-merge `
  -OutputMode both `
  -Voice zh-CN-XiaoxiaoNeural `
  -Rate "+5%" `
  -BurnSubtitles `
  -ForceStep tts
  
  
  6. 一次从翻译到合成：DeepL 翻译 + 英文原音中文字幕
  
 .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage translate-merge `
  -Translator deepl `
  -OutputMode subtitle `
  -BurnSubtitles `
  -ForceStep translate
  
  
  7. 完整流程：DeepL 翻译 + 中文配音版 + 英文原音中文字幕版
  .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_DbeY8lP-ftPUS0YL_720p.mp4" `
  -Workdir "E:\Git\video_translate_pipeline\jobs" `
  -Stage all `
  -SegmentSeconds 60 `
  -WhisperModel small `
  -Device cpu `
  -ComputeType int8 `
  -Translator deepl `
  -OutputMode both `
  -Voice zh-CN-XiaoxiaoNeural `
  -Rate "+5%" `
  -BurnSubtitles
  
  
  8. 完整流程：Whisper 识别英文字幕 + DeepLx 翻译 + 中文配音版 + 英文原音中文字幕版
  .\run_video_translate.ps1 `
  -Video "E:\Git\X2Twitter.com_QIQw1XDP2rDuTCoE_720p.mp4" `
  -Workdir "E:\Git\video_translate_prod\jobs" `
  -Stage all `
  -SegmentSeconds 60 `
  -WhisperModel small `
  -Device cpu `
  -ComputeType int8 `
  -Translator deeplx `
  -OutputMode subtitle `
  -DeepLXUrl "http://192.168.126.135:1188/translate" `
  -DeepLXToken "6b21d217-0359-4abc-8e45-a51e8de02f75:fx" `
  -BurnSubtitles
  
```







# 生成声音命令

```
Yunyang(Male/CN)     男声，稳重
Yunxi(Male/CN)       男声，较年轻
Yunjian(Male/CN)     男声，新闻/讲解感
Yunxia(Male/CN)      男声
Xiaoxiao(Female/CN)  女声，通用、自然
Xiaoyi(Female/CN)    女声
Xiaoni(Female/CN)    女声，陕西口音
Xiaobei(Female/CN)   女声，辽宁口音
```



```
pyVideoTrans 的代码中会调用 get_edge_rolelist(it['role'], self.language) 做映射；而它的中文声音映射键是 Yunyang(Male/CN)、Xiaoxiao(Female/CN) 这样的显示名称，不是 zh-CN-YunyangNeural

uv run cli.py --task tts --name "./narration.srt" --tts_type 0 --voice_role "Yunyang(Male/CN)" --target_language_code zh-cn


小晓
uv run cli.py --task tts --name "./narration.srt" --tts_type 0 --voice_role "Xiaoxiao(Female/CN)" --target_language_code zh-cn

合成音视频内容
ffmpeg -i ".\Video_2026-06-15_195030.wmv" -i ".\output\narration-srt\narration-20260622-102724.wav" -stream_loop -1 -i ".\bgm.mp3" -filter_complex "[0:a]volume=0.05[click];[1:a]volume=1.0[narr];[2:a]volume=0.08[bgm];[click][narr][bgm]amix=inputs=3:duration=longest:dropout_transition=2[a]" -vf "subtitles='narration.srt':force_style='FontName=Microsoft YaHei,FontSize=22,Outline=2,Alignment=2,MarginV=55'" -map 0:v:0 -map "[a]" -c:v libx264 -preset medium -crf 20 -pix_fmt yuv420p -c:a aac -b:a 192k -movflags +faststart -shortest ".\output_douyin.mp4"

```

# narration.srt

```
1
00:00:00,000 --> 00:00:05,000
大家好，下面演示这个软件的配置方法。

2
00:00:05,000 --> 00:00:12,000
首先打开软件主界面，点击右上角的设置按钮。

3
00:00:12,000 --> 00:00:20,000
进入配置页面后，找到保存路径设置选项。

4
00:00:20,000 --> 00:00:28,000
点击浏览按钮，然后选择 E 盘中的配置文件目录。

5
00:00:28,000 --> 00:00:36,000
确认保存后，重新启动软件，新的配置就会自动加载。
```

# 裁剪合并音频

```
# 建议加 1 秒淡入、2 秒淡出
ffmpeg -ss 00:01:38 -i ".\213370871.mp3" -t 00:02:00 -af "afade=t=in:st=0:d=1,afade=t=out:st=20:d=2" -c:a libmp3lame -q:a 2 ".\bgm_clip01.mp3"

# 从原音乐 03:00 开始，截取 60 秒 
ffmpeg -ss 00:03:00 -i ".\213370871.mp3" -t 00:01:00 -af "afade=t=in:st=0:d=1,afade=t=out:st=58:d=2" -c:a libmp3lame -q:a 2 ".\bgm_clip02.mp3"


# 顺序合并
ffmpeg -i ".\bgm_clip01.mp3" -i ".\bgm_clip02.mp3" -filter_complex "[0:a][1:a]concat=n=2:v=0:a=1[a]" -map "[a]" -c:a libmp3lame -q:a 2 ".\bgm_merged.mp3"
# 顺序合并+1秒交叉淡化， 总时长会比“两段时长相加”少 1 秒。
ffmpeg -i ".\bgm_clip01.mp3" -i ".\bgm_clip02.mp3" -filter_complex "[0:a][1:a]acrossfade=d=1[a]" -map "[a]" -c:a libmp3lame -q:a 2 ".\bgm_merged.mp3"

将视频合并 原视频鼠标点击声：3%， 背景音乐：55%， 讲解声音：90%， 字幕字体：微软雅黑， 字幕大小：FontSize=30， 字幕位置：底部，距底部约 60 像素
ffmpeg -i ".\Video_2026-06-15_195030.wmv" -i ".\output\narration-srt\narration-20260622-134549.wav" -stream_loop -1 -i ".\bgm_merged.mp3" -filter_complex "[0:a]volume=0.03[orig];[1:a]volume=0.90[narr];[2:a]volume=0.55[bgm];[orig][narr][bgm]amix=inputs=3:duration=first:dropout_transition=2[a]" -vf "subtitles=filename='narration.srt':charenc=UTF-8:force_style='FontName=Microsoft YaHei,FontSize=30,Outline=2,Shadow=1,Alignment=2,MarginV=60'" -map 0:v:0 -map "[a]" -c:v libx264 -preset medium -crf 20 -pix_fmt yuv420p -c:a aac -b:a 192k -movflags +faststart ".\video_narration_bgm_subtitle.mp4"



```

