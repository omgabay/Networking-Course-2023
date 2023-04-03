#define UTC 0
#define EST -5
#define CET 2
#define AST 3 

void GetTime(const time_t& rawTime, char* buffer);


void GetTimeWithoutDate(const time_t& rawTime, char* buffer);

void GetTimeWithoutDateInCity();

void GetTimeInCity(std::string cityName, const time_t& rawTime, char* buffer);




void GetTime_HH_MM(const time_t& rawTime, char* buffer);

void GetYear(const time_t& rawTime, char* buffer);

void GetMonthAndDay(const time_t& rawTime, char* buffer);

void GetSecondsSinceMonthStart(const time_t& rawTime, char* buffer);

void GetWeekOfYear(const time_t& rawTime, char* buffer);

void GetDaylightSaving(const time_t& rawTime, char* buffer);

static void GetTimeSinceEpoch(time_t currentTime, char* sendBuff);



bool iequals(const string& a, const string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}
