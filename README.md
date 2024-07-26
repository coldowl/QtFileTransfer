# QtFileTransfer🚀

## Overview 📚

This is a practice project aimed at demonstrating the fundamentals of network programming using Qt C++. Please do not consider this as a mature product! 🚧

The project includes a file client and a file server. The client and server communicate using either TCP or UDP protocols. The server shares a directory, allowing clients to connect, preview the directory in real-time, upload files to the directory, and download files from the directory. The project supports file resumption and integrity checking. 🌐

## Features ✨

1. **Real-time Directory Preview:** Clients can see the server's shared directory in real-time. 👀
2. **File Upload:** Clients can upload files from their local machine to the server's directory. ⬆️
3. **File Download:** Clients can download files from the server's directory. ⬇️
4. **Resumable Transfers:** Supports resuming interrupted file uploads and downloads. 🔄
5. **File Integrity Checking:** Ensures the uploaded and downloaded files are intact and uncorrupted. ✅

## Components 🛠️

### File Server 🖥️

- **Directory Sharing:** Shares a specified directory with clients. 📂
- **TCP/UDP Communication:** Supports both TCP and UDP protocols for client-server communication. 🌐
- **File Management:** Manages file uploads and downloads, ensuring integrity and supporting resumption. 🔄

### File Client 💻

- **Directory Preview:** Connects to the server and previews the shared directory. 👁️
- **File Upload:** Allows users to upload files to the server's shared directory. ⬆️
- **File Download:** Allows users to download files from the server's shared directory. ⬇️
- **Resumable Transfers:** Supports resuming interrupted transfers. 🔄
- **File Integrity Checking:** Verifies file integrity after transfers. ✅

## Getting Started 🚀

### Prerequisites 📋

- Qt framework installed. 🛠️
- Basic understanding of C++ and Qt. 👩‍💻👨‍💻

### Installation 📥

1. **Clone the Repository:**
   ```bash
   git clone <repository_url>
   ```
2. **Open the Project:**
   Open the project in Qt Creator. 🖥️

3. **Build the Project:**
   Configure and build the project using Qt Creator. 🔧

### Running the Project ▶️

1. **Start the Server:**
   Run the server application. It will share a specified directory for clients to access. 🚀

2. **Start the Client:**
   Run the client application. Connect to the server using the server's IP address and port. 🌐

3. **Perform File Operations:**
   - **Preview Directory:** View the server's shared directory. 👀
   - **Upload Files:** Upload files from the client to the server. ⬆️
   - **Download Files:** Download files from the server to the client. ⬇️

## Usage 💡

1. **Connect to Server:**
   Enter the server's IP address and port to connect. 🌐

2. **Browse Directory:**
   Navigate the shared directory in real-time. 📂

3. **Upload File:**
   Select a file from your local machine and upload it to the server. ⬆️

4. **Download File:**
   Select a file from the server's directory and download it to your local machine. ⬇️

5. **Resume Transfers:**
   If a transfer is interrupted, restart the client and resume the transfer from where it left off. 🔄

6. **Check File Integrity:**
   Ensure the file transferred is complete and uncorrupted. ✅

## Note 📝

This is a practice project. While it includes fundamental functionalities, it is not intended for production use. ⚠️

## License 📜

This project is open-source and free to use under the MIT License. Feel free to contribute and enhance its functionalities! 🎉

