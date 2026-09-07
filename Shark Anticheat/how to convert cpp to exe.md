# SHARK AntiCheat & Guardian — Build Instructions

Windows-এ MinGW (g++) ব্যবহার করে `SHARK_AntiCheat.cpp` ও `SHARK_Guardian.cpp` কম্পাইল করার সম্পূর্ণ গাইড।

## ১. MinGW ডাউনলোড করুন

নিচের রিলিজ পেজ থেকে সর্বশেষ বিল্ড ডাউনলোড করুন:

👉 [niXman/mingw-builds-binaries — Latest Release](https://github.com/niXman/mingw-builds-binaries/releases/latest)

নির্দিষ্ট ফাইল:

```
x86_64-16.1.0-release-win32-seh-msvcrt-rt_v14-rev1.7z
```

## ২. এক্সট্র্যাক্ট করুন

ডাউনলোড করা `.7z` ফাইলটি এক্সট্র্যাক্ট করে নিচের লোকেশনে রাখুন:

```
C:\mingw64
```

> ⚠️ এক্সট্র্যাক্ট করার পর নিশ্চিত করুন যে `C:\mingw64\bin` ফোল্ডারে `g++.exe` আছে। কিছু আর্কাইভে extract করলে একটা extra `mingw64` সাব-ফোল্ডার তৈরি হয় — সেক্ষেত্রে ভেতরের ফাইলগুলো `C:\mingw64` তে move করে নিন।

## ৩. সিস্টেম PATH-এ যোগ করুন

**Administrator** হিসেবে Command Prompt চালু করে নিচের কমান্ড রান করুন:

```cmd
setx PATH "%PATH%;C:\mingw64\bin" /M
```

এরপর **নতুন একটি Command Prompt / টার্মিনাল খুলুন** (পুরনোটাতে PATH আপডেট হয় না), তারপর যাচাই করুন:

```cmd
g++ --version
```

সঠিকভাবে ইনস্টল হলে g++ এর ভার্সন তথ্য দেখাবে।

## ৪. কম্পাইল করুন

প্রজেক্ট ফোল্ডারে গিয়ে নিচের কমান্ডগুলো রান করুন:

```cmd
g++ SHARK_AntiCheat.cpp -o SHARK_AntiCheat.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
g++ SHARK_Guardian.cpp -o SHARK_Guardian.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
```

কম্পাইল সফল হলে একই ফোল্ডারে `SHARK_AntiCheat.exe` এবং `SHARK_Guardian.exe` তৈরি হবে।

### লিঙ্ক করা লাইব্রেরি সম্পর্কে

| লাইব্রেরি | ব্যবহার |
|---|---|
| `ole32` | COM (Component Object Model) সাপোর্ট |
| `oleaut32` | OLE Automation ফাংশনসমূহ |
| `wininet` | ইন্টারনেট/HTTP রিকোয়েস্ট হ্যান্ডলিং |
| `wintrust` | ফাইল সিগনেচার/অথেন্টিকোড ভেরিফিকেশন |
| `wbemuuid` | WMI (Windows Management Instrumentation) কুয়েরি |

## সমস্যা সমাধান (Troubleshooting)

- **`'g++' is not recognized`** — PATH আপডেটের পর টার্মিনাল রিস্টার্ট করুন, প্রয়োজনে কম্পিউটার রিবুট করুন।
- **`undefined reference` এরর** — লাইব্রেরি ফ্ল্যাগের অর্ডার ঠিক আছে কিনা এবং `.cpp` ফাইলের নাম সঠিক কিনা যাচাই করুন।
- **`.7z` এক্সট্র্যাক্ট হচ্ছে না** — [7-Zip](https://www.7-zip.org/) ইনস্টল করে নিন।

---

**প্রয়োজনীয় টুলস:** [MinGW-w64 (niXman build)](https://github.com/niXman/mingw-builds-binaries) · [7-Zip](https://www.7-zip.org/)
