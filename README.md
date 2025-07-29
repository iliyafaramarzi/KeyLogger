# KeyLogger

A lightweight, educational key logger written in C++ using native Windows API. This tool captures keystrokes **before they are processed by the operating system** and logs them to a file. It runs silently in the background with **no visible window or console**, and can be accessed via the system tray (under hidden icons).

> ⚠️ **Disclaimer:** This project is for educational purposes only. Do not use it for malicious activities.

---

## 🛠️ How to Use

1. **Clone the project.**

2. **Build it in Release mode** using your preferred C++ development environment (e.g., MSVC or MinGW).

3. **Add the compiled binary to Windows startup** to run automatically on boot.

---

## 🚀 Add to Windows Startup

To make the program launch at system startup:

1. Open the **Registry Editor** (`regedit.exe`).

2. Navigate to the following path:
   
   mathematica
   
   CopyEdit
   
   `HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run`

3. Create a **new `String Value`** (Right-click > New > String Value).

4. Name it as desired (e.g., `KeyLogger`), and set its value to the **full path** of your compiled executable (e.g., `C:\Path\To\KeyLogger.exe`).

✅ Done! The application will now start automatically with Windows.

---

## 📄 Accessing the Log

- You can open the log file by **right-clicking the icon in the system tray** and selecting the appropriate option (note: the icon may appear blank or hidden under the "Show hidden icons" menu).

---

## 📬 Contact Me

- **Email:** iliyafaramarzi1384@gmail.com

- **Telegram:** @iliyafaramarzi


