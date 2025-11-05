# UTF-8 Post-Processing Fix Documentation

## Issue #1429: Non-ASCII Characters in File Paths

### Problem Description
When downloading files with post-processing enabled, Parabolic fails if the download path contains non-ASCII characters (e.g., Hungarian accents, Cyrillic, CJK characters, etc.).

### Root Cause
The `{filepath}` variable substitution in post-processor arguments is not properly handling UTF-8 encoded paths. When passed to yt-dlp/ffmpeg, the path gets incorrectly split into multiple arguments, causing the post-processing command to fail.

**Example of the issue:**
Instead of:
```
'--postprocessor-args', "VideoConverter+ffmpeg:-i {filepath} ..."
```

We get:
```
'--postprocessor-args', "VideoConverter+ffmpeg:-i magyar felirattal.mp4 -y"
```

The path with special characters gets split into separate words.

### Solution Approach
1. **Proper String Encoding**: Ensure all file paths are properly encoded as UTF-8 when substituting `{filepath}`
2. **Quote Handling**: The filepath should be properly quoted when passed to shell/subprocess
3. **C++ String Handling**: Review the code in `downloadmanager.cpp` and `postprocessorargument.cpp` where variable substitution occurs
4. **Testing**: Add unit tests with international characters in filenames

### Files to Review
- `libparabolic/src/models/downloadmanager.cpp` - Main download logic
- `libparabolic/src/models/postprocessorargument.cpp` - Argument building
- `libparabolic/include/models/postprocessorargument.hpp` - Header file

### Recommended Implementation
- Use proper UTF-8 string handling in C++
- When substituting variables, ensure proper escaping for shell/subprocess calls
- Consider using std::format or similar for safe string substitution
- Add validation tests with Unicode filenames

### Testing
Test with filenames containing:
- Hungarian accents: `magyar_felirattal.mp4`
- Cyrillic: `видео_файл.mp4`
- CJK: `视频_文件.mp4`
- Mixed: `Видео-magyar_αρχείο.mp4`

---
**Status**: Open - Awaiting implementation in V2025.10.4 or later
