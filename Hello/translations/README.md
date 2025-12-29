# 多语言翻译说明

## 文件说明

- `Hello_zh_CN.ts` - 简体中文翻译文件（源文件）
- `Hello_en_US.ts` - 英文翻译文件（源文件）
- `Hello_zh_CN.qm` - 简体中文编译后的翻译文件（运行时使用）
- `Hello_en_US.qm` - 英文编译后的翻译文件（运行时使用）

## 使用方法

### 1. 生成翻译文件

运行项目根目录下的 `generate_translations.bat` 脚本：

```cmd
generate_translations.bat
```

这会：
- 从源代码中提取所有 `tr()` 包裹的字符串
- 更新 `.ts` 文件
- 编译生成 `.qm` 文件

### 2. 编辑翻译

使用 Qt Linguist 工具编辑 `.ts` 文件：

```cmd
C:\Qt\6.8.3\msvc2022_64\bin\linguist.exe translations\Hello_en_US.ts
```

或者直接用文本编辑器编辑 `.ts` 文件中的 `<translation>` 标签内容。

### 3. 重新编译翻译文件

编辑完 `.ts` 文件后，重新运行 `generate_translations.bat` 生成新的 `.qm` 文件。

### 4. 测试不同语言

程序会自动根据系统语言加载对应的翻译文件：
- 中文系统：加载 `Hello_zh_CN.qm`
- 英文系统：加载 `Hello_en_US.qm`

## 添加新语言

1. 在 `CMakeLists.txt` 中添加新的 `.ts` 文件：
   ```cmake
   set(TS_FILES
       translations/Hello_zh_CN.ts
       translations/Hello_en_US.ts
       translations/Hello_ja_JP.ts  # 例如：日语
   )
   ```

2. 创建新的翻译文件并翻译内容

3. 在 `Hello.qrc` 中添加新的 `.qm` 文件引用

4. 重新生成翻译文件

## 注意事项

- 所有需要翻译的字符串必须用 `tr()` 包裹
- 修改源代码中的字符串后，需要重新运行 `generate_translations.bat`
- `.qm` 文件是编译后的二进制文件，不要手动编辑
- `.ts` 文件应该提交到版本控制系统，`.qm` 文件可以忽略（构建时生成）
