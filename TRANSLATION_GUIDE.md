# 多语言支持实现指南

## 已完成的修改

### 1. 代码修改
所有中文字符串已用 `tr()` 包裹，支持多语言翻译：

- ✅ `Hello/Hello.cpp` - 主窗口的按钮和消息框文本
- ✅ `Hello/SerialWindow.cpp` - 串口窗口的所有界面文本
- ✅ `Hello/SerialWorker.cpp` - 串口工作类的错误消息
- ✅ `Hello/main.cpp` - 添加了翻译加载逻辑

### 2. 翻译文件
创建了中英文翻译文件：

- `translations/Hello_zh_CN.ts` - 简体中文
- `translations/Hello_en_US.ts` - 英文

### 3. 资源文件
更新了 `Hello/Hello.qrc`，包含编译后的翻译文件（.qm）

### 4. 构建配置
- 更新了 `CMakeLists.txt`，添加了 Qt LinguistTools 支持
- 创建了 `generate_translations.bat` 脚本用于生成翻译文件

## 使用步骤

### 第一次使用

1. **生成翻译文件**
   ```cmd
   generate_translations.bat
   ```

2. **编译项目**
   - Visual Studio: 直接编译
   - CMake: 运行 `build.bat`

3. **运行程序**
   - 程序会自动根据系统语言显示对应的界面文本

### 修改翻译

1. **使用 Qt Linguist 编辑**（推荐）
   ```cmd
   C:\Qt\6.8.3\msvc2022_64\bin\linguist.exe translations\Hello_en_US.ts
   ```

2. **或直接编辑 .ts 文件**
   用文本编辑器打开 `.ts` 文件，修改 `<translation>` 标签内容

3. **重新生成 .qm 文件**
   ```cmd
   generate_translations.bat
   ```

4. **重新编译项目**

### 添加新的可翻译文本

1. 在代码中用 `tr()` 包裹字符串：
   ```cpp
   QString text = tr("需要翻译的文本");
   ```

2. 重新生成翻译文件：
   ```cmd
   generate_translations.bat
   ```

3. 编辑 `.ts` 文件添加翻译

4. 重新编译

## 支持的语言

当前支持：
- 简体中文 (zh_CN) - 默认
- 英文 (en_US)

## 语言切换逻辑

程序启动时会：
1. 检测系统语言（通过 `QLocale::system().name()`）
2. 尝试加载对应的 `.qm` 文件
3. 如果找不到对应语言，使用源代码中的原始文本（中文）

## 文件结构

```
Hello/
├── translations/
│   ├── Hello_zh_CN.ts      # 中文翻译源文件
│   ├── Hello_zh_CN.qm      # 中文编译文件（自动生成）
│   ├── Hello_en_US.ts      # 英文翻译源文件
│   ├── Hello_en_US.qm      # 英文编译文件（自动生成）
│   └── README.md           # 翻译说明
├── Hello/
│   ├── Hello.qrc           # 资源文件（包含 .qm）
│   └── ...
├── generate_translations.bat  # 翻译生成脚本
└── TRANSLATION_GUIDE.md    # 本文档
```

## 注意事项

1. `.ts` 文件应该提交到版本控制
2. `.qm` 文件可以忽略（构建时自动生成）
3. 修改代码中的字符串后，必须重新运行 `generate_translations.bat`
4. 翻译文件必须使用 UTF-8 编码
5. 在 lambda 表达式中使用 `tr()` 时，确保上下文正确

## 测试

### 测试中文界面
在中文 Windows 系统上运行，或设置环境变量：
```cmd
set LANG=zh_CN
Hello.exe
```

### 测试英文界面
在英文 Windows 系统上运行，或设置环境变量：
```cmd
set LANG=en_US
Hello.exe
```

## 常见问题

**Q: 翻译不生效？**
A: 检查：
1. 是否运行了 `generate_translations.bat`
2. `.qm` 文件是否存在
3. `Hello.qrc` 是否包含了 `.qm` 文件
4. 项目是否重新编译

**Q: 新添加的文本没有翻译？**
A: 确保：
1. 文本用 `tr()` 包裹
2. 运行了 `generate_translations.bat`
3. 在 `.ts` 文件中添加了翻译
4. 重新编译了项目

**Q: 如何添加更多语言？**
A: 
1. 在 `CMakeLists.txt` 中添加新的 `.ts` 文件
2. 复制现有的 `.ts` 文件并修改语言代码
3. 翻译内容
4. 在 `Hello.qrc` 中添加新的 `.qm` 文件引用
