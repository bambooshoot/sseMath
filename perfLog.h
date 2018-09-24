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
	void Write()
	{
		for (auto tm : logMap) {
			fprintf(fileId, "%s time: %f sec(s) \n", tm.first.c_str(), tm.second.accTime);
		}
		fflush(fileId);
	}
public:
	FILE * fileId;
	std::map<std::string, TimeMark> logMap;

	static PerfLog perfLog;
};