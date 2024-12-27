#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string key, unit, line;
  float memory, freeMemory, usedMemory, totalMemory;
  std::ifstream filestream(kOSPath + kMeminfoFilename);
   if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> memory) {
        if (key == "MemTotal") {
          totalMemory = memory;
        }
        else if (key == "MemFree") {
          freeMemory = memory;
        }
      }
    }
   }
  if (totalMemory == 0) {
    return 0.0;
  }
  else {
    return usedMemory = (totalMemory - freeMemory) / totalMemory;
  }
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  std::ifstream filestream(kOSPath + kUptimeFilename);
   if (filestream.is_open()) {
     filestream >> uptime;
   }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies = 0;
  vector<string> cpuUsage = LinuxParser::CpuUtilization();
  for (string jiffy : cpuUsage) {
    jiffies += std::stol(jiffy);
  }
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long activeJiffies;
  string line, field;
  vector<string> v;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
      if (std::getline(filestream, line)) {
        std::stringstream linestream(line);
        while (std::getline(linestream, field)) {
          v.push_back(field);
        }
        activeJiffies = std::stol(v[13]) + std::stol(v[14]) + std::stol(v[15]) + std::stol(v[16]);
      }
    }
  return activeJiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long activeJiffies;
  vector<string> cpuUsage = LinuxParser::CpuUtilization();
  activeJiffies = std::stol(cpuUsage[CPUStates::kUser_]) + std::stol(cpuUsage[CPUStates::kNice_]) + std::stol(cpuUsage[CPUStates::kSystem_]) + std::stol(cpuUsage[CPUStates::kIRQ_]) + std::stol(cpuUsage[CPUStates::kSoftIRQ_]) + std::stol(cpuUsage[CPUStates::kSteal_]);
  return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idleJiffies;
  vector<string> cpuUsage = LinuxParser::CpuUtilization();
  idleJiffies = std::stol(cpuUsage[CPUStates::kIdle_]) + std::stol(cpuUsage[CPUStates::kIOwait_]);
  return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string key;
  vector<string> cpuUsage;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    filestream >> key;
    if (key == "cpu") {
      for (int i = 0; i <= CPUStates::kGuestNice_; i++) {
        string value;
        filestream >> value;
        cpuUsage.push_back(value);
      }
    }
  }
  return cpuUsage;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcesses = 0;
  string key, line;
  std::ifstream filestream(kOSPath + kStatFilename);
   if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> totalProcesses) {
        if (key == "processes") {
          return totalProcesses;
        }
      }
    }
   }
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningProcesses = 0;
  string key, line;
  std::ifstream filestream(kOSPath + kStatFilename);
   if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> runningProcesses) {
        if (key == "procs_running") {
          return runningProcesses;
        }
      }
    }
   }
  return runningProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string comm = "None", line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
      if (std::getline(filestream, line)) {
        comm = line;
      }
  }
  return comm;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, ram, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> ram) {
        if (key == "VmSize") {
          return ram;
        }
      }
    }
   }
  return ram;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, uid = "-1", line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> uid) {
        if (key == "Uid") {
          return uid;
        }
      }
    }
   }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string name = "None", password, c_uid, line;
  string uid = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> password >> c_uid) {
        if (c_uid == uid) {
          return name;
        }
      }
    }
  }
  return name;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long upTime;
  string line, field;
  vector<string> v;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
      if (std::getline(filestream, line)) {
        std::stringstream linestream(line);
        while (std::getline(linestream, field)) {
          v.push_back(field);
        }
      }
    upTime = std::stol(v[22]);
  }
  return upTime / sysconf(_SC_CLK_TCK);
}