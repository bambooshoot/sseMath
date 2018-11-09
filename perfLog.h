#pragma once

#include <string>
#include <map>
#include <ctime>
#include <stdio.h>

struct TimeMark
{
	time_t lastMoment;
	double accTime;
	TimeMark() : accTime(0) { lastMoment = time(NULL); };
};

class PerfLog
{
public:
	PerfLog(const std::string & fileName)
	{
		fileId = fopen(fileName.c_str(), "w");
	}
	~PerfLog() {
		Write();
		fclose(fileId);
	}

	void TraceBegin(const std::string & key) 
	{
		if (logMap.find(key) == logMap.end()) {
			logMap.insert(std::pair<std::string, TimeMark>(key,TimeMark()));
		}
		logMap[key].lastMoment = time(NULL);
	}
	void TraceEnd(const std::string & key)
	{
		time_t endTime = time(NULL);
		logMap[key].accTime += endTime - logMap[key].lastMoment;
		logMap[key].lastMoment = endTime;
	}
	void PrintStr(const std::string & key,const std::string & str)
	{
		txtMap[key] = str;
	}
	void PrintFlt(const std::string & key, const float value)
	{
		fltMap[key] = value;
	}
	void Write()
	{
		for (auto tm : logMap) {
			fprintf(fileId, "%s time: %f sec(s) \n", tm.first.c_str(), tm.second.accTime);
		}
		for (auto str : txtMap) {
			fprintf(fileId, "%s %s\n", str.first.c_str(), str.second.c_str());
		}
		for (auto value : fltMap) {
			fprintf(fileId, "%s %f\n", value.first.c_str(), value.second);
		}
		fflush(fileId);
	} 
public:
	FILE * fileId;
	std::map<std::string, TimeMark> logMap;
	std::map<std::string, std::string> txtMap;
	std::map<std::string, float> fltMap;

	static PerfLog perfLog;
};